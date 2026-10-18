#include "FixtureContainerComponent.h"

namespace {
constexpr auto containerBackground = 0xff18181c;
constexpr auto groupHeaderBackground = 0xff29292f;
constexpr auto primaryText = 0xfff0f0f5;
}  // namespace

FixtureContainerComponent::FixtureContainerComponent(juce::AudioProcessorValueTreeState& apvts,
                                                     std::vector<FixtureCard::Config> fixtures,
                                                     std::function<void(int, int)> midiCcOutput) {
  fixtureCards.reserve(fixtures.size());
  juce::String activeGroupName;
  for (auto& fixture : fixtures) {
    if (fixture.groupName != activeGroupName) {
      activeGroupName = fixture.groupName;
      auto groupLabel = std::make_unique<juce::Label>();
      groupLabel->setText(activeGroupName, juce::dontSendNotification);
      groupLabel->setColour(juce::Label::textColourId, juce::Colour(primaryText));
      groupLabel->setColour(juce::Label::backgroundColourId, juce::Colour(groupHeaderBackground));
      groupLabel->setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
      groupLabel->setJustificationType(juce::Justification::centredLeft);
      groupLabel->setBorderSize(juce::BorderSize<int>(0, 8, 0, 8));
      groupLabel->setOpaque(true);
      addAndMakeVisible(*groupLabel);
      layoutComponents.push_back(groupLabel.get());
      layoutItemIsGroupHeader.push_back(true);
      groupLabels.push_back(std::move(groupLabel));
      groupCardCounts.push_back(0);
    }

    auto card = std::make_unique<FixtureCard>(apvts, std::move(fixture), midiCcOutput);
    addAndMakeVisible(*card);
    layoutComponents.push_back(card.get());
    layoutItemIsGroupHeader.push_back(false);
    ++groupCardCounts.back();
    fixtureCards.push_back(std::move(card));
  }
}

void FixtureContainerComponent::paint(juce::Graphics& graphics) {
  graphics.fillAll(juce::Colour(containerBackground));
}

void FixtureContainerComponent::resized() {
  const auto availableWidth = juce::jmax(1, getWidth());
  const auto columns = juce::jmax(1, availableWidth / cardStrideX);
  auto contentHeight = 0;
  for (const auto groupCardCount : groupCardCounts) {
    const auto rows = (groupCardCount + columns - 1) / columns;
    contentHeight += groupHeaderHeight + groupHeaderSpacing + rows * cardStrideY;
  }

  if (getHeight() != contentHeight) setSize(getWidth(), contentHeight);

  juce::FlexBox layout;
  layout.flexDirection = juce::FlexBox::Direction::row;
  layout.flexWrap = juce::FlexBox::Wrap::wrap;
  layout.alignContent = juce::FlexBox::AlignContent::flexStart;

  for (size_t index = 0; index < layoutComponents.size(); ++index) {
    auto* component = layoutComponents[index];
    if (layoutItemIsGroupHeader[index]) {
      layout.items.add(juce::FlexItem(*component)
                           .withWidth(static_cast<float>(availableWidth - cardSpacing * 2))
                           .withHeight(static_cast<float>(groupHeaderHeight))
                           .withMargin(static_cast<float>(cardSpacing / 2)));
    } else {
      layout.items.add(juce::FlexItem(*component)
                           .withWidth(static_cast<float>(FixtureCard::preferredWidth))
                           .withHeight(static_cast<float>(FixtureCard::preferredHeight))
                           .withMargin(static_cast<float>(cardSpacing)));
    }
  }

  layout.performLayout(getLocalBounds().toFloat());
}