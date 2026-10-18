#include "FixtureCard.h"

namespace {
constexpr auto cardBackground = 0xff222228;
constexpr auto primaryText = 0xfff0f0f5;
constexpr auto mutedText = 0xff70707a;
}  // namespace

FixtureCard::FixtureCard(juce::AudioProcessorValueTreeState& state, Config configuration,
                         std::function<void(int, int)> midiOutput)
    : apvts(state),
      config(std::move(configuration)),
      midiCcOutput(std::move(midiOutput)),
      redParameterId(config.fixtureId + "_red"),
      greenParameterId(config.fixtureId + "_green"),
      blueParameterId(config.fixtureId + "_blue"),
      colourSelector(juce::ColourSelector::showColourAtTop | juce::ColourSelector::showColourspace,
                     4, 4) {
  setOpaque(true);

  titleLabel.setText(config.displayName, juce::dontSendNotification);
  titleLabel.setColour(juce::Label::textColourId, juce::Colour(primaryText));
  titleLabel.setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
  addAndMakeVisible(titleLabel);

  colourSelector.setColour(juce::ColourSelector::backgroundColourId, juce::Colour(cardBackground));
  colourSelector.setCurrentColour(getFixtureColour(), juce::dontSendNotification);
  colourSelector.addChangeListener(this);
  addAndMakeVisible(colourSelector);

  const std::array<juce::Colour, 4> swatchColours{
      juce::Colour(0xffd93838), juce::Colour(0xffe6a100), juce::Colour(0xff38a86b),
      juce::Colour(0xfff0f0f5)};
  for (size_t index = 0; index < colourSwatches.size(); ++index) {
    auto& swatch = colourSwatches[index];
    swatch.setColour(juce::TextButton::buttonColourId, swatchColours[index]);
    swatch.setTooltip("Apply colour preset");
    swatch.onClick = [this, colour = swatchColours[index]] { setFixtureColour(colour); };
    addAndMakeVisible(swatch);
  }

  panLabel.setText("PAN", juce::dontSendNotification);
  tiltLabel.setText("TILT", juce::dontSendNotification);
  for (auto* label : {&panLabel, &tiltLabel}) {
    label->setColour(juce::Label::textColourId, juce::Colour(mutedText));
    label->setJustificationType(juce::Justification::centred);
    label->setFont(juce::Font(juce::FontOptions(9.0f)));
    addAndMakeVisible(*label);
  }

  panSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  panSlider.setRange(-180.0, 180.0, 1.0);
  panSlider.setName("Pan");
  panSlider.setDoubleClickReturnValue(true, 0.0);
  addAndMakeVisible(panSlider);

  tiltSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  tiltSlider.setRange(-90.0, 90.0, 1.0);
  tiltSlider.setName("Tilt");
  tiltSlider.setDoubleClickReturnValue(true, 0.0);
  addAndMakeVisible(tiltSlider);

  muteButton.setClickingTogglesState(true);
  soloButton.setClickingTogglesState(true);
  addAndMakeVisible(muteButton);
  addAndMakeVisible(soloButton);

  dimmerSlider.setSliderStyle(juce::Slider::LinearVertical);
  dimmerSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  dimmerSlider.setRange(0.0, 254.0, 1.0);
  dimmerSlider.setName("Dimmer");
  dimmerSlider.addListener(this);
  addAndMakeVisible(dimmerSlider);
  dimmerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, config.dimmerParameterId, dimmerSlider);

  for (const auto& parameterId : {redParameterId, greenParameterId, blueParameterId})
    apvts.addParameterListener(parameterId, this);
}

FixtureCard::~FixtureCard() {
  cancelPendingUpdate();
  dimmerSlider.removeListener(this);
  colourSelector.removeChangeListener(this);
  for (const auto& parameterId : {redParameterId, greenParameterId, blueParameterId})
    apvts.removeParameterListener(parameterId, this);
}

