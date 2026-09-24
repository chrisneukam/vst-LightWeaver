#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

class LightWeaverAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
 public:
  LightWeaverAudioProcessorEditor(LightWeaverAudioProcessor&);
  ~LightWeaverAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  void timerCallback() override;

 private:
  class DragSourceComponent;

  juce::File createSampleMidiFile();
  void beginExternalMidiDrag();
  void setStatus(const juce::String& message);

  LightWeaverAudioProcessor& audioProcessor;
  std::unique_ptr<DragSourceComponent> dragSource;
  juce::Label statusLabel;
  uint32_t displayedMidiCcEventCount{0};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightWeaverAudioProcessorEditor)
};