# Vocloning Native — Technical Architecture

## 1. Overview

Vocloning Native is a JUCE desktop application for Windows. The GUI coordinates local voice-reference management and invokes speech engines through backend adapters. Speech inference is performed by external runtime processes rather than being embedded into the GUI executable.

```text
Windows user
    |
    v
JUCE application
    |
    +-- MainComponent (UI and workflow coordination)
    |
    +-- VoiceLibrary (local reference metadata/files)
    |
    +-- PocketTTSBackend
    |       |
    |       +-- pocket-tts runtime
    |       +-- Pocket TTS model assets
    |
    +-- SoproTTSBackend
            |
            +-- audio.cpp CLI
            +-- Sopro model/specification

Generated WAV -> playback -> user-selected export location
```

## 2. Source layout

| Path | Responsibility |
|---|---|
| `Source/Main.cpp` | JUCE application lifecycle and main window |
| `Source/MainComponent.h/.cpp` | Main interface, reference selection, text entry, engine selection, generation, playback and export |
| `Source/VoiceLibrary.h/.cpp` | Voice-reference library and local metadata/file handling |
| `Source/PocketTTSBackend.h/.cpp` | Pocket TTS process integration |
| `Source/SoproTTSBackend.h/.cpp` | Sopro/audio.cpp process integration and generated-WAV validation |
| `Source/Utf8.h` | UTF-8 helper functions |
| `CMakeLists.txt` | JUCE modules, application target, resources and Windows build configuration |
| `scripts/` | Build, runtime setup, diagnostics and packaging scripts |
| `installer/` | Inno Setup installer definition |
| `Runtime/` | Runtime documentation; third-party runtime binaries are not committed |

## 3. Application lifecycle

The JUCE application entry point creates the application instance and main window. The main window owns the main component. The main component coordinates the visual workflow and delegates engine-specific operations to the selected backend.

## 4. Voice library and references

Voice references are selected from local WAV/MP3 files. Reference recordings remain user data and should not be committed to the repository. The application presents reference cards and provides the text/generation workflow.

## 5. Pocket TTS backend

The Pocket TTS adapter locates and invokes the standalone Pocket TTS runtime. The current source selects the `portuguese_24l` model identifier. The runtime and model files are external dependencies and are not included in the public source repository.

The backend contains local Hugging Face token handling. Treat the token as a secret: never print it, commit it or include it in diagnostics. A future hardening pass should prefer a per-child-process environment over temporary process-wide environment mutation.

## 6. Sopro backend

The Sopro adapter invokes the `audio.cpp` command-line runtime with the selected model/specification and voice reference. The backend checks process completion and validates the generated WAV. The validation includes readability, finite samples, non-silence, mono output and a 24 kHz sample rate.

The runtime and model are external dependencies. Their exact versions and compatibility must be documented and pinned for reproducible distribution.

## 7. Build system

CMake builds the JUCE application for Windows x64. The supported CI workflow checks out JUCE 9.0.6, configures with the Visual Studio 2022 generator and builds Release. The workflow uploads the executable as an artifact.

CI compilation does not replace manual tests of the UI, runtime processes, model loading, audio playback or export.

## 8. Security boundaries

- Voice recordings and generated speech are private user data.
- Runtime binaries and model weights are third-party assets.
- Do not commit authentication tokens, model caches, generated WAVs or build outputs.
- Pin third-party versions and verify SHA-256 before production distribution.
- Do not claim that the source repository's licence covers JUCE or model assets.

## 9. Future engineering priorities

- Pin runtime releases and model revisions.
- Verify downloaded files using SHA-256.
- Prefer per-process credential passing.
- Centralize application version metadata.
- Add automated tests for reference import, library persistence, backend failure handling and WAV export.
- Test installation, upgrade and uninstall on clean Windows 11 systems.