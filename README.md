# Vocloning Native

<p align="center"><img src="Assets/VocloningLogo.png" alt="Vocloning Native" width="420"></p>

<p align="center"><strong>Native Windows voice studio · C++20 · JUCE 9.0.6</strong><br>Local voice generation and voice-reference workflows, with a focus on European Portuguese.</p>

<p align="center"><a href="https://github.com/INDROBEATZ/VocloningNative/actions/workflows/windows-build.yml"><img src="https://github.com/INDROBEATZ/VocloningNative/actions/workflows/windows-build.yml/badge.svg" alt="Windows build"></a> <img src="https://img.shields.io/badge/platform-Windows%2011%20x64-blue"> <img src="https://img.shields.io/badge/language-C%2B%2B20-orange"> <img src="https://img.shields.io/badge/status-development-yellow"></p>

**Current source baseline:** `0.2.9.24 HOTFIX`  
**Next development line:** `0.2.9.25` (preparation branch; not yet a validated release)

Vocloning Native is a Windows desktop application built with C++ and JUCE. It provides a graphical workflow for importing voice references, entering text, generating speech through a locally installed engine, previewing audio and exporting WAV files.

The project is designed around local runtimes. It does **not** integrate the xAI API.

> **Release status:** this repository contains source code and build scripts. The Windows CI workflow verifies compilation; it does not prove that third-party model runtimes are installed or that end-to-end voice generation works on every machine. No installer is release-ready until built and tested on Windows 11.

## Current capabilities in the source

- Native JUCE desktop interface.
- Import voice reference files in WAV or MP3 format.
- Voice-reference library and text-entry workflow.
- Local Pocket TTS backend configured for the `portuguese_24l` model identifier.
- Local Sopro V2 Turbo backend through the `audio.cpp` command-line runtime, with European Portuguese generation options.
- Sopro WAV output validation: readability, finite samples, non-silence, mono channel count and 24 kHz sample rate.
- Audio preview and WAV export.

Actual engine availability depends on separately installed executables, model files, hardware and upstream compatibility.

## Screenshots

The repository currently includes the application logo and installer artwork. Genuine screenshots of the running application will be added after a Windows 11 validation run.

## Requirements

- Windows 11 x64.
- Visual Studio 2022 with the **Desktop development with C++** workload and Windows SDK.
- CMake 3.22 or newer.
- JUCE 9.0.6, obtained separately under its applicable licence.
- Inno Setup 6, only if building the installer.
- Pocket TTS and/or Sopro runtime and model files, depending on the engine selected.

## Build from source

Install Visual Studio 2022 and CMake. Obtain JUCE 9.0.6 from the official JUCE repository or website and review the JUCE 9 EULA. Open PowerShell in the repository root:

```powershell
$env:JUCE_DIR = "C:\path\to\JUCE-9.0.6"
Set-ExecutionPolicy -Scope Process Bypass -Force
.\scripts\build-windows.ps1 -JuceDir $env:JUCE_DIR -Config Release
```

Expected executable:

```text
build\VocloningNative_artefacts\Release\Vocloning.exe
```

The GitHub Actions workflow in `.github/workflows/windows-build.yml` performs a clean Windows x64 CMake/MSVC build using JUCE 9.0.6. It is a compile check, not a runtime or model-quality certification.

## Runtime setup

### Pocket TTS

The Pocket TTS executable and model assets are not committed. Obtain the runtime from the upstream project, review its licence and model terms, and install it locally as described in [Runtime/README.txt](Runtime/README.txt). The application code currently selects `portuguese_24l`.

Upstream: [Kyutai Pocket TTS](https://github.com/kyutai-labs/pocket-tts)

### Sopro V2 Turbo

The Sopro integration uses the `audio.cpp` CLI and Sopro model assets. The installer currently downloads a latest-release CLI, a model specification from the upstream default branch and model files from Hugging Face. These are moving targets and are **not yet pinned to immutable versions or verified with published SHA-256 checksums**.

Do not run the installer blindly on a production machine. Review [scripts/install-sopro.ps1](scripts/install-sopro.ps1) and [docs/SECURITY-AND-LICENSING.md](docs/SECURITY-AND-LICENSING.md) first. Do not redistribute third-party binaries or model weights until their exact licence and notice requirements have been confirmed.

Upstream: [audio.cpp](https://github.com/0xShug0/audio.cpp) · [Sopro model](https://huggingface.co/samuel-vitorino/sopro-v2-turbo)

## Documentation

- [Architecture and data flow](docs/ARCHITECTURE.md)
- [Security and third-party licensing review](docs/SECURITY-AND-LICENSING.md)
- [0.2.9.24 static audit](docs/AUDITORIA-0.2.9.24.md)
- [Sopro integration notes](INTEGRACAO-SOPRO-0.2.9.23.md)
- [0.2.9.25 release plan](docs/RELEASE-0.2.9.25-PLAN.md)
- [Changelog](CHANGELOG-0.2.9.24-HOTFIX.md)

## Privacy and security

Voice-reference recordings and generated audio may be sensitive personal data. Use only recordings you have the right and permission to process. Keep model caches, local authentication tokens, private voice samples, generated audio, build outputs and installer artifacts out of Git commits.

The Pocket TTS backend may read a local Hugging Face token file and expose the token to its child process through the process environment. The Sopro setup script downloads executable code and model assets from upstream sources. See the security review before using these paths.

## Licensing

No licence has been selected for the Vocloning Native application source itself. Until the project owner adds a licence, the public repository is viewable on GitHub but is not an open-source grant of permission to reuse, modify or redistribute the project source.

JUCE, Pocket TTS, audio.cpp and model weights have separate terms. See [the third-party licensing review](docs/SECURITY-AND-LICENSING.md). This documentation is not legal advice.

## Contributing and reporting issues

Please include Windows edition/build and CPU architecture, Visual Studio and CMake versions, JUCE version and exact build command, selected engine/runtime version, and relevant logs. Remove tokens, private paths and personal audio before sharing diagnostics.

Do not attach voice samples or model files unless you have the rights to share them.

## Project owner

**INDROBEATZ** · [GitHub repository](https://github.com/INDROBEATZ/VocloningNative)  
Portuguese documentation: [README.pt-PT.md](README.pt-PT.md)