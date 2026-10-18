#include "HeaderComponent.h"

class HeaderComponent::KeyframeDragButton : public juce::TextButton {
 public:
  explicit KeyframeDragButton(HeaderComponent& ownerToUse)
      : juce::TextButton("DRAG KEYFRAME"), owner(ownerToUse) {
    setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff8c00));
    setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffff8c00));
    setColour(juce::TextButton::textColourOffId, juce::Colour(0xff18181c));
    setClickingTogglesState(false);
  }

  void mouseDrag(const juce::MouseEvent& event) override {
    if (!dragStarted && event.mouseWasDraggedSinceMouseDown()) {
      dragStarted = true;
      if (owner.onKeyframeDragRequested) owner.onKeyframeDragRequested();
    }
  }

  void mouseUp(const juce::MouseEvent& event) override {
    dragStarted = false;
    juce::TextButton::mouseUp(event);
  }

 private:
  HeaderComponent& owner;
  bool dragStarted{false};
};

HeaderComponent::HeaderComponent(juce::AudioProcessorValueTreeState& apvts) {
  titleLabel.setText("LIGHTWEAVER VST3", juce::dontSendNotification);
  titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff0f0f5));
  titleLabel.setFont(juce::Font(juce::FontOptions(16.0f, juce::Font::bold)));
  addAndMakeVisible(titleLabel);

  for (int channel = 1; channel <= 16; ++channel)
    midiChannelBox.addItem("MIDI CH: " + juce::String(channel), channel);
  addAndMakeVisible(midiChannelBox);
  midiChannelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
      apvts, "midi_channel", midiChannelBox);

  blackoutButton.setButtonText("BLACKOUT");
  blackoutButton.setClickingTogglesState(true);
  addAndMakeVisible(blackoutButton);
  blackoutAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      apvts, "blackout_on_stop", blackoutButton);

  stageViewButton.setButtonText("STAGE VIEW");
  stageViewButton.setClickingTogglesState(true);
  stageViewButton.setToggleState(true, juce::dontSendNotification);
  stageViewButton.onClick = [this] {
    if (onStageViewVisibilityChanged)
      onStageViewVisibilityChanged(stageViewButton.getToggleState());
  };
  addAndMakeVisible(stageViewButton);

  keyframeDragButton = std::make_unique<KeyframeDragButton>(*this);
  addAndMakeVisible(*keyframeDragButton);
}

HeaderComponent::~HeaderComponent() = default;

void HeaderComponent::paint(juce::Graphics& graphics) {
  graphics.fillAll(juce::Colour(0xff121215));
}

void HeaderComponent::resized() {
  juce::FlexBox layout;
  layout.flexDirection = juce::FlexBox::Direction::row;
  layout.flexWrap = juce::FlexBox::Wrap::noWrap;
  layout.alignItems = juce::FlexBox::AlignItems::center;

  layout.items.add(juce::FlexItem(titleLabel).withFlex(1.0f).withMinWidth(170.0f).withMargin(4.0f));
  layout.items.add(
      juce::FlexItem(midiChannelBox).withWidth(132.0f).withHeight(32.0f).withMargin(4.0f));
  layout.items.add(
      juce::FlexItem(blackoutButton).withWidth(92.0f).withHeight(32.0f).withMargin(4.0f));
  layout.items.add(
      juce::FlexItem(stageViewButton).withWidth(106.0f).withHeight(32.0f).withMargin(4.0f));
  layout.items.add(
      juce::FlexItem(*keyframeDragButton).withWidth(160.0f).withHeight(34.0f).withMargin(4.0f));
  layout.performLayout(getLocalBounds().toFloat());
}