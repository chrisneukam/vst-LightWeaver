#pragma once

#include <JuceHeader.h>

class FixtureCard : public juce::Component,
                    private juce::AudioProcessorValueTreeState::Listener,
                    private juce::ChangeListener,
                    private juce::Slider::Listener,
                    private juce::AsyncUpdater {
 public:
  struct Config {
    juce::String fixtureId;
    juce::String displayName;
    juce::String dimmerParameterId;
    int redCcNumber{0};
    int greenCcNumber{0};
    int blueCcNumber{0};
    int dimmerCcNumber{0};
    juce::String groupName;
  };

  FixtureCard(juce::AudioProcessorValueTreeState& apvts, Config config,
              std::function<void(int, int)> midiCcOutput);
  ~FixtureCard() override;

  void paint(juce::Graphics& graphics) override;
  void resized() override;

  static constexpr int preferredWidth = 160;
  static constexpr int preferredHeight = 240;

 private:
  void parameterChanged(const juce::String& parameterId, float newValue) override;
  void changeListenerCallback(juce::ChangeBroadcaster* source) override;
  void sliderValueChanged(juce::Slider* slider) override;
  void handleAsyncUpdate() override;
  void setFixtureColour(juce::Colour colour);
  juce::Colour getFixtureColour() const;

  juce::AudioProcessorValueTreeState& apvts;
  Config config;
  std::function<void(int, int)> midiCcOutput;
  juce::String redParameterId;
  juce::String greenParameterId;
  juce::String blueParameterId;

  juce::Label titleLabel;
  juce::ColourSelector colourSelector;
  std::array<juce::TextButton, 4> colourSwatches;
  juce::Label panLabel;
  juce::Label tiltLabel;
  juce::Slider panSlider;
  juce::Slider tiltSlider;
  juce::ToggleButton muteButton{"M"};
  juce::ToggleButton soloButton{"S"};
  juce::Slider dimmerSlider;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dimmerAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FixtureCard)
};