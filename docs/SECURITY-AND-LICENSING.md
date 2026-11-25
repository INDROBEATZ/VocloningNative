# Security and third-party licensing review

Review date: 2026-11-25  
Baseline: Vocloning Native 0.2.9.24 HOTFIX  
Scope: static review of the source archive and public repository files. This is not a penetration test, legal opinion or complete software bill of materials.

## Executive status

| Area | Status | Notes |
|---|---|---|
| Vocloning Native source licence | Decision required | No LICENSE file; public visibility does not grant reuse rights |
| JUCE 9.0.6 | EULA and licence tier must be checked | JUCE 9 is governed by JUCE's own licence framework |
| Pocket TTS code | MIT identified upstream | Preserve notices if redistributing covered code |
| Pocket TTS runtime/model | Verify separately | Do not assume the code licence covers every runtime/model artifact |
| audio.cpp code | Apache-2.0 identified upstream | Preserve applicable notices and licence text |
| Sopro V2 Turbo model | Apache-2.0 is declared in the current Hugging Face model card | Pin the exact model revision and verify the files served to the installer |
| Download integrity | Needs improvement | Installer follows mutable latest/main references and has no SHA-256 verification |
| Secrets | No obvious hardcoded token pattern in reviewed text | Static scan is limited; Pocket backend reads a local HF token file |
| Windows validation | Pending | No clean native build or end-to-end engine test is claimed here |

## Project-owned source

No licence has been selected for Vocloning Native. Do not add MIT, Apache-2.0 or another licence without an explicit decision by the copyright holder. Until a licence is added, the public repository is viewable but downstream users do not receive a general licence to reuse, modify or redistribute the source.

## JUCE 9.0.6

JUCE 9 is governed by JUCE's own licence framework and EULA, not a blanket permissive open-source grant. The official JUCE 9 EULA lists Starter, Indie, Pro and Educational licence types, revenue/funding thresholds and developer-seat/product-distribution obligations.

Before distributing a product built with JUCE 9.0.6, confirm the applicable licence tier for the project owner and each developer, the permitted use case, and continuing distribution/maintenance rights. Do not redistribute JUCE source or modules separately from what the EULA permits.

Official terms: https://juce.com/legal/juce-9-licence/

## Pocket TTS

The upstream Pocket TTS repository contains an MIT licence for project code. If Pocket TTS code is copied or redistributed, retain its copyright and permission notice. This application currently integrates an external standalone runtime and does not commit its executable. Review runtime packaging and model weights independently; do not infer their terms solely from the code repository licence.

Upstream: https://github.com/kyutai-labs/pocket-tts  
MIT notice: https://github.com/kyutai-labs/pocket-tts/blob/main/LICENSE

## audio.cpp and Sopro

The audio.cpp upstream project identifies its code as Apache-2.0. The framework licence does not automatically cover model weights: upstream documentation says model weights retain the terms of their original release.

The current Hugging Face model card for `samuel-vitorino/sopro-v2-turbo` declares Apache-2.0 and describes English, European Portuguese, French and German support. The model card also states that current artifacts require `sopro>=2.2.0` or `@soprotts/onnx-web>=0.3.0`. The current Vocloning setup script instead obtains a Windows CPU archive from the latest audio.cpp release, a model specification from the default branch and model files from the Hugging Face main revision. Compatibility between that CLI build and the current model snapshot must therefore be tested rather than assumed. These references are mutable.

Before production packaging:
1. Select and record an exact audio.cpp release/tag and commit.
2. Select an immutable Sopro model repository revision.
3. Obtain the exact model-card and licence files for that revision and all bundled components.
4. Record SHA-256 for every downloaded file.
5. Validate hashes before extraction or use.
6. Preserve third-party notices in installer and release archive.

Sopro model card: https://huggingface.co/samuel-vitorino/sopro-v2-turbo  
Upstream: https://github.com/0xShug0/audio.cpp  
Model licence guidance: https://github.com/0xShug0/audio.cpp/blob/main/docs/model_licenses.md  
Sopro model: https://huggingface.co/samuel-vitorino/sopro-v2-turbo

## Credential handling

The Pocket TTS backend checks the local Hugging Face token file and temporarily exposes the token as HF_TOKEN in the parent process environment so the child process can inherit it. The code restores the previous value and serializes launches because the environment variable is process-wide.

Do not log or display token contents. Do not commit Hugging Face cache files. Avoid concurrent unrelated child processes that might inherit HF_TOKEN. Prefer a per-child environment block if supported by the process API or a small launcher helper.

## Installer and download chain

The current scripts/install-sopro.ps1 has the following production risks:
- Resolves the latest audio.cpp release at install time.
- Downloads a model specification from a mutable default branch.
- Downloads model weights from a mutable Hugging Face branch.
- Checks minimum file sizes but not cryptographic hashes.
- Writes VOCLONING_SOPRO_ROOT at Machine scope, which can require elevation and affects all users.

Recommended 0.2.9.25 changes:
- Pin immutable versions and revisions.
- Verify SHA-256 before moving partial downloads into final locations.
- Fail closed on checksum mismatch.
- Prefer a user-scoped setting or app-relative path unless machine-wide configuration is required.
- Produce installation logs without secrets.
- Test cancellation, interrupted downloads, retries, upgrade and uninstall.

## Static scan

The reviewed text sources did not reveal obvious hardcoded API keys, passwords or access tokens. The scan did identify the deliberate Hugging Face token-file integration described above. This does not prove the absence of secrets in binaries, archives, future commits or generated artifacts.

Before each public release, inspect staged files and the full diff, run a secret scanner, confirm .gitignore excludes runtime binaries/model weights/caches/voice samples/generated WAVs/build outputs, and review release assets separately.

## Release gate

Do not publish a public Windows installer until the project licence decision is recorded, JUCE licence eligibility is confirmed, third-party redistribution rights and notices are confirmed, downloads are pinned and hash-verified, a clean Windows 11 x64 build succeeds, and install/first launch/both engine paths/playback/export/upgrade/uninstall have been tested.

This document is an engineering checklist, not legal advice.