#include "SoproTTSBackend.h"
#include "Utf8.h"
#include <cmath>
#include <mutex>
#include <limits>

namespace
{
    std::mutex soproProcessMutex;

    juce::File appDirectory()
    {
        return juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    }

    bool validateAudioFile(const juce::File& file, juce::String& error)
    {
        juce::AudioFormatManager formats;
        formats.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
        if (!reader || reader->sampleRate <= 0.0 || reader->numChannels <= 0 || reader->lengthInSamples <= 0)
        {
            error = Vocloning::utf8("O ficheiro de áudio não é válido ou não pode ser descodificado.");
            return false;
        }

        constexpr int blockSize = 8192;
        juce::AudioBuffer<float> buffer((int) reader->numChannels, blockSize);
        double sumSquares = 0.0;
        double peak = 0.0;
        juce::int64 sampleCount = 0;
        for (juce::int64 start = 0; start < reader->lengthInSamples; start += blockSize)
        {
            const int count = (int) juce::jmin<juce::int64>(blockSize, reader->lengthInSamples - start);
            buffer.clear();
            if (!reader->read(&buffer, 0, count, start, true, true))
            {
                error = Vocloning::utf8("Não foi possível ler o áudio gerado.");
                return false;
            }
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                const auto* samples = buffer.getReadPointer(ch);
                for (int i = 0; i < count; ++i)
                {
                    const double sample = samples[i];
                    if (!std::isfinite(sample))
                    {
                        error = Vocloning::utf8("O WAV contém amostras inválidas.");
                        return false;
                    }
                    sumSquares += sample * sample;
                    peak = juce::jmax(peak, std::abs(sample));
                    ++sampleCount;
                }
            }
        }
        const double rms = sampleCount > 0 ? std::sqrt(sumSquares / (double) sampleCount) : 0.0;
        if (peak < 0.0001 || rms < 0.00001)
        {
            error = Vocloning::utf8("O Sopro criou um WAV praticamente silencioso.");
            return false;
        }
        return true;
    }

    bool runSopro(const juce::StringArray& args, juce::String& error)
    {
        juce::ChildProcess process;
        // Keep stdout/stderr unpiped: the CLI can emit progress/metrics and a
        // full pipe could block the child before it exits.
        if (!process.start(args))
        {
            error = Vocloning::utf8("Não foi possível iniciar o motor Sopro.");
            return false;
        }
        if (!process.waitForProcessToFinish(15 * 60 * 1000))
        {
            process.kill();
            error = Vocloning::utf8("A geração Sopro excedeu o limite de 15 minutos.");
            return false;
        }
        if (process.getExitCode() != 0)
        {
            error = Vocloning::utf8("O Sopro terminou com o código de erro ") + juce::String(process.getExitCode()) + ".";
            return false;
        }
        return true;
    }
}

juce::File SoproTTSBackend::getInstallRoot()
{
    // Preferred portable layout: <Vocloning.exe>/Runtime/Sopro/audio.cpp/...
    auto portable = appDirectory().getChildFile("Runtime").getChildFile("Sopro").getChildFile("audio.cpp");
    if (portable.getChildFile("build/windows-cpu-release/bin/audiocpp_cli.exe").existsAsFile())
        return portable;

    // Explicit override allows a future installer to choose another location.
    const auto overridePath = juce::SystemStats::getEnvironmentVariable("VOCLONING_SOPRO_ROOT", {}).trim();
    if (overridePath.isNotEmpty())
    {
        auto candidate = juce::File(overridePath);
        if (candidate.isDirectory()) return candidate;
    }

    // Installer-managed portable runtime (first-install default).
    auto installedPortable = appDirectory().getChildFile("Runtime/Sopro/audio.cpp");
    if (installedPortable.getChildFile("build/windows-cpu-release/bin/audiocpp_cli.exe").existsAsFile())
        return installedPortable;

    // Shared machine installation created by the setup bootstrap.
    auto programData = juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory)
        .getChildFile("Vocloning/Sopro/audio.cpp");
    if (programData.getChildFile("build/windows-cpu-release/bin/audiocpp_cli.exe").existsAsFile())
        return programData;

    // Current user installation created and tested on this Windows machine.
    auto installed = juce::File("C:/AI/Sopro-PT-PT/audio.cpp");
    if (installed.isDirectory()) return installed;
    return portable;
}

