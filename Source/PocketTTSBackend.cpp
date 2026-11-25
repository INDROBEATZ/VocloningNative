#include "PocketTTSBackend.h"
#include "Utf8.h"
#include <cstdlib>
#include <mutex>
#include <string>
#include <cmath>
#include <limits>

namespace
{
    std::mutex pocketProcessMutex;

    class HuggingFaceTokenScope
    {
    public:
        HuggingFaceTokenScope()
        {
           #if JUCE_WINDOWS
            const auto tokenFile = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
                                       .getChildFile(".cache").getChildFile("huggingface")
                                       .getChildFile("token");
            if (tokenFile.existsAsFile())
            {
                const auto token = tokenFile.loadFileAsString().trim();
                if (token.isNotEmpty())
                {
                    char* current = nullptr;
                    size_t currentSize = 0;
                    if (_dupenv_s(&current, &currentSize, "HF_TOKEN") == 0 && current != nullptr)
                    {
                        hadPrevious = true;
                        previous.assign(current, currentSize > 0 ? currentSize - 1 : 0);
                        std::free(current);
                    }
                    if (_putenv_s("HF_TOKEN", token.toRawUTF8()) == 0)
                        changed = true;
                }
            }
           #endif
        }

        ~HuggingFaceTokenScope()
        {
           #if JUCE_WINDOWS
            if (changed)
                _putenv_s("HF_TOKEN", hadPrevious ? previous.c_str() : "");
           #endif
        }

    private:
        std::string previous;
        bool hadPrevious = false;
        bool changed = false;
    };

    bool runPocketCommand(const juce::StringArray& args, juce::String& log, juce::String& error)
    {
        juce::ChildProcess process;
        // Do not pipe stdout/stderr here: a verbose runtime can fill an OS pipe
        // before waitForProcessToFinish returns, deadlocking the child process.
        // The official runtime is launched directly and its exit code is authoritative.
        if (!process.start(args))
        {
            error = Vocloning::utf8("N\303\243o foi poss\303\255vel iniciar o Pocket TTS.");
            return false;
        }
        if (!process.waitForProcessToFinish(15 * 60 * 1000))
        {
            process.kill();
            error = Vocloning::utf8("A opera\303\247\303\243o excedeu o limite de 15 minutos.");
            return false;
        }
        log.clear();
        if (process.getExitCode() != 0)
        {
            error = log.substring(0, 1600);
            if (error.isEmpty()) error = "Pocket TTS terminou com erro.";
            return false;
        }
        return true;
    }

    bool createGrokCompatibleReference(const juce::File& input, juce::File& output, juce::String& error)
    {
        juce::AudioFormatManager formats;
        formats.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(input));
        if (!reader)
        {
            error = Vocloning::utf8("N\303\243o foi poss\303\255vel descodificar a refer\303\252ncia de voz.");
            return false;
        }

        const auto sampleRate = reader->sampleRate;
        if (sampleRate <= 0.0 || reader->numChannels <= 0 || reader->lengthInSamples <= 0)
        {
            error = Vocloning::utf8("A referência não contém áudio válido ou tem parâmetros inválidos.");
            return false;
        }
        const auto requestedSamples = (juce::int64)std::ceil(sampleRate * 30.0);
        const auto maxSamples = (int)juce::jmin<juce::int64>(reader->lengthInSamples,
                                      juce::jmin<juce::int64>(requestedSamples, (juce::int64)std::numeric_limits<int>::max()));
        if (maxSamples < (int)(sampleRate * 2.0))
        {
            error = Vocloning::utf8("A refer\303\252ncia deve ter pelo menos 2 segundos de \303\241udio.");
            return false;
        }

        juce::AudioBuffer<float> source((int)reader->numChannels, maxSamples);
        if (!reader->read(&source, 0, maxSamples, 0, true, true))
        {
            error = Vocloning::utf8("Falha ao ler o \303\241udio da refer\303\252ncia.");
            return false;
        }

        juce::AudioBuffer<float> mono(1, maxSamples);
        mono.clear();
        for (int channel = 0; channel < source.getNumChannels(); ++channel)
            mono.addFrom(0, 0, source, channel, 0, maxSamples, 1.0f / (float)source.getNumChannels());

