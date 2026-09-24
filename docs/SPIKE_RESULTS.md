# Spike Sprint: MIDI Drag-and-Drop in Cubase

**Date:** 2026-09-24  
**Host:** Steinberg Cubase 14.0.41 64-bit  
**Plugin:** LightWeaver VST3  
**Status:** Successfully verified

## Result

The LightWeaver plugin generates a temporary Standard MIDI File and passes it to Cubase through an external file drag-and-drop operation.

To insert the MIDI event into the existing MIDI track under the drop location, the following Cubase setting must be disabled:

> **Preferences > MIDI > MIDI File > Automatically Dissolve Format 0**

The setting must be **disabled**.

When **Automatically Dissolve Format 0** is enabled, Cubase creates additional tracks during the drop. When it is disabled, Cubase inserts the MIDI event into the existing target track.

## Reproducible Test

1. Open Cubase and create a MIDI track.
2. Make sure **Automatically Dissolve Format 0** is disabled.
3. Open LightWeaver as a VST3 instrument/MIDI effect.
4. Drag the **Drag sample MIDI to Cubase** control from the plugin window onto the existing MIDI track.
5. Verify the generated MIDI part on the target track.
6. Play the event and inspect the MIDI output.

## Test Payload

The current spike file contains:

- Standard MIDI File, Format 0
- One MIDI track
- MIDI channel 9
- Controller event: CC #1
- Value: 127
- No SysEx data

The file is created in the Windows temporary folder, for example:

```text
C:\Users\<user>\AppData\Local\Temp\LightWeaver_Spike_CC1_3.mid
```

## Cubase Setting Reference

![Cubase MIDI File settings with Automatically Dissolve Format 0 disabled](assets/cubase-midi-file-format-0-disabled.png)

> **Note:** The screenshot from the manual Cubase test must be stored as `docs/assets/cubase-midi-file-format-0-disabled.png` so that the image reference resolves locally in this document.

## Technical Conclusion

The external file drag-and-drop workflow works for this use case. Cubase determines the target track from the drop location. LightWeaver does not generate a SysEx track; the generated file contains neither SysEx data nor multiple MIDI tracks. The observed behavior is controlled by Cubase's **Automatically Dissolve Format 0** setting.

## Next Steps

- Repeat the test with multiple drops onto different existing MIDI tracks.
- Verify the MIDI channel and controller value with a MIDI monitor.
- For the full keyframe workflow, extend the payload from one CC to the relevant CC messages 1 through 22.
