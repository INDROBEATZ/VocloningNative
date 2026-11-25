# Vocloning Native — User Guide

## 1. Purpose

Vocloning Native is a Windows desktop application for working with voice references and generating speech through locally installed engines. The exact engines available in the interface depend on the runtime and model assets installed on the computer.

## 2. Prepare the computer

Before opening the application:

1. Use Windows 11 x64.
2. Confirm that the required speech runtime is installed.
3. Confirm that the matching model files are present.
4. Keep the runtime and model directories in the locations expected by the application or its configuration.
5. Do not move or rename runtime files while a generation is running.

Pocket TTS and Sopro V2 Turbo are separate engines. Installing one does not automatically install the other.

## 3. Add a voice reference

1. Open Vocloning Native.
2. Use the voice-reference selection control.
3. Select a supported WAV or MP3 audio file.
4. Confirm that the selected reference appears in the voice library.
5. Use a recording that is clear, contains the intended speaker and has permission for this use.

For the most reliable result, use a clean recording with one speaker, little background noise and no music masking the voice.

## 4. Enter text and choose an engine

1. Select a voice/reference card or the intended reference workflow.
2. Enter the text to be spoken.
3. Select the desired engine in the voice-engine selector.
4. Check the selected language/locale option before generating.

The Pocket TTS integration uses the `portuguese_24l` model identifier in the current source. The Sopro V2 Turbo integration includes a European Portuguese generation path.

## 5. Generate and listen

1. Press the generate control.
2. Wait for the selected local runtime to finish.
3. If generation fails, confirm that the runtime executable and model paths are correct.
4. Use the playback control to listen to the generated audio.
5. If the result is not satisfactory, review the reference recording and text, then generate again.

Generation time depends on the selected engine, model, CPU/GPU support and text length.

## 6. Export WAV

1. Use the WAV download/export control.
2. Choose the destination and filename in the Windows save dialog.
3. Save the file.
4. Open the exported WAV in an audio editor or player to confirm it is readable and contains the expected speech.

The Sopro backend validates its generated WAV for readability, finite samples, non-silence, mono channel count and a 24 kHz sample rate.

## 7. Troubleshooting

### Engine not available

- Confirm the runtime executable exists.
- Confirm the model directory and model specification exist.
- Check the runtime installation notes.
- Restart the application after changing environment variables or runtime paths.

### Generation fails

- Check that the reference file can be opened.
- Try a short test sentence.
- Confirm that the selected engine's runtime can run independently.
- Inspect application/runtime logs, removing tokens and private file paths before sharing them.

### Output is silent or invalid

- Confirm the engine completed successfully.
- Check the generated WAV in an audio editor.
- For Sopro, confirm the expected runtime/model compatibility and inspect the backend validation message.

### Audio quality is poor

- Use a cleaner reference with less noise and reverberation.
- Avoid overlapping speakers and background music.
- Try a shorter text sample.
- Compare results using the same reference and text so that engine differences are meaningful.

## 8. Privacy and consent

Voice recordings are personal data. Use only voices you are authorized to process. Do not use generated speech to impersonate someone, deceive listeners or publish a third party's voice without permission. Keep private recordings and generated files out of public issue reports.

## 9. Known distinction

A successful C++ build confirms that the application compiles. It does not by itself certify model installation, voice similarity, pronunciation quality or every hardware configuration. See the validation report for the tested scope.