        output.getParentDirectory().createDirectory();
        juce::WavAudioFormat wav;
        std::unique_ptr<juce::OutputStream> stream = output.createOutputStream();
        if (!stream)
        {
            error = Vocloning::utf8("N\303\243o foi poss\303\255vel criar o WAV tempor\303\241rio da refer\303\252ncia.");
            return false;
        }
        auto writer = wav.createWriterFor(stream, juce::AudioFormatWriter::Options{}
            .withSampleRate(sampleRate).withNumChannels(1).withBitsPerSample(16));
        if (!writer)
        {
            error = Vocloning::utf8("N\303\243o foi poss\303\255vel preparar o WAV mono da refer\303\252ncia.");
            return false;
        }
        if (!writer->writeFromAudioSampleBuffer(mono, 0, maxSamples))
        {
            error = Vocloning::utf8("Falha ao gravar a refer\303\252ncia tempor\303\241ria.");
            return false;
        }
        return true;
    }

    juce::String europeanPortugueseHint(const juce::String& text)
    {
        struct Term { const char* word; const char* replacement; };
        static const Term terms[] = {
            { "você", "tu" }, { "celular", "telemóvel" }, { "ônibus", "autocarro" },
            { "onibus", "autocarro" }, { "trem", "comboio" }, { "tela", "ecrã" },
            { "usuário", "utilizador" }, { "usuario", "utilizador" },
            { "arquivo", "ficheiro" }, { "baixar", "descarregar" },
            { "aplicativo", "aplicação" }, { "a gente", "nós" },
            { "cadê", "onde está" }, { "cade", "onde está" },
            { "beleza", "está bem" }, { "time", "equipa" }
        };
        auto lower = text.toLowerCase();
        for (const auto& term : terms)
        {
            const juce::String word = juce::String::fromUTF8(term.word);
            if (lower.containsWholeWord(word))
                return Vocloning::utf8("Este texto contém uma expressão normalmente associada ao português do Brasil: «")
                    + word + Vocloning::utf8("». Para manter pt-PT, considera «")
                    + juce::String::fromUTF8(term.replacement) + "» antes de gerar.";
        }
        return {};
    }

    juce::File getVoiceEmbeddingFile(const juce::File& reference)
    {
        const auto fingerprint = reference.getFullPathName() + "|" +
            juce::String(reference.getSize()) + "|" +
            juce::String(reference.getLastModificationTime().toMilliseconds());
        const auto key = juce::String::toHexString((juce::int64)fingerprint.hashCode64());
        auto cache = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                         .getChildFile("INDRO").getChildFile("Vocloning")
                         .getChildFile("Voices").getChildFile("PocketTTS24L");
        cache.createDirectory();
        return cache.getChildFile("voice_v02912_" + key + ".safetensors");
    }
}

juce::File PocketTTSBackend::getRuntimeExecutable()
{
    auto app = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    auto local = app.getChildFile("Runtime").getChildFile("pocket-tts.exe");
    if (local.existsAsFile()) return local;
    auto data = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                    .getChildFile("INDRO").getChildFile("Vocloning")
                    .getChildFile("Runtime").getChildFile("pocket-tts.exe");
    return data;
}

juce::File PocketTTSBackend::getModelDirectory()
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("INDRO").getChildFile("Vocloning").getChildFile("Models")
        .getChildFile("PocketTTS");
}

bool PocketTTSBackend::isRuntimeAvailable()
{
    return getRuntimeExecutable().existsAsFile();
}

