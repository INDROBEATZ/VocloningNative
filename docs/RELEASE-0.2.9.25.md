# Vocloning Native 0.2.9.25 — Release Notes

**Release track:** preparation
**Status:** documentation and CI validation recorded; final installer/release packaging remains a separate gate.

## Highlights

- Native Windows application built with C++ and JUCE.
- Local speech-engine integration through Pocket TTS and Sopro V2 Turbo backends.
- Pocket TTS configured for the `portuguese_24l` model identifier.
- Sopro V2 Turbo European Portuguese workflow.
- Voice-reference selection and voice-library workflow.
- Speech generation, playback and WAV export.
- Windows x64 CI build using MSVC 2022 and JUCE 9.0.6.

## Validation record

The GitHub Actions Windows x64 Release build completed successfully in run `45689868801` on commit `ecfee99ddb7749fb23d58a7d25d44374f203f8d6`.

The project owner supplied Windows 11 evidence of the Sopro V2 Turbo workflow, including reference selection, speech generation, playback and WAV export. See [the validation report](TEST-REPORT-0.2.9.25.md).

## Dependencies

The application requires separately installed runtime/model assets for the selected engine. Third-party runtime binaries and model weights are not included in the source repository unless their redistribution terms have been explicitly verified.

## Important notes

- The project source has no selected licence yet.
- JUCE 9.0.6 has separate licence terms.
- Pocket TTS, audio.cpp and model artifacts have their own notices/terms.
- Runtime versions and model revisions should be pinned and hash-verified before public binary distribution.
- A successful CI build does not certify every engine, model, acoustic quality metric or installer scenario.

## Release checklist

- [x] Windows x64 CI configure and Release build.
- [x] CI executable artifact uploaded.
- [x] Owner-provided Windows workflow evidence recorded.
- [x] Sopro WAV output inspected.
- [ ] Confirm exact application version metadata in source, executable and installer.
- [ ] Pin all external runtime/model versions and verify hashes.
- [ ] Confirm project and third-party licensing/redistribution requirements.
- [ ] Test installer install, upgrade and uninstall on a clean Windows 11 system.
- [ ] Publish final signed/verified release assets and checksums.