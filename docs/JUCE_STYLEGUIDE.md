# JUCE & C++20 Style Guide: LightWeaver VST3

## 1. Modern C++ Standards
* **Language Standard:** C++20 (`cxx_std_20`).
* **Memory Management:** RAII. Prefer `std::unique_ptr` and `std::shared_ptr`. Strictly NO raw `new`/`delete`.
* **Const Correctness:** Methods not mutating state must be marked `const`.

## 2. JUCE Threading & Architecture
* **Audio Thread (`AudioProcessor`):** Must remain real-time safe. Absolutely NO allocations, I/O, or locks inside `processBlock()`.
* **GUI Thread (`AudioProcessorEditor`):** Access or mutate audio parameters only via `juce::AudioProcessorValueTreeState` (APVTS) attachments or atomic primitives.
* **MIDI Buffer Handling:** Create events via `juce::MidiMessage::controllerEvent(channel, ccNumber, value)`. Note: JUCE internal MIDI channels are 1-based ($1 \dots 16$).

## 3. UI & Layout Guidelines
* Perform layouts exclusively inside `resized()` using relative bounds or JUCE `FlexBox` / `Grid`.
* Avoid heavy path or color allocations inside `paint()`.