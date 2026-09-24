# 🚀 LightWeaver (vst-LightWeaver) - Iterative Roadmap

## 🛠️ Phase 0: Vorbereitung & Tooling
- [x] Visual Studio Build Tools 2022 ("Desktopentwicklung mit C++")
- [x] VS Code Extensions (C/C++, CMake Tools)
- [x] Context-Dateien im Ordner `docs/` gepflegt

---

## 📦 Sprint 1: JUCE-VST3-Skelett & Deployment (ABGESCHLOSSEN)
- [x] `CMakeLists.txt` aufsetzen (C++20, JUCE 8 FetchContent, VST3 MIDI-Effect)
- [x] Erfolgreicher Lade-Test in Cubase

---

## Spike Sprint: Drag & Drop MIDI Keyframe Feasibility (Prior to Sprint 2)
> **Goal:** Validate JUCE's native Drag-and-Drop export of generated MIDI data into Cubase tracks across supported host environments.

- [ ] **Spike Task 1:** Implement `juce::DragAndDropContainer` and a temporary file/memory export helper for `.mid` keyframe snippets.
- [ ] **Spike Task 2:** Verify drag payload behavior in Cubase (ensure Cubase accepts the dropped `.mid` snippet as a inline MIDI Part on the target MIDI track at locator position).
- [ ] **Spike Task 3:** Test MIDI CC pass-through from Cubase back into `processBlock()` during scrub/playback for visualizer sync.
- [ ] **Deliverable:** Proof of Concept (PoC) confirming seamless drop-to-track functionality in Cubase.

---

## 🎛️ Sprint 2: Fixture-UI & Live MIDI Output (Das MVP)
**Ziel:** Steuerung der Scheinwerfer über das Plugin GUI mit Live-Ausgabe von MIDI CCs an die Hardware-Bridge.

- [ ] **State Layer:**
  - [ ] `APVTS` im `AudioProcessor` einrichten (MIDI Channel, RGB + Dimmer für alle 6 Fixtures)
  - [ ] `FixtureData` und `FixtureManager` unter `src/Model/` anlegen; Fixture-Profile im MVP statisch definieren
  - [ ] Interne RGB-/Dimmerwerte auf `0..254` begrenzen
- [ ] **GUI Layer:**
  - [ ] Header-Komponente (MIDI Channel Dropdown)
  - [ ] 6 `FixtureCard`-Komponenten mit `juce::ColourSelector` & Dimmer-Fadern
  - [ ] UI-Regler per APVTS-Attachments verknüpfen
  - [ ] `Insert Keyframe`-Button für die aktuellen Werte an der Locator-Position vorsehen
- [ ] **Real-Time Output:**
  - [ ] GUI-Änderungen sofort als MIDI CC ausgeben
  - [ ] Ungefiltertes MIDI-Pass-through sicherstellen
- [ ] **Meilenstein:** Bewegung der Plugin-Regler steuert die Bridge live an.

---

## 🔄 Sprint 3: Read-from-Track & Blackout on Stop
**Ziel:** Bidirektionales Feedback (Spur-Daten im Plugin anzeigen) & Transport-Safety.

- [ ] Reading eingehender MIDI CCs von der Cubase-Spur in `processBlock()`
- [ ] Live-Aktualisierung der Color Picker & Dimmer im Plugin beim Abspielen/Faden
- [ ] Playhead Monitoring & "Blackout on Stop" Logic (bei jedem Stop CC 1..22 auf 0, ohne GUI/APVTS zu verändern)
- [ ] **Meilenstein:** Fließendes Feedback im UI beim Abspielen der Cubase-Spur + Blackout bei Stopp.

---

## 🎭 Sprint 4: 2D Stage Top View & Immersion Visualizer
**Ziel:** Visuelles Highlight – Interaktiver Bühnenplan mit Echtzeit-Lichtsimulation.

- [ ] 2D Stage Canvas Component (`juce::Component`) mit abdunkelbarem Hintergrund
- [ ] Fixture-Placement via Drag & Drop auf dem Bühnenplan
- [ ] Fixture-Rotation ($0^\circ \dots 360^\circ$) für gesamte Gehäuse-Ensembles (z. B. Schrägstellen der Bar)
- [ ] Dynamische Lichtkegel-Rendering & Immersions-Hintergrundglow basierend auf den aktuellen CC-Werten
- [ ] **Meilenstein:** Funktionsfähiger 2D-Echtzeit-Visualizer der Live-Lichtshow.

---

## 📄 Sprint 5: JSON-Fixture-Loader
**Ziel:** Abstraktion und Lade-Schnittstelle für beliebige DMX-Fixtures.

- [ ] JSON-Parser (`juce::JSON`) für Fixture-Profile
- [ ] Dynamisches Erzeugen von FixtureCards & Stage-Elementen anhand geladener JSON-Dateien
- [ ] **Meilenstein:** Einbinden neuer Scheinwerfer ohne C++ Code-Änderungen.

---

## 📐 Sprint 6: Pan/Tilt Spot-Vectors & Stage Presets (Zukunft)
- [ ] Ausrichtung einzelner Spot-Köpfe (Pan/Tilt) im Visualizer (Veränderung der Lichtkegel-Länge & Richtung)
- [ ] Speichern und Laden von Bühnen-Layouts (Fixture-Positionen auf dem Bühnenplan)