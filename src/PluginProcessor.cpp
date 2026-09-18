#include "PluginProcessor.h"

#include "PluginEditor.h"

LightWeaverAudioProcessor::LightWeaverAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()),
#endif
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
}

LightWeaverAudioProcessor::~LightWeaverAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
LightWeaverAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  // Global Parameters
  params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"midi_channel", 1},
                                                             "MIDI Channel", 1, 16, 9));

  params.push_back(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{"blackout_on_stop", 1}, "Blackout on Stop", true));

  // Helper to generate Spot RGB Parameters
  auto addSpotParams = [&](const juce::String& prefix, const juce::String& name) {
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{prefix + "_red", 1}, name + " Red", 0, 255, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{prefix + "_green", 1}, name + " Green", 0, 255, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{prefix + "_blue", 1}, name + " Blue", 0, 255, 0));
  };

  // CLB4 Spots 1 - 4
  addSpotParams("spot1", "Spot 1");
  addSpotParams("spot2", "Spot 2");
  addSpotParams("spot3", "Spot 3");
  addSpotParams("spot4", "Spot 4");

  // Flood Panels 1 - 2
  addSpotParams("panel1", "Panel 1");
  addSpotParams("panel2", "Panel 2");

  // Dimmers
  params.push_back(std::make_unique<juce::AudioParameterInt>(
      juce::ParameterID{"clb4_master_dimmer", 1}, "CLB4 Master Dimmer", 0, 255, 255));
  params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"panel1_dimmer", 1},
                                                             "Panel 1 Dimmer", 0, 255, 255));
  params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"panel2_dimmer", 1},
                                                             "Panel 2 Dimmer", 0, 255, 255));

  return {params.begin(), params.end()};
}

void LightWeaverAudioProcessor::prepareToPlay(double /*sampleRate*/, int /*samplesPerBlock*/) {}

void LightWeaverAudioProcessor::releaseResources() {}

bool LightWeaverAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  // MIDI Effect without audio channels
  return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled();
}

void LightWeaverAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
  buffer.clear();

  // Audio-thread logic for reading/emitting MIDI CC will be expanded in Sprint 2 & 3
}

juce::AudioProcessorEditor* LightWeaverAudioProcessor::createEditor() {
  return new LightWeaverAudioProcessorEditor(*this);
}

void LightWeaverAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void LightWeaverAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new LightWeaverAudioProcessor(); }