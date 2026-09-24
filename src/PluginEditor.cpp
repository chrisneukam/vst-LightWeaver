#include "PluginEditor.h"

#include "PluginProcessor.h"

class LightWeaverAudioProcessorEditor::DragSourceComponent : public juce::Component {
 public:
  explicit DragSourceComponent(LightWeaverAudioProcessorEditor& ownerToUse) : owner(ownerToUse) {
    setMouseCursor(juce::MouseCursor::DraggingHandCursor);
  }

  void paint(juce::Graphics& g) override {
    g.setColour(juce::Colour(0xff3d7eff));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Drag sample MIDI to Cubase", getLocalBounds(), juce::Justification::centred, true);
  }

  void mouseDrag(const juce::MouseEvent& event) override {
    if (!dragStarted && event.mouseWasDraggedSinceMouseDown()) {
      dragStarted = true;
      owner.beginExternalMidiDrag();
    }
  }

  void mouseUp(const juce::MouseEvent&) override { dragStarted = false; }

 private:
  LightWeaverAudioProcessorEditor& owner;
  bool dragStarted{false};
};

LightWeaverAudioProcessorEditor::LightWeaverAudioProcessorEditor(LightWeaverAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  dragSource = std::make_unique<DragSourceComponent>(*this);
  addAndMakeVisible(*dragSource);

  statusLabel.setJustificationType(juce::Justification::centred);
  statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
  statusLabel.setText("Drag the button onto a Cubase MIDI track", juce::dontSendNotification);
  addAndMakeVisible(statusLabel);

  setSize(1000, 700);
  setResizable(true, true);
  setResizeLimits(800, 500, 1920, 1080);
}

LightWeaverAudioProcessorEditor::~LightWeaverAudioProcessorEditor() {}

void LightWeaverAudioProcessorEditor::paint(juce::Graphics& g) {
  // Dark Theme Background
  g.fillAll(juce::Colour(0xff1e1e24));

  g.setColour(juce::Colours::white);
  g.setFont(24.0f);
  g.drawText("LightWeaver MIDI Drag-and-Drop Spike XXX", getLocalBounds().removeFromTop(120),
             juce::Justification::centred, true);
}

void LightWeaverAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();

  if (dragSource != nullptr)
    dragSource->setBounds(bounds.withSizeKeepingCentre(420, 64).translated(0, 10));

  statusLabel.setBounds(bounds.withSizeKeepingCentre(900, 32).translated(0, 100));
}

juce::File LightWeaverAudioProcessorEditor::createSampleMidiFile() {
  auto file = juce::File::getSpecialLocation(juce::File::tempDirectory)
                  .getNonexistentChildFile("LightWeaver_Spike_CC1", ".mid", false);

  juce::MidiMessageSequence sequence;
  sequence.addEvent(juce::MidiMessage::controllerEvent(9, 1, 127), 0.0);
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
  const auto midiFile = createSampleMidiFile();
  if (!midiFile.existsAsFile()) {
    setStatus("Could not create the temporary MIDI file");
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
