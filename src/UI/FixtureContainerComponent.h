#pragma once

#include <JuceHeader.h>

#include "FixtureCard.h"

class FixtureContainerComponent : public juce::Component {
 public:
  FixtureContainerComponent(juce::AudioProcessorValueTreeState& apvts,
                            std::vector<FixtureCard::Config> fixtures,
                            std::function<void(int, int)> midiCcOutput = {});

  void paint(juce::Graphics& graphics) override;
  void resized() override;

 private:
  static constexpr int cardSpacing = 6;
  static constexpr int cardStrideX = FixtureCard::preferredWidth + cardSpacing * 2;
  static constexpr int cardStrideY = FixtureCard::preferredHeight + cardSpacing * 2;
  static constexpr int groupHeaderHeight = 22;
  static constexpr int groupHeaderSpacing = 6;

  std::vector<std::unique_ptr<FixtureCard>> fixtureCards;
  std::vector<std::unique_ptr<juce::Label>> groupLabels;
  std::vector<juce::Component*> layoutComponents;
  std::vector<bool> layoutItemIsGroupHeader;
  std::vector<int> groupCardCounts;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FixtureContainerComponent)
};