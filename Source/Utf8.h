#pragma once
#include <JuceHeader.h>

namespace Vocloning
{
    // Interpret byte sequences explicitly as UTF-8, independent of Windows ACP.
    inline juce::String utf8(const char* text)
    {
        return juce::String::fromUTF8(text);
    }
}
