# Contributing

Thank you for your interest in Vocloning Native.

## Before opening an issue

Include:
- Windows edition/build and CPU architecture.
- Visual Studio and CMake versions.
- JUCE version.
- Exact build command and relevant build output.
- Selected speech engine and runtime/model version.
- Reproduction steps and expected versus actual behavior.

Remove access tokens, personal paths, private voice recordings and generated audio before sharing logs.

## Pull requests

- Keep changes focused.
- Explain the problem and the proposed solution.
- Update documentation when behavior or setup changes.
- Do not commit build directories, executables, model weights, caches, tokens, private references or generated WAV files.
- Test the Windows x64 Release build when changing C++ or CMake.
- Include manual test results for changes to audio generation, playback or export.

## Licensing

Do not copy third-party code, binaries or model assets into this repository unless their terms permit it and the required notices are included. The Vocloning Native project licence has not yet been selected.