# GUI Design Specification: LightWeaver (VST3)

## 1. Overview & Visual Aesthetic
The LightWeaver GUI follows a modern, dark-themed pro-audio/lighting control interface style (similar to grandMA3 / Ableton Live).
It is structured into three main vertical sections:
1. **Header Toolbar (Fixed Height: 48px)**
2. **2D Stage Top-Down View Visualizer (Collapsible, Flexible Height)**
3. **Fixture Control Grid (Responsive FlexBox, Scrollable Viewport)**

---

## 2. Component Hierarchy & Layout Structure

LightWeaverAudioProcessorEditor (Main Component)
├── HeaderComponent
│   ├── juce::Label (Plugin Title / Logo "LIGHTWEAVER VST3")
│   ├── juce::ComboBox (MIDI Channel Selector "MIDI CH: 1")
│   ├── juce::ToggleButton (Blackout Switch)
│   ├── juce::TextButton (Collapse/Expand "STAGE VIEW")
│   └── KeyframeDragButton (Glowing Orange "DRAG KEYFRAME" Button)
├── StageViewComponent (Collapsible 2D Top-Down View Canvas)
│   ├── StageFloorGrid (2D metric grid with 0m, 5m, 10m axis markers)
│   ├── FixtureChassisMarkers (Top-view Fixture icons: circles, squares with IDs e.g. F1..F8)
│   └── Dynamic Beam Renderers (Light cone projections over the stage floor)
└── FixtureContainerComponent (inside juce::Viewport)
    └── FlexBox / Grid Layout of FixtureCard components
        └── FixtureCard (Repeated per Fixture e.g. F1, F2...)
            ├── Header Label (Fixture ID: F1, F2...)
            ├── ColorSection (RGB Color Wheel + Preset Swatches)
            ├── VectorSection (PAN & TILT Rotary Sliders)
            ├── MuteSoloSection (Compact M / S Toggle Buttons)
            └── DimmerSection (Vertical Master Dimmer Slider)

---

## 3. 2D Stage View Canvas Specification (Top-Down View)
* **Perspective:** Strict **2D Top-Down Draufsicht** looking directly down at the stage floor canvas.
* **Axis & Metric Grid:**
  * Displays subtle horizontal and vertical grid lines with meter labels (0m, 5m, 10m).
  * Stage floor background: Dark Charcoal (`juce::Colour(0xff121215)`).
* **Fixture Placement & Symbols:**
  * Fixture positions (e.g. F1 through F8) rendered as small geometric shapes (circles or rounded squares) along stage boundaries or custom positions.
  * Labels (e.g. `F1`, `F2`) displayed next to chassis symbols.
* **Beam Projection:**
  * Light beams emanate from the chassis origin and project across the 2D grid floor based on current Color, Dimmer, Pan, and Tilt settings.

---

## 4. UI Color Palette & Design Tokens
Copilot must use the following `juce::Colour` definitions for rendering:

* **Background Dark:** `juce::Colour(0xff18181c)`
* **Card Container Background:** `juce::Colour(0xff222228)`
* **Header / Bar Background:** `juce::Colour(0xff121215)`
* **Accent Orange (Drag Button):** `juce::Colour(0xffff8c00)`
* **Mute Active (M Button):** `juce::Colour(0xffd93838)`
* **Solo Active (S Button):** `juce::Colour(0xffe6a100)`
* **Text Primary:** `juce::Colour(0xfff0f0f5)`
* **Text Muted / Grid Axis Labels:** `juce::Colour(0xff70707a)`

---

## 5. Responsive Layout Rules (`resized()`)

1. **Header Layout (`HeaderComponent`):**
   * Placed at top: `bounds.removeFromTop(48)`.
   * Left side: Title (`LIGHTWEAVER VST3`).
   * Center/Right: `MIDI CH` Dropdown, `BLACKOUT` Toggle, `STAGE VIEW` Collapse Toggle, and the prominent glowing `DRAG KEYFRAME` Button (Width >= 160px).

2. **Stage View (`StageViewComponent`):**
   * If `isStageViewCollapsed == false`: Allocates top 40%...50% of remaining height (`bounds.removeFromTop(remaining * 0.45f)`).
   * If `isStageViewCollapsed == true`: Height set to 0px (`setVisible(false)`).

3. **Fixture Grid Layout (`FixtureContainerComponent`):**
   * Occupies all remaining vertical bounds inside a `juce::Viewport`.
   * Uses `juce::FlexBox` with:
     * `flexDirection = juce::FlexBox::Direction::row`
     * `flexWrap = juce::FlexBox::Wrap::wrap`
     * `alignContent = juce::FlexBox::AlignContent::flexStart`
   * Base `FixtureCard` dimensions: approx. 160px x 240px with 6px padding.

---

## 6. Parameter & APVTS Binding Matrix

Each `FixtureCard` binds its child controls to `APVTS` parameters using the corresponding `ParameterID`:

| UI Control | Type | APVTS Parameter ID Pattern | Range |
| :--- | :--- | :--- | :--- |
| **MIDI Channel** | ComboBox | `"midi_channel"` | 1 ... 16 |
| **Blackout Toggle** | ToggleButton | `"blackout_on_stop"` | `true / false` |
| **Spot RGB Color** | ColourSelector | `"<fixture_id>_red"`, `"_green"`, `"_blue"` | 0 ... 254 |
| **Pan Knob** | Rotary Slider | `"<fixture_id>_pan"` | -180° ... +180° |
| **Tilt Knob** | Rotary Slider | `"<fixture_id>_tilt"` | -90° ... +90° |
| **Mute / Solo** | Button | Internal State / UI Override | `bool` |
| **Dimmer Fader** | Vertical Slider | `"<fixture_id>_dimmer"` / `"clb4_master_dimmer"` | 0 ... 254 |

---

## 7. Drag & Drop MIDI Keyframe Action
* **Source Component:** `KeyframeDragButton` inside `HeaderComponent`.
* **Action:** On `mouseDrag()`, queries current APVTS values, calls `FixtureManager::createMidiKeyframeFile()`, and invokes `juce::DragAndDropContainer::performExternalDragDropOfFiles()` passing the generated temporary `.mid` file path.