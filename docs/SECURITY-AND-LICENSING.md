# Security and Third-Party Licensing

## Scope

This is an engineering checklist for the Vocloning Native source repository and its external speech-engine dependencies. It is not legal advice or a complete software bill of materials.

## Project source licence

No licence has been selected for the Vocloning Native application source. Do not add an MIT, Apache-2.0 or other licence without the copyright holder's explicit decision. A public GitHub repository does not automatically grant general permission to reuse, modify or redistribute the project source.

## JUCE

JUCE 9.0.6 has its own licence framework and EULA. Confirm the correct licence tier and product-distribution rights for the project before distributing binaries.

Official terms: https://juce.com/legal/juce-9-licence/

## Pocket TTS

The upstream Pocket TTS code repository includes an MIT licence. Preserve the applicable copyright and permission notices if redistributing covered code. Review the exact runtime and model terms separately; do not assume the code licence automatically covers every model or binary.

Upstream: https://github.com/kyutai-labs/pocket-tts

## audio.cpp and Sopro

Review the audio.cpp licence and notices, and the exact Sopro model card/revision and all associated model assets. The licence for framework code does not necessarily determine the licence of weights or third-party artifacts.

Upstream:
- https://github.com/0xShug0/audio.cpp
- https://huggingface.co/samuel-vitorino/sopro-v2-turbo

Before a public installer:
1. Pin an exact runtime release/tag and commit.
2. Pin an immutable model repository revision.
3. Record SHA-256 for every downloaded file.
4. Verify hashes before extraction or execution.
5. Preserve required third-party notices and licence texts.
6. Test compatibility between the selected runtime and model snapshot.

## Credentials and local data

- Never commit Hugging Face tokens, API credentials or authentication files.
- Never include private voice references or generated audio in public commits.
- Remove personal paths and tokens from diagnostic logs before sharing.
- Treat voice recordings as sensitive personal data.
- Review child-process environment handling so credentials are not inherited by unrelated processes.

## Installer and downloads

Review `scripts/install-sopro.ps1` before executing it. Production distribution should use pinned immutable downloads, SHA-256 validation, clear error handling and least-privilege environment configuration.

## Responsible voice use

Use only voice recordings you are authorized to process. Do not impersonate people, deceive listeners or distribute another person's voice without permission.

## Reporting a vulnerability

See `SECURITY.md`. Do not publish exploitable security details or credentials in a public issue.