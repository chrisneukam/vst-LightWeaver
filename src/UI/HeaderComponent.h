#pragma once

#include <JuceHeader.h>

class HeaderComponent : public juce::Component {
 public:
  explicit HeaderComponent(juce::AudioProcessorValueTreeState& apvts);
  ~HeaderComponent() override;

  void paint(juce::Graphics& graphics) override;
  void resized() override;

  std::function<void(bool)> onStageViewVisibilityChanged;
  std::function<void()> onKeyframeDragRequested;

 private:
  class KeyframeDragButton;

  juce::Label titleLabel;
  juce::ComboBox midiChannelBox;
  juce::ToggleButton blackoutButton;
  juce::ToggleButton stageViewButton;
  std::unique_ptr<KeyframeDragButton> keyframeDragButton;

  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> midiChannelAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> blackoutAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderComponent)
};