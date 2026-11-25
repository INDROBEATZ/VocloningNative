# Vocloning Native - Architecture

Source baseline: 0.2.9.24 HOTFIX  
Target: Windows 11 x64  
Toolchain: C++20, JUCE 9.0.6, CMake and MSVC 2022

This document describes the architecture visible in the current source tree. It does not claim that every runtime path has passed end-to-end testing.

## Components

| Component | Source | Responsibility |
|---|---|---|
| Application entry point | Source/Main.cpp | JUCE lifecycle, splash screen and main window |
| Main interface | Source/MainComponent.h/.cpp | Navigation, reference import, text input, engine selection, generation, preview and export |
| Voice library | Source/VoiceLibrary.h/.cpp | Local reference metadata and reference-file management |
| Pocket TTS adapter | Source/PocketTTSBackend.h/.cpp | Prepares reference audio and invokes the Pocket TTS CLI |
| Sopro adapter | Source/SoproTTSBackend.h/.cpp | Invokes audio.cpp with Sopro model/spec paths and validates generated WAV |
| UTF-8 helper | Source/Utf8.h | Converts UTF-8 literals for JUCE strings |
| Build | CMakeLists.txt | JUCE modules, resources and compile definitions |
| Installer | installer/VocloningNative.iss | Inno Setup layout and post-install Sopro bootstrap |

## Startup and UI

1. CMake builds the JUCE GUI target with product name Vocloning.
2. START_JUCE_APPLICATION enters VocloningApplication.
3. initialise() creates a splash window and timer.
4. When the splash animation finishes, the application creates MainWindow.
5. MainWindow owns a MainComponent instance.

The main component coordinates the voice library, engine selection and generation controls. The source supports WAV/MP3 reference selection and WAV export. Reference recordings and generated audio are local user data and must not be committed.

## Pocket TTS adapter

PocketTTSBackend is a process adapter, not an embedded inference engine. It locates the standalone Pocket TTS executable, prepares reference audio and invokes the external process. The current source selects the model identifier portuguese_24l.

The backend has a scoped Hugging Face token helper. If a token exists in the local Hugging Face cache, it temporarily sets HF_TOKEN in the parent process environment so a child process can inherit it, then restores the previous value. Calls are serialized because the environment variable is process-wide.

Security improvement for 0.2.9.25: prefer a per-child environment block or isolated launcher instead of mutating the process-wide environment.

## Sopro adapter

SoproTTSBackend locates the audio.cpp CLI, Sopro model directory and model specification. It launches the CLI with the TTS task, Sopro family, CPU backend, thread count, text, voice reference, Portuguese language option and output path.

The backend waits up to 15 minutes, checks the process exit code and validates the generated file. Validation checks readability, finite/non-silent samples and the expected mono/24 kHz format.

The current installer downloads the latest audio.cpp release, a model specification from the upstream default branch and model files from the Hugging Face main revision. These references are mutable. Pin exact versions/revisions and verify SHA-256 before production distribution.

## Build and packaging

CMake requires JUCE 9.0.6 through JUCE_DIR, the environment variable or third_party/JUCE. It embeds the application logo, links JUCE GUI/audio modules and compiles the Windows resource file on Windows.

If Runtime/pocket-tts.exe exists at configure time, CMake copies Runtime beside the executable after build. The repository intentionally excludes that runtime binary.

The Inno Setup script expects a prepared BuildOutput directory. The packaging script must create the expected layout before compiling the installer. The installer invokes the Sopro setup script after installation, so tests must cover downloads, elevation, cancellation, upgrade and uninstall.

## Known boundaries

- Both inference engines run as external processes; inference is not linked into the GUI executable.
- The project does not use the xAI API.
- CI compilation does not certify model availability, audio quality or end-to-end operation.
- Runtime/model versions are not yet locked to immutable artifacts.
- The Sopro installer does not verify cryptographic hashes.
- Version metadata is duplicated across CMakeLists.txt, Source/Main.cpp and installer/VocloningNative.iss.
- Clean Windows 11 build and engine-specific functional tests remain required.

## Data flow

User -> JUCE MainComponent -> VoiceLibrary and selected backend

PocketTTSBackend -> pocket-tts.exe -> Pocket TTS model

SoproTTSBackend -> audiocpp_cli.exe -> Sopro model/spec

Generated WAV -> JUCE preview -> user-selected export destination

## 0.2.9.25 priorities

1. Pin runtime/model revisions and verify SHA-256.
2. Remove process-wide credential environment mutation where possible.
3. Centralize version metadata.
4. Ensure diagnostics never include tokens or private voice paths.
5. Validate import, persistence, synthesis, playback, export and failure recovery on Windows 11.
6. Confirm JUCE and model/runtime licensing before distributing binaries.