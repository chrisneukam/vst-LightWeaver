# Project Context: LightWeaver (vst-LightWeaver)

## 1. Project Overview
**LightWeaver** is a dedicated **VST3 Plugin (C++20 / JUCE 8)** designed to automate and visualize live stage lighting for musicians and cover bands. 
It runs inside **Steinberg Cubase** as a MIDI Effect and transmits `MIDI CC` messages to an external custom hardware bridge (**arduino-MidiDmxBridge**).

### Repositories & Hardware Link
* **Plugin Repository:** `vst-LightWeaver`
* **Hardware Bridge Repo:** https://github.com/chrisneukam/arduino-MidiDmxBridge/tree/main
* **Bridge Functionality:** Listens to a user-defined MIDI channel (Default: **Channel 9**). It receives `MIDI CC` messages, scales the 7-bit MIDI value (0–127) to an 8-bit DMX value (0–255, by multiplying by 2), and maps `CC Number` directly to `DMX Channel Number`.

---

## 2. Core Architectural Principles
1. **Cubase as Single Source of Truth:**
  * The plugin does **not** store or manage timeline keyframes internally.
  * Cubase handles all MIDI tracks, playback, recording, timing, and keyframe persistence.
  * The GUI provides an **Insert Keyframe** action that writes the current parameter values to Cubase at the current locator position.
2. **Bidirectional Communication & Streaming:**
  * **Plugin $\rightarrow$ Hardware:** User UI adjustments immediately emit live MIDI CCs. The values are also available to `processBlock()` for real-time MIDI output and automation handling.
  * **Cubase $\rightarrow$ Plugin:** Playing back or scrubbing the Cubase timeline feeds MIDI CCs into `processBlock()`, dynamically updating the UI controls, color pickers, and the 2D Stage Visualizer in real time.
  * MIDI CCs are not filtered by channel or CC number. Messages from other MIDI devices sharing the configured channel remain pass-through data.
  * A user-driven GUI change has precedence over an incoming/automated value at that moment.
3. **Blackout behavior:**
  * When enabled, every transport state in which Cubase is stopped triggers a blackout sequence: CC 1 through CC 22 are sent with value 0 on the configured MIDI channel.
  * The blackout is a hardware-only reset. It must not change APVTS values, GUI state, or stored automation.
  * Incoming MIDI CCs received after a blackout are passed through normally and can update the GUI according to the normal playback/input rules.
4. **Geometry & Movement Terminology:**
   * **Fixture Rotation (2D Stage Layout):** Rotating the entire physical fixture enclosure/bar ($0^\circ \dots 360^\circ$) on the 2D stage canvas (e.g., bar placed behind, left, or right of the stage).
   * **Pan / Tilt (Spot Head Movement):** Physical aiming vectors of individual spot heads relative to the fixture chassis. Pan = horizontal angle shift; Tilt = vertical angle shift (influences beam length and dispersion in 2D top-view).

---

## 3. Directory & File Structure
Copilot must adhere to this project layout:

```text
vst-LightWeaver/
├── CMakeLists.txt
├── docs/
│   ├── PROJECT_CONTEXT.md
│   ├── MAPPING_SPEC.md
│   ├── JUCE_STYLEGUIDE.md
│   └── ROADMAP.md
└── src/
    ├── PluginProcessor.h / .cpp       # AudioProcessor & APVTS Setup
    ├── PluginEditor.h / .cpp          # Main AudioProcessorEditor
    ├── Model/                         # Data Models & Fixture Abstractions
    │   ├── FixtureData.h              # Structs for Fixtures, Spots, CC Mappings
    │   └── FixtureManager.h / .cpp    # In-memory fixture registry (JSON ready)
    └── UI/                            # GUI Components
        ├── HeaderComponent.h / .cpp   # MIDI Channel & Global Controls
        ├── FixtureCard.h / .cpp       # UI Card for Color/Dimmer Control
        └── StageView/                 # 2D Canvas (Sprint 4)
```

## 4. State Management & Parameter Naming (APVTS)
Use JUCE juce::AudioProcessorValueTreeState (APVTS). Parameter IDs must follow this strict naming pattern so Copilot generates matching code across Processor and Editor:

* Global Parameters:
  * "midi_channel" (Range: $1 \dots 16$, Default: $9$)
  * "blackout_on_stop" (Bool / Toggle, Default: true)
* Fixture Parameters (spot1 to spot4, panel1, panel2):
  * Color: "<fixture_id>_red", "<fixture_id>_green", "<fixture_id>_blue" (Range: $0 \dots 254$)
  * Dimmers: "clb4_master_dimmer", "panel1_dimmer", "panel2_dimmer" (Range: $0 \dots 254$)

All newly loaded fixtures start black with their dimmer at 0. The MVP uses a statically defined fixture registry; the JSON loader supplies the same data model in Sprint 5.

Internal Data Structures (`src/Model/FixtureData.h`)

### Internal Data Structures (`src/Model/FixtureData.h`)

```cpp
#pragma once
#include <JuceHeader.h>
#include <vector>

struct Spot {
    juce::String id;
    juce::String name;
    uint8_t ccRed;
    uint8_t ccGreen;
    uint8_t ccBlue;
};

struct Fixture {
    juce::String id;
    juce::String name;
    uint8_t ccDimmer;
    std::vector<Spot> spots;
    float posX { 0.0f };
    float posY { 0.0f };
    float rotationDeg { 0.0f };
};
```

## 5. Technical Stack & Thread Safety Rules
* Language Standard: C++20 (target_compile_features(... cxx_std_20)).
* Framework: JUCE 8 (via CMake FetchContent).
* Plugin Type: VST3 MIDI Effect (IS_SYNTH FALSE, NEEDS_MIDI_OUTPUT TRUE, NEEDS_MIDI_INPUT TRUE).
* Target OS: Windows 11 (64-bit) / MSVC (Visual Studio Build Tools 2022).
* Audio Thread (AudioProcessor): Must remain real-time safe. Absolutely NO allocations (new, std::vector::push_back), disk I/O, OS logging (DBG), or mutex locks inside processBlock().
* Lock-Free Parameter Passing: To pass incoming MIDI CC data from processBlock() back to the UI for live visualization, use std::atomic variables or juce::AbstractFifo buffers.
* `processBlock()` must not allocate, lock, perform I/O, or log. Any MIDI event storage required for output must be preallocated or written directly into the supplied `juce::MidiBuffer`.
* GUI-originated parameter changes must be emitted immediately without waiting for transport playback. The Insert Keyframe action is a separate explicit write of the current GUI/APVTS values into Cubase.

## 6. GitHub Copilot Usage Guidelines
When prompting GitHub Copilot in VS Code:

1. Always reference @docs/PROJECT_CONTEXT.md and @docs/MAPPING_SPEC.md in workspace queries.
2. Demand JUCE 8 & C++20 compliant code (prefer std::unique_ptr, override keywords, RAII).
3. Enforce strict separation: No GUI components (juce::Component) inside PluginProcessor.
4. Keep all source files under `src/`; do not introduce a parallel `Source/` tree.