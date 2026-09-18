# Mapping Specification: Fixtures to MIDI CC & DMX

## 1. Value Scaling Formula
MIDI CC uses a 7-bit integer range ($0 \dots 127$). The hardware bridge multiplies this value by 2 to achieve an 8-bit DMX value ($0 \dots 254$).

The plugin therefore limits internal RGB and dimmer values to $0 \dots 254$. This preserves the mapping without generating the invalid MIDI value 128.

To convert internal RGB/Dimmer values ($0 \dots 254$) to MIDI CC values:

$$\text{MIDI CC Value} = \text{round}\left(\frac{\text{Color/Dimmer Value}}{2}\right)$$

---

## 2. Default Hardware Mapping (Channel 9)

### C++ Hardware Reference Arrays
```cpp
constexpr uint8_t RED_CHANNELS[6]    = { 1,  4,  7, 10, 16, 20 };
constexpr uint8_t GREEN_CHANNELS[6]  = { 2,  5,  8, 11, 17, 21 };
constexpr uint8_t BLUE_CHANNELS[6]   = { 3,  6,  9, 12, 18, 22 };
constexpr uint8_t DIMMER_CHANNELS[3] = { 14, 15, 19 }; // 14 = CLB4 Master, 15 = Panel 1, 19 = Panel 2
```
---

## 3. Fixture Mapping Table

The system controls 6 logical light units split across 2 fixture models.

| Fixture ID    | Fixture Model   | Logical Component | DMX Channel | MIDI CC Number | Notes                              |
| :------------ | :-------------- | :---------------- | :---------- | :------------- | :--------------------------------- |
| **Spot 1**    | Stairville CLB4 | Red               | DMX 1       | **CC 1**       |                                    |
| **Spot 1**    | Stairville CLB4 | Green             | DMX 2       | **CC 2**       |                                    |
| **Spot 1**    | Stairville CLB4 | Blue              | DMX 3       | **CC 3**       |                                    |
| **Spot 2**    | Stairville CLB4 | Red               | DMX 4       | **CC 4**       |                                    |
| **Spot 2**    | Stairville CLB4 | Green             | DMX 5       | **CC 5**       |                                    |
| **Spot 2**    | Stairville CLB4 | Blue              | DMX 6       | **CC 6**       |                                    |
| **Spot 3**    | Stairville CLB4 | Red               | DMX 7       | **CC 7**       |                                    |
| **Spot 3**    | Stairville CLB4 | Green             | DMX 8       | **CC 8**       |                                    |
| **Spot 3**    | Stairville CLB4 | Blue              | DMX 9       | **CC 9**       |                                    |
| **Spot 4**    | Stairville CLB4 | Red               | DMX 10      | **CC 10**      |                                    |
| **Spot 4**    | Stairville CLB4 | Green             | DMX 11      | **CC 11**      |                                    |
| **Spot 4**    | Stairville CLB4 | Blue              | DMX 12      | **CC 12**      |                                    |
| **All Spots** | Stairville CLB4 | Master Dimmer     | DMX 14      | **CC 14**      | Shared Master Dimmer for Spots 1–4 |
| **Panel 1**   | Flood Panel 150 | Red               | DMX 16      | **CC 16**      |                                    |
| **Panel 1**   | Flood Panel 150 | Green             | DMX 17      | **CC 17**      |                                    |
| **Panel 1**   | Flood Panel 150 | Blue              | DMX 18      | **CC 18**      |                                    |
| **Panel 1**   | Flood Panel 150 | Dimmer            | DMX 15      | **CC 15**      | Dedicated Dimmer for Panel 1       |
| **Panel 2**   | Flood Panel 150 | Red               | DMX 20      | **CC 20**      |                                    |
| **Panel 2**   | Flood Panel 150 | Green             | DMX 21      | **CC 21**      |                                    |
| **Panel 2**   | Flood Panel 150 | Blue              | DMX 22      | **CC 22**      |                                    |
| **Panel 2**   | Flood Panel 150 | Dimmer            | DMX 19      | **CC 19**      | Dedicated Dimmer for Panel 2       |

---

## 4. Blackout Logic

When "Blackout on Stop" is enabled, every stopped transport state emits MIDI CC value 0 for CC 1 through CC 22 on the configured MIDI channel. The implementation must not change APVTS values, GUI state, or stored automation; this is a hardware-only reset.

The blackout sequence does not filter or suppress other MIDI messages. Incoming CCs after the blackout are passed through normally.

## 5. MIDI Pass-Through and Keyframes

The plugin does not filter MIDI CCs by channel or number. CC messages from other devices sharing the configured channel remain pass-through data.

GUI changes send the corresponding CC immediately. The separate "Insert Keyframe" command writes the current GUI/APVTS values to Cubase at the current locator position so Cubase can store and automate them.

---
