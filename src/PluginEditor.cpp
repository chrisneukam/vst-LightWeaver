#include "PluginEditor.h"

#include "PluginProcessor.h"

LightWeaverAudioProcessorEditor::LightWeaverAudioProcessorEditor(LightWeaverAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Basic Window Setup
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
  g.drawText("LightWeaver VST3 Skeleton - Sprint 1 Updated", getLocalBounds(),
             juce::Justification::centred, true);
}

void LightWeaverAudioProcessorEditor::resized() {}