void FixtureCard::paint(juce::Graphics& graphics) {
  graphics.setColour(juce::Colour(cardBackground));
  graphics.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
}

void FixtureCard::resized() {
  auto bounds = getLocalBounds().reduced(6);
  titleLabel.setBounds(bounds.removeFromTop(20));

  auto dimmerBounds = bounds.removeFromRight(30);
  bounds.removeFromRight(4);
  colourSelector.setBounds(bounds.removeFromTop(88));

  bounds.removeFromTop(3);
  auto swatchBounds = bounds.removeFromTop(16);
  const auto swatchWidth = swatchBounds.getWidth() / static_cast<int>(colourSwatches.size());
  for (size_t index = 0; index < colourSwatches.size(); ++index)
    colourSwatches[index].setBounds(swatchBounds.removeFromLeft(swatchWidth).reduced(1));

  bounds.removeFromTop(2);
  auto labelBounds = bounds.removeFromTop(12);
  panLabel.setBounds(labelBounds.removeFromLeft(labelBounds.getWidth() / 2));
  tiltLabel.setBounds(labelBounds);
  auto vectorBounds = bounds.removeFromTop(48);
  panSlider.setBounds(vectorBounds.removeFromLeft(vectorBounds.getWidth() / 2).reduced(1));
  tiltSlider.setBounds(vectorBounds.reduced(1));

  bounds.removeFromTop(2);
  auto toggleBounds = bounds.removeFromTop(20);
  const auto toggleWidth = toggleBounds.getWidth() / 2;
  muteButton.setBounds(toggleBounds.removeFromLeft(toggleWidth).reduced(1));
  soloButton.setBounds(toggleBounds.reduced(1));
  dimmerSlider.setBounds(dimmerBounds);
}

void FixtureCard::parameterChanged(const juce::String&, float) { triggerAsyncUpdate(); }

void FixtureCard::changeListenerCallback(juce::ChangeBroadcaster* source) {
  if (source == &colourSelector) setFixtureColour(colourSelector.getCurrentColour());
}

void FixtureCard::sliderValueChanged(juce::Slider* slider) {
  if (slider == &dimmerSlider && dimmerSlider.isMouseButtonDown() && midiCcOutput)
    midiCcOutput(config.dimmerCcNumber, juce::roundToInt(dimmerSlider.getValue()));
}

void FixtureCard::handleAsyncUpdate() {
  colourSelector.setCurrentColour(getFixtureColour(), juce::dontSendNotification);
}

void FixtureCard::setFixtureColour(juce::Colour colour) {
  struct ColourValue {
    const juce::String& parameterId;
    int ccNumber;
    int value;
  };
  const std::array<ColourValue, 3> values{{
      {redParameterId, config.redCcNumber, colour.getRed()},
      {greenParameterId, config.greenCcNumber, colour.getGreen()},
      {blueParameterId, config.blueCcNumber, colour.getBlue()},
  }};

  for (const auto& [parameterId, ccNumber, value] : values) {
    auto* parameter = apvts.getParameter(parameterId);
    if (parameter == nullptr) continue;

    parameter->beginChangeGesture();
    const auto dmxValue = juce::jmin(value, 254);
    parameter->setValueNotifyingHost(parameter->convertTo0to1(static_cast<float>(dmxValue)));
    parameter->endChangeGesture();
    if (midiCcOutput && ccNumber > 0) midiCcOutput(ccNumber, dmxValue);
  }
}

juce::Colour FixtureCard::getFixtureColour() const {
  const auto readValue = [this](const juce::String& parameterId) {
    const auto* value = apvts.getRawParameterValue(parameterId);
    return value != nullptr ? juce::jlimit(0, 254, juce::roundToInt(value->load())) : 0;
  };

  return juce::Colour(static_cast<juce::uint8>(readValue(redParameterId)),
                      static_cast<juce::uint8>(readValue(greenParameterId)),
                      static_cast<juce::uint8>(readValue(blueParameterId)));
}