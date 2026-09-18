#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

class LightWeaverAudioProcessorEditor : public juce::AudioProcessorEditor {
 public:
  LightWeaverAudioProcessorEditor(LightWeaverAudioProcessor&);
  ~LightWeaverAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

 private:
  LightWeaverAudioProcessor& audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightWeaverAudioProcessorEditor)
};