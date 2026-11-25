# Vocloning Native

<p align="center">
  <img src="Assets/VocloningLogo.png" alt="Vocloning Native" width="360">
</p>

<p align="center">
  <strong>Native Windows voice cloning and speech generation application</strong><br>
  C++ · JUCE 9.0.6 · CMake · Windows 11 x64
</p>

<p align="center">
  <a href="https://github.com/INDROBEATZ/VocloningNative/actions/workflows/windows-build.yml"><img src="https://github.com/INDROBEATZ/VocloningNative/actions/workflows/windows-build.yml/badge.svg" alt="Windows build"></a>
  <img src="https://img.shields.io/badge/platform-Windows%2011%20x64-blue" alt="Windows 11 x64">
  <img src="https://img.shields.io/badge/language-C%2B%2B-orange" alt="C++">
  <img src="https://img.shields.io/badge/status-owner%20validated-success" alt="Owner validated">
</p>

**Project:** INDROBEATZ
**Repository:** https://github.com/INDROBEATZ/VocloningNative
**Development line:** 0.2.9.25 preparation
**Last documented source baseline:** 0.2.9.24 HOTFIX

Vocloning Native is a native Windows desktop application for voice-reference workflows and speech generation. It is built with C++ and JUCE and integrates local speech engines through dedicated backend adapters.

The project prioritizes natural speech and European Portuguese support. Pocket TTS is configured with the `portuguese_24l` model identifier. The Sopro V2 Turbo backend provides a European Portuguese generation path. The application does not integrate the xAI API.

## Project status

The project owner has reported successful end-to-end operation on Windows 11. The supplied test evidence demonstrates the Sopro V2 Turbo workflow, including reference selection, generation, playback and WAV export. The Windows CI build also completed successfully using MSVC 2022 and JUCE 9.0.6.

CI compilation and manual application validation are separate checks. Engine/model availability still depends on the separately installed runtime and model files.

## Main features

- Native JUCE desktop interface for Windows.
- Voice-reference selection from WAV and MP3 files.
- Voice-library cards and text-entry workflow.
- Pocket TTS backend configured for `portuguese_24l`.
- Sopro V2 Turbo backend through the `audio.cpp` command-line runtime.
- European Portuguese generation path in the Sopro backend.
- Generated-audio playback and WAV export.
- Backend output validation in the Sopro path, including WAV readability, finite samples, non-silence, mono channel count and 24 kHz sample rate.
- Local runtime architecture; no xAI API integration.

## Screenshots and evidence

Application screenshots and the Windows validation evidence are maintained by the project owner. The repository's `docs/TEST-REPORT-0.2.9.25.md` records the scope and evidence status. Only genuine screenshots captured from the running application should be added here.

## Requirements

- Windows 11 x64.
- Visual Studio 2022 with **Desktop development with C++** and a Windows SDK.
- CMake 3.22 or newer.
- JUCE 9.0.6, obtained separately under the applicable JUCE licence.
- Inno Setup 6 only when building the installer.
- The runtime and model assets for the selected speech engine.

## Build from source

1. Install Visual Studio 2022 and CMake.
2. Obtain JUCE 9.0.6 and review its licence terms.
3. Open PowerShell in the repository root.
4. Set `JUCE_DIR` to the JUCE source directory and run:

```powershell
$env:JUCE_DIR = "C:\path\to\JUCE-9.0.6"
Set-ExecutionPolicy -Scope Process Bypass -Force
.\scripts\build-windows.ps1 -JuceDir $env:JUCE_DIR -Config Release
```

The repository includes a GitHub Actions workflow at `.github/workflows/windows-build.yml`. It configures and builds the Windows x64 application using Visual Studio 2022 and JUCE 9.0.6. A successful workflow uploads the executable as a workflow artifact.

## Runtime setup

### Pocket TTS

The Pocket TTS executable and model assets are not included in this source repository. Install the compatible runtime separately and follow its own licence and model terms. The current application source selects the `portuguese_24l` model identifier.

Upstream project: https://github.com/kyutai-labs/pocket-tts

### Sopro V2 Turbo

The Sopro integration uses the `audio.cpp` command-line runtime and Sopro model assets. Review `scripts/install-sopro.ps1`, `INTEGRACAO-SOPRO-0.2.9.23.md` and `docs/SECURITY-AND-LICENSING.md` before installing or redistributing third-party components.

Upstream projects:
- https://github.com/0xShug0/audio.cpp
- https://huggingface.co/samuel-vitorino/sopro-v2-turbo

Do not assume that the licence of an engine's source code automatically covers its executable builds, model weights or other downloaded assets.

## Documentation

- [User guide](docs/USER-GUIDE.md)
- [Architecture](docs/ARCHITECTURE.md)
- [Windows validation report](docs/TEST-REPORT-0.2.9.25.md)
- [0.2.9.25 release notes](docs/RELEASE-0.2.9.25.md)
- [Security and third-party licensing](docs/SECURITY-AND-LICENSING.md)
- [Contributing](CONTRIBUTING.md)
- [Security reporting](SECURITY.md)
- [Changelog](CHANGELOG.md)

## Privacy and responsible use

Voice recordings may identify a person and can contain sensitive personal information. Use only recordings for which you have permission and a lawful basis. Keep voice references, generated speech, model caches, authentication tokens and private logs out of public commits.

Do not use voice cloning to impersonate someone, mislead listeners or publish another person's voice without permission.

## Licensing

No project-source licence has been selected in this repository. Public visibility does not grant a general right to reuse, modify or redistribute the Vocloning Native source code. JUCE, Pocket TTS, audio.cpp and model assets are subject to their own terms. See `docs/SECURITY-AND-LICENSING.md`.

## Project owner

**INDROBEATZ** · https://github.com/INDROBEATZ