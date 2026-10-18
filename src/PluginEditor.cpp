#include "PluginEditor.h"

#include "PluginProcessor.h"

class LightWeaverAudioProcessorEditor::StageViewPreview : public juce::Component {
 public:
  void paint(juce::Graphics& graphics) override {
    auto bounds = getLocalBounds().toFloat();
    graphics.fillAll(juce::Colour(0xff121215));

    graphics.setColour(juce::Colour(0xff29292f));
    constexpr float gridSpacing = 48.0f;
    for (float x = 0.0f; x < bounds.getWidth(); x += gridSpacing)
      graphics.drawVerticalLine(juce::roundToInt(x), bounds.getY(), bounds.getBottom());
    for (float y = 0.0f; y < bounds.getHeight(); y += gridSpacing)
      graphics.drawHorizontalLine(juce::roundToInt(y), bounds.getX(), bounds.getRight());

    graphics.setColour(juce::Colour(0xff70707a));
    graphics.setFont(juce::Font(juce::FontOptions(10.0f)));
    graphics.drawText("STAGE VIEW", getLocalBounds().reduced(8).removeFromTop(16),
                      juce::Justification::centredLeft, false);
  }
};

LightWeaverAudioProcessorEditor::LightWeaverAudioProcessorEditor(LightWeaverAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  headerComponent = std::make_unique<HeaderComponent>(audioProcessor.getAPVTS());
  headerComponent->onStageViewVisibilityChanged = [this](bool shouldBeVisible) {
    stageViewPreview->setVisible(shouldBeVisible);
    resized();
  };
  headerComponent->onKeyframeDragRequested = [this] { beginExternalMidiDrag(); };
  addAndMakeVisible(*headerComponent);

  stageViewPreview = std::make_unique<StageViewPreview>();
  addAndMakeVisible(*stageViewPreview);

  std::vector<FixtureCard::Config> fixtureConfigs{
      {"spot1", "SPOT 1", "clb4_master_dimmer", 1, 2, 3, 14, "CLB4 SPOTS  |  CC 1-12 + 14"},
      {"spot2", "SPOT 2", "clb4_master_dimmer", 4, 5, 6, 14, "CLB4 SPOTS  |  CC 1-12 + 14"},
      {"spot3", "SPOT 3", "clb4_master_dimmer", 7, 8, 9, 14, "CLB4 SPOTS  |  CC 1-12 + 14"},
      {"spot4", "SPOT 4", "clb4_master_dimmer", 10, 11, 12, 14, "CLB4 SPOTS  |  CC 1-12 + 14"},
      {"panel1", "PANEL 1", "panel1_dimmer", 16, 17, 18, 15, "FLOOD PANELS  |  CC 15-18"},
      {"panel2", "PANEL 2", "panel2_dimmer", 20, 21, 22, 19, "FLOOD PANELS  |  CC 19-22"},
  };
  fixtureContainer = std::make_unique<FixtureContainerComponent>(
      audioProcessor.getAPVTS(), std::move(fixtureConfigs),
      [this](int ccNumber, int value) { audioProcessor.enqueueGuiMidiCc(ccNumber, value); });
  fixtureViewport.setViewedComponent(fixtureContainer.get(), false);
  fixtureViewport.setScrollBarsShown(true, false);
  addAndMakeVisible(fixtureViewport);

  statusLabel.setJustificationType(juce::Justification::centred);
  statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
  statusLabel.setText("Live MIDI output ready", juce::dontSendNotification);
  addAndMakeVisible(statusLabel);

  setSize(1000, 700);
  setResizable(true, true);
  setResizeLimits(800, 500, 1920, 1080);
  startTimerHz(20);
}

LightWeaverAudioProcessorEditor::~LightWeaverAudioProcessorEditor() {}

void LightWeaverAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour(0xff18181c));
}

void LightWeaverAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  headerComponent->setBounds(bounds.removeFromTop(48));

  auto footer = bounds.removeFromBottom(22);
  statusLabel.setBounds(footer);

  if (stageViewPreview->isVisible()) {
    const auto stageHeight = juce::roundToInt(static_cast<float>(bounds.getHeight()) * 0.45f);
    stageViewPreview->setBounds(bounds.removeFromTop(stageHeight));
  } else {
    stageViewPreview->setBounds(0, 0, 0, 0);
  }

  fixtureViewport.setBounds(bounds);
  fixtureContainer->setSize(fixtureViewport.getMaximumVisibleWidth(),
                            fixtureContainer->getHeight());
}

juce::File LightWeaverAudioProcessorEditor::createSceneMidiFile() {
  struct ParameterCc {
    const char* parameterId;
    int ccNumber;
  };
  static constexpr std::array<ParameterCc, 21> parameterMappings{{
      {"spot1_red", 1},           {"spot1_green", 2},    {"spot1_blue", 3},   {"spot2_red", 4},
      {"spot2_green", 5},         {"spot2_blue", 6},     {"spot3_red", 7},    {"spot3_green", 8},
      {"spot3_blue", 9},          {"spot4_red", 10},     {"spot4_green", 11}, {"spot4_blue", 12},
      {"clb4_master_dimmer", 14}, {"panel1_dimmer", 15}, {"panel1_red", 16},  {"panel1_green", 17},
      {"panel1_blue", 18},        {"panel2_dimmer", 19}, {"panel2_red", 20},  {"panel2_green", 21},
      {"panel2_blue", 22},
  }};

  auto file = juce::File::getSpecialLocation(juce::File::tempDirectory)
                  .getNonexistentChildFile("LightWeaver_Scene", ".mid", false);

  juce::MidiMessageSequence sequence;
  const auto& apvts = audioProcessor.getAPVTS();
  const auto* channelParameter = apvts.getRawParameterValue("midi_channel");
  const auto channel = channelParameter != nullptr
                           ? juce::jlimit(1, 16, juce::roundToInt(channelParameter->load()))
                           : 9;

  for (const auto& mapping : parameterMappings) {
    const auto* parameterValue = apvts.getRawParameterValue(mapping.parameterId);
    if (parameterValue == nullptr) continue;

    const auto dmxValue = juce::jlimit(0, 254, juce::roundToInt(parameterValue->load()));
    const auto midiValue = juce::jlimit(0, 127, juce::roundToInt(dmxValue * 0.5f));
    sequence.addEvent(juce::MidiMessage::controllerEvent(channel, mapping.ccNumber, midiValue),
                      0.0);
  }
  sequence.addEvent(juce::MidiMessage::endOfTrack(), 1.0);

  juce::MidiFile midiFile;
  midiFile.setTicksPerQuarterNote(480);
  midiFile.addTrack(sequence);

  auto output = file.createOutputStream();
  if (output == nullptr || !midiFile.writeTo(*output, 0)) return {};

  output->flush();
  return file;
}

void LightWeaverAudioProcessorEditor::beginExternalMidiDrag() {
  const auto midiFile = createSceneMidiFile();
  if (!midiFile.existsAsFile()) {
    setStatus("Could not create the scene MIDI file");
    return;
  }

  setStatus("Dragging " + midiFile.getFullPathName());
  juce::StringArray files{midiFile.getFullPathName()};
  juce::MessageManager::callAsync([files] {
    juce::DragAndDropContainer::performExternalDragDropOfFiles(files, false, nullptr);
  });
}

void LightWeaverAudioProcessorEditor::setStatus(const juce::String& message) {
  statusLabel.setText(message, juce::dontSendNotification);
}

void LightWeaverAudioProcessorEditor::timerCallback() {
  const auto eventCount = audioProcessor.getMidiCcEventCount();
  if (eventCount == displayedMidiCcEventCount) return;

  displayedMidiCcEventCount = eventCount;
  statusLabel.setText("Received CC " + juce::String(audioProcessor.getLastMidiCcNumber()) + " = " +
                          juce::String(audioProcessor.getLastMidiCcValue()) + " on MIDI channel " +
                          juce::String(audioProcessor.getLastMidiCcChannel()),
                      juce::dontSendNotification);
}
