#pragma once

#include <JuceHeader.h>

class LightWeaverAudioProcessor : public juce::AudioProcessor {
 public:
  LightWeaverAudioProcessor();
  ~LightWeaverAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return JucePlugin_Name; }

  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return true; }
  bool isMidiEffect() const override { return true; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return {}; }
  void changeProgramName(int, const juce::String&) override {}

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  // Accessor for State Management
  juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

  int getLastMidiCcChannel() const noexcept { return lastMidiCcChannel.load(); }
  int getLastMidiCcNumber() const noexcept { return lastMidiCcNumber.load(); }
  int getLastMidiCcValue() const noexcept { return lastMidiCcValue.load(); }
  uint32_t getMidiCcEventCount() const noexcept { return midiCcEventCount.load(); }

 private:
  juce::AudioProcessorValueTreeState apvts;
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  std::atomic<int> lastMidiCcChannel{0};
  std::atomic<int> lastMidiCcNumber{0};
  std::atomic<int> lastMidiCcValue{0};
  std::atomic<uint32_t> midiCcEventCount{0};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightWeaverAudioProcessor)
};