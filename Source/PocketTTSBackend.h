#pragma once
#include <JuceHeader.h>

// Native Windows process bridge for the official Pocket TTS standalone runtime.
// The JUCE application remains fully native; no browser or HTTP service is used.
class PocketTTSBackend
{
public:
    struct Result
    {
        bool success = false;
        juce::String message;
        juce::File outputFile;
    };

    static juce::File getRuntimeExecutable();
    static juce::File getModelDirectory();
    static bool isRuntimeAvailable();
    static Result generatePortuguese24L(const juce::File& referenceAudio,
                                         const juce::String& text,
                                         const juce::File& outputFile);
};
