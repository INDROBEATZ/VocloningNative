# Vocloning Native 0.2.9.25 - Release plan

Status: preparation only; not a release candidate.  
Baseline: 0.2.9.24 HOTFIX.  
Working branch: release/0.2.9.25-prep.

## P0 - Security and reproducibility
- [ ] Pin audio.cpp release and commit used by the Sopro installer.
- [ ] Pin Sopro model repository revision.
- [ ] Add SHA-256 verification for every downloaded runtime/spec/model artifact.
- [ ] Replace machine-wide environment mutation where possible.
- [ ] Review subprocess environment handling for Pocket TTS credentials.
- [ ] Confirm third-party notices and redistribution rights.
- [ ] Decide and add the Vocloning Native source licence.

## P1 - Build and packaging
- [ ] Obtain a clean Windows x64 CI build with JUCE 9.0.6.
- [ ] Run the build script on a clean Windows 11 machine.
- [ ] Confirm Inno Setup packaging paths and generated installer.
- [ ] Test install, upgrade, uninstall and interrupted setup.
- [ ] Add SHA-256 checksums for release artifacts.

## P2 - Functional validation
- [ ] First launch and GUI layout.
- [ ] Import WAV reference.
- [ ] Import MP3 reference.
- [ ] Save and reopen reference library.
- [ ] Pocket TTS synthesis using intended portuguese_24l runtime.
- [ ] Sopro V2 Turbo synthesis in European Portuguese.
- [ ] Preview generated audio.
- [ ] Export WAV and verify sample rate/channels.
- [ ] Test missing runtime, invalid reference and failed generation.
- [ ] Verify logs do not expose tokens or private voice paths.

## P3 - Documentation and release
- [ ] Add genuine application screenshots from a validated Windows build.
- [ ] Publish English and Portuguese documentation.
- [ ] Create a GitHub Release only after P0-P2 gates pass.
- [ ] Attach installer, source archive and SHA-256 manifest.
- [ ] Mark the release as prerelease until all checks pass.

## Version consistency

Version is currently duplicated in CMakeLists.txt, Source/Main.cpp and installer/VocloningNative.iss. Centralize the version before tagging 0.2.9.25 so application metadata, installer and release tag cannot drift.

## Release policy

A successful CI compile is necessary but not sufficient. Do not label 0.2.9.25 stable until the runtime engines and installer have been tested on Windows 11 with exact pinned dependencies.