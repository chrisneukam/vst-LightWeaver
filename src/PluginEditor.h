#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "UI/FixtureContainerComponent.h"
#include "UI/HeaderComponent.h"

class LightWeaverAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
 public:
  LightWeaverAudioProcessorEditor(LightWeaverAudioProcessor&);
  ~LightWeaverAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  void timerCallback() override;

 private:
  class StageViewPreview;

  juce::File createSceneMidiFile();
  void beginExternalMidiDrag();
  void setStatus(const juce::String& message);

  LightWeaverAudioProcessor& audioProcessor;
  std::unique_ptr<HeaderComponent> headerComponent;
  std::unique_ptr<StageViewPreview> stageViewPreview;
  std::unique_ptr<FixtureContainerComponent> fixtureContainer;
  juce::Viewport fixtureViewport;
  juce::Label statusLabel;
  uint32_t displayedMidiCcEventCount{0};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightWeaverAudioProcessorEditor)
};