PocketTTSBackend::Result PocketTTSBackend::generatePortuguese24L(
    const juce::File& referenceAudio, const juce::String& text, const juce::File& outputFile)
{
    Result result;
    if (!isRuntimeAvailable())
    {
        result.message = Vocloning::utf8("Pocket TTS runtime n\303\243o encontrado. Coloca pocket-tts.exe na pasta Runtime.");
        return result;
    }
    if (!referenceAudio.existsAsFile())
    {
        result.message = Vocloning::utf8("A amostra de voz selecionada j\303\241 n\303\243o existe.");
        return result;
    }
    if (!referenceAudio.hasFileExtension("wav") && !referenceAudio.hasFileExtension("mp3"))
    {
        result.message = Vocloning::utf8("Formato n\303\243o suportado. Usa WAV ou MP3.");
        return result;
    }
    if (text.trim().length() < 2)
    {
        result.message = "Escreve pelo menos dois caracteres para gerar voz.";
        return result;
    }
    const auto dialectHint = europeanPortugueseHint(text);
    if (dialectHint.isNotEmpty())
    {
        result.message = dialectHint;
        return result;
    }

    // Serialize process launches because HF_TOKEN is inherited from this process environment.
    const std::lock_guard<std::mutex> processLock(pocketProcessMutex);
    HuggingFaceTokenScope tokenScope;

    outputFile.getParentDirectory().createDirectory();
    auto voiceEmbedding = getVoiceEmbeddingFile(referenceAudio);

    // Use up to 30 seconds: the official Pocket TTS CLI documents this maximum.
    // Keep a versioned cache so embeddings created by the older 15-second pipeline are not reused.
    if (!voiceEmbedding.existsAsFile() || voiceEmbedding.getSize() < 1000)
    {
        auto tempReference = juce::File::getSpecialLocation(juce::File::tempDirectory)
            .getChildFile("VocloningNative").getChildFile(juce::Uuid().toString() + "_reference30s.wav");
        juce::String prepError;
        if (!createGrokCompatibleReference(referenceAudio, tempReference, prepError))
        {
            result.message = prepError;
            return result;
        }

        juce::StringArray exportArgs;
        exportArgs.add(getRuntimeExecutable().getFullPathName());
        exportArgs.add("export-voice");
        exportArgs.add(tempReference.getFullPathName());
        exportArgs.add(voiceEmbedding.getFullPathName());
        exportArgs.add("--language"); exportArgs.add("portuguese_24l");
        exportArgs.add("--quiet");

        juce::String exportLog, exportError;
        const bool exported = runPocketCommand(exportArgs, exportLog, exportError);
        tempReference.deleteFile();
        if (!exported || !voiceEmbedding.existsAsFile() || voiceEmbedding.getSize() < 1000)
        {
            voiceEmbedding.deleteFile();
            result.message = Vocloning::utf8("N\303\243o foi poss\303\255vel preparar a identidade vocal. ") + exportError;
            if (result.message.length() < 80) result.message += exportLog.substring(0, 1000);
            return result;
        }
    }

    juce::StringArray args;
    args.add(getRuntimeExecutable().getFullPathName());
    args.add("generate");
    args.add("--language"); args.add("portuguese_24l");
    args.add("--voice"); args.add(voiceEmbedding.getFullPathName());
    args.add("--text"); args.add(text.trim());
    args.add("--output-path"); args.add(outputFile.getFullPathName());
    // Prioritise synthesis fidelity over INT8 speed on this voice-cloning workflow.
    args.add("--sampler-decode-steps"); args.add("5");
    args.add("--quiet");

    // Never let an old/partial file make a failed generation look successful.
    outputFile.deleteFile();
    juce::String log, error;
    if (!runPocketCommand(args, log, error) || !outputFile.existsAsFile() || outputFile.getSize() < 1000)
    {
        outputFile.deleteFile();
        result.message = Vocloning::utf8("Pocket TTS n\303\243o conseguiu gerar o WAV. ") + error;
        if (result.message.length() < 100) result.message += log.substring(0, 1000);
        return result;
    }

    // Validate that the generated artifact is actually a readable audio file.
    juce::AudioFormatManager outputFormats;
    outputFormats.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> outputReader(outputFormats.createReaderFor(outputFile));
    if (!outputReader || outputReader->sampleRate <= 0.0 || outputReader->numChannels <= 0 || outputReader->lengthInSamples <= 0)
    {
        outputFile.deleteFile();
        result.message = Vocloning::utf8("O Pocket TTS terminou, mas o ficheiro WAV gerado está inválido.");
        return result;
    }

    // A readable WAV can still contain silence. Inspect the generated samples
    // before reporting success so the UI never claims that silent output is ready.
    constexpr int blockSize = 8192;
    juce::AudioBuffer<float> checkBuffer((int) outputReader->numChannels, blockSize);
    double sumSquares = 0.0;
    double peak = 0.0;
    juce::int64 countedSamples = 0;
    for (juce::int64 start = 0; start < outputReader->lengthInSamples; start += blockSize)
    {
        const int count = (int) juce::jmin<juce::int64>(blockSize, outputReader->lengthInSamples - start);
        checkBuffer.clear();
        if (!outputReader->read(&checkBuffer, 0, count, start, true, true))
        {
            outputFile.deleteFile();
            result.message = Vocloning::utf8("Não foi possível validar os dados de áudio do WAV gerado.");
            return result;
        }
        for (int channel = 0; channel < checkBuffer.getNumChannels(); ++channel)
        {
            const auto* samples = checkBuffer.getReadPointer(channel);
            for (int i = 0; i < count; ++i)
            {
                const double sample = samples[i];
                if (!std::isfinite(sample))
                {
                    outputFile.deleteFile();
                    result.message = Vocloning::utf8("O WAV gerado contém amostras inválidas (NaN ou infinito).");
                    return result;
                }
                sumSquares += sample * sample;
                peak = juce::jmax(peak, std::abs(sample));
                ++countedSamples;
            }
        }
    }
    const double rms = countedSamples > 0 ? std::sqrt(sumSquares / (double) countedSamples) : 0.0;
    if (peak < 0.0001 || rms < 0.00001)
    {
        outputFile.deleteFile();
        result.message = Vocloning::utf8("O Pocket TTS criou um WAV praticamente silencioso. A geração não foi considerada bem-sucedida.");
        return result;
    }

    result.success = true;
    result.outputFile = outputFile;
    result.message = Vocloning::utf8("Voz gerada com Pocket TTS 24L. Identidade vocal reutilizada da refer\303\252ncia.");
    return result;
}
