#pragma once
#include <JuceHeader.h>

struct VoiceEntry
{
    juce::String id;
    juce::String name;
    juce::File referenceFile;
    juce::String engine = "Local engine (not connected)";
    juce::Time createdAt;
};

class VoiceLibrary
{
public:
    VoiceLibrary();
    const juce::Array<VoiceEntry>& getVoices() const noexcept { return voices; }
    bool addVoice(const juce::String& name, const juce::File& reference, juce::String& error);
    void removeVoice(int index);
    juce::File getStorageDirectory() const;

private:
    void load();
    void save() const;
    juce::Array<VoiceEntry> voices;
};