juce::File SoproTTSBackend::getCliExecutable()
{
    auto root = getInstallRoot();
    auto cli = root.getChildFile("build/windows-cpu-release/bin/audiocpp_cli.exe");
    if (cli.existsAsFile()) return cli;
    // Also support a flattened Runtime/Sopro package.
    auto flat = appDirectory().getChildFile("Runtime/Sopro/audiocpp_cli.exe");
    return flat;
}

juce::File SoproTTSBackend::getModelDirectory()
{
    auto root = getInstallRoot();
    auto model = root.getChildFile("models/sopro-v2-turbo");
    if (model.isDirectory()) return model;
    return appDirectory().getChildFile("Runtime/Sopro/models/sopro-v2-turbo");
}

juce::File SoproTTSBackend::getModelSpec()
{
    auto root = getInstallRoot();
    auto spec = root.getChildFile("model_specs/sopro_tts.json");
    if (spec.existsAsFile()) return spec;
    return appDirectory().getChildFile("Runtime/Sopro/model_specs/sopro_tts.json");
}

bool SoproTTSBackend::isAvailable()
{
    return getCliExecutable().existsAsFile()
        && getModelDirectory().isDirectory()
        && getModelSpec().existsAsFile()
        && getModelDirectory().getChildFile("config.json").existsAsFile()
        && getModelDirectory().getChildFile("model.safetensors").existsAsFile()
        && getModelDirectory().getChildFile("semantic_encoder.safetensors").existsAsFile()
        && getModelDirectory().getChildFile("speaker_encoder.safetensors").existsAsFile()
        && getModelDirectory().getChildFile("tokenizer.model").existsAsFile()
        && getModelDirectory().getChildFile("vocoder.safetensors").existsAsFile();
}

SoproTTSBackend::Result SoproTTSBackend::generatePortugueseEuropean(
    const juce::File& referenceAudio, const juce::String& text, const juce::File& outputFile)
{
    Result result;
    if (!isAvailable())
    {
        result.message = Vocloning::utf8("Sopro V2 Turbo não encontrado. Verifica o CLI, o modelo e model_specs/sopro_tts.json.");
        return result;
    }
    if (!referenceAudio.existsAsFile())
    {
        result.message = Vocloning::utf8("A amostra de voz selecionada já não existe.");
        return result;
    }
    if (!referenceAudio.hasFileExtension("wav") && !referenceAudio.hasFileExtension("mp3"))
    {
        result.message = Vocloning::utf8("Formato não suportado. Usa WAV ou MP3.");
        return result;
    }
    if (text.trim().length() < 2)
    {
        result.message = Vocloning::utf8("Escreve pelo menos dois caracteres para gerar voz.");
        return result;
    }

    const std::lock_guard<std::mutex> lock(soproProcessMutex);
    outputFile.getParentDirectory().createDirectory();
    outputFile.deleteFile();

    juce::StringArray args;
    args.add(getCliExecutable().getFullPathName());
    args.add("--task"); args.add("tts");
    args.add("--family"); args.add("sopro_tts");
    args.add("--model"); args.add(getModelDirectory().getFullPathName());
    args.add("--model-spec-override"); args.add(getModelSpec().getFullPathName());
    args.add("--backend"); args.add("cpu");
    args.add("--threads"); args.add(juce::String(juce::jlimit(2, 4, juce::SystemStats::getNumCpus())));
    args.add("--text"); args.add(text.trim());
    args.add("--voice-ref"); args.add(referenceAudio.getFullPathName());
    args.add("--request-option"); args.add("language=pt");
    args.add("--out"); args.add(outputFile.getFullPathName());
    args.add("--metrics");

    juce::String processError;
    if (!runSopro(args, processError) || !outputFile.existsAsFile() || outputFile.getSize() < 1000)
    {
        outputFile.deleteFile();
        result.message = Vocloning::utf8("Falha na geração Sopro V2 Turbo. ") + processError;
        return result;
    }

    juce::String validationError;
    if (!validateAudioFile(outputFile, validationError))
    {
        outputFile.deleteFile();
        result.message = validationError;
        return result;
    }

    juce::AudioFormatManager formats;
    formats.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(outputFile));
    if (!reader || std::abs(reader->sampleRate - 24000.0) > 1.0 || reader->numChannels != 1)
    {
        outputFile.deleteFile();
        result.message = Vocloning::utf8("O Sopro gerou áudio, mas o WAV não corresponde ao formato esperado de 24 kHz mono.");
        return result;
    }

    result.success = true;
    result.outputFile = outputFile;
    result.message = Vocloning::utf8("Voz gerada com Sopro V2 Turbo em português europeu (pt-PT). WAV validado.");
    return result;
}
