#pragma once
#include <JuceHeader.h>

// Native JUCE bridge to the locally installed audio.cpp Sopro V2 Turbo CLI.
// No browser, WebView, HTTP service, or xAI API is involved.
class SoproTTSBackend
{
public:
    struct Result
    {
        bool success = false;
        juce::String message;
        juce::File outputFile;
    };

    static juce::File getInstallRoot();
    static juce::File getCliExecutable();
    static juce::File getModelDirectory();
    static juce::File getModelSpec();
    static bool isAvailable();
    static Result generatePortugueseEuropean(const juce::File& referenceAudio,
                                             const juce::String& text,
                                             const juce::File& outputFile);
};
