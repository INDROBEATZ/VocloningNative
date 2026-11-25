# Vocloning Native — Windows Validation Report

**Report:** 0.2.9.25 preparation
**Platform:** Windows 11 x64
**Project owner:** INDROBEATZ
**Report status:** owner-reported manual validation plus successful GitHub Actions build

## 1. Summary

The project owner reports that the application is functioning on the Windows 11 test machine. Supplied visual/video evidence demonstrates the Sopro V2 Turbo workflow, including voice-reference selection, generation, playback and WAV export.

The CI build completed successfully using the Windows 2022 runner, MSVC 2022 and JUCE 9.0.6.

## 2. CI build record

| Field | Result |
|---|---|
| Repository | `INDROBEATZ/VocloningNative` |
| Branch | `release/0.2.9.25-prep` |
| Commit | `ecfee99ddb7749fb23d58a7d25d44374f203f8d6` |
| Workflow run | `45689868801` |
| Job | `MSVC 2022 - JUCE 9.0.6 - x64` |
| Configure | Success |
| Release build | Success |
| Executable artifact upload | Success |
| Artifact size | 4,899,773 bytes |
| Artifact SHA-256 digest reported by GitHub | `2876f614fb80a97515f95ef77a91dd6befe7fbb88ff96469558f4e309a042474` |

Workflow run: https://github.com/INDROBEATZ/VocloningNative/actions/runs/45689868801

The digest above is the digest reported for the GitHub Actions artifact archive. It is not represented as the SHA-256 of the extracted executable.

## 3. Manual application evidence

The project owner supplied screenshots, a screen recording and an exported `Vocloning.wav` file.

The visible evidence demonstrates:
- Opening the voice-reference file picker.
- Selecting a reference file from a local Windows directory.
- Selecting the Sopro V2 Turbo — European Portuguese engine.
- Entering text for speech generation.
- Starting generation.
- Playback of the generated audio.
- Opening the WAV save dialog and exporting the result.

The supplied WAV was inspected and found to be readable, mono, 24,000 Hz, approximately 4.024 seconds long, with finite non-silent samples. The measured peak was approximately -5.98 dBFS and RMS approximately -22.78 dBFS.

These measurements describe the supplied output file only; they are not a universal quality benchmark.

## 4. Validation scope

| Area | Status | Evidence |
|---|---|---|
| Windows x64 CMake/MSVC Release build | Passed | GitHub Actions run 45689868801 |
| JUCE 9.0.6 checkout/configuration | Passed | CI job steps |
| Application launch and visible UI | Owner-reported / visual evidence | Supplied Windows capture/video |
| Voice-reference selection | Passed in supplied evidence | File picker capture |
| Sopro V2 Turbo selection | Passed in supplied evidence | Engine selector capture |
| Sopro speech generation | Passed in supplied evidence | Screen recording and WAV |
| Playback | Passed in supplied evidence | Screen recording |
| WAV export | Passed in supplied evidence | Save dialog and exported WAV |
| Pocket TTS `portuguese_24l` | Owner reports application functionality; independent repeatable test record not attached to this report | Owner confirmation |
| Installer install/upgrade/uninstall | Not covered by this report | Separate installer test required |

## 5. Interpretation

The CI result proves that the configured Windows x64 Release build completed and produced an executable artifact. The manual evidence supports the Sopro user workflow on the project owner's machine.

The report does not claim:
- A formal acoustic similarity score or speaker-identity benchmark.
- Validation on every Windows PC or hardware configuration.
- That the CI runner contains the production speech models.
- That all third-party redistribution rights have been cleared.
- That the final installer has passed clean-machine install, upgrade and uninstall testing.

## 6. Reproduction

Open the workflow run above and download the artifact named `VocloningNative-windows-x64-ecfee99ddb7749fb23d58a7d25d44374f203f8d6`. Extract it and test it on a Windows 11 x64 machine with the required external runtime and model assets installed.

For source builds, follow the build instructions in the repository README.

## 7. Sign-off

This report records the evidence available for the 0.2.9.25 preparation line. The project owner is responsible for confirming the final release candidate and the exact version metadata embedded in the executable and installer.