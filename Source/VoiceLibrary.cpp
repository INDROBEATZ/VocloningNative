#include "VoiceLibrary.h"
#include "Utf8.h"

VoiceLibrary::VoiceLibrary() { load(); }

juce::File VoiceLibrary::getStorageDirectory() const
{
    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("INDRO").getChildFile("Vocloning").getChildFile("Voices");
    dir.createDirectory();
    return dir;
}

void VoiceLibrary::load()
{
    voices.clear();
    auto file = getStorageDirectory().getChildFile("library.json");
    if (!file.existsAsFile()) return;
    auto parsed = juce::JSON::parse(file);
    if (auto* array = parsed.getArray())
    {
        for (const auto& item : *array)
        {
            if (auto* obj = item.getDynamicObject())
            {
                VoiceEntry v;
                v.id = obj->getProperty("id").toString();
                v.name = obj->getProperty("name").toString();
                v.referenceFile = juce::File(obj->getProperty("reference").toString());
                v.engine = obj->getProperty("engine").toString();
                v.createdAt = juce::Time((juce::int64)obj->getProperty("createdAt"));
                if (v.name.isNotEmpty()) voices.add(v);
            }
        }
    }
}

void VoiceLibrary::save() const
{
    juce::Array<juce::var> array;
    for (const auto& v : voices)
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("id", v.id);
        obj->setProperty("name", v.name);
        obj->setProperty("reference", v.referenceFile.getFullPathName());
        obj->setProperty("engine", v.engine);
        obj->setProperty("createdAt", (juce::int64)v.createdAt.toMilliseconds());
        array.add(juce::var(obj));
    }
    getStorageDirectory().getChildFile("library.json")
        .replaceWithText(juce::JSON::toString(juce::var(array), true));
}

bool VoiceLibrary::addVoice(const juce::String& name, const juce::File& reference, juce::String& error)
{
    if (name.trim().length() < 2) { error = "O nome da voz deve ter pelo menos 2 caracteres."; return false; }
    if (!reference.existsAsFile()) { error = Vocloning::utf8("O ficheiro de refer\303\252ncia n\303\243o existe."); return false; }
    if (reference.getSize() == 0) { error = Vocloning::utf8("O ficheiro est\303\241 vazio."); return false; }
    VoiceEntry v;
    v.id = juce::Uuid().toString();
    v.name = name.trim();
    v.referenceFile = reference;
    v.createdAt = juce::Time::getCurrentTime();
    voices.insert(0, v);
    save();
    return true;
}

void VoiceLibrary::removeVoice(int index)
{
    if (juce::isPositiveAndBelow(index, voices.size()))
    {
        voices.remove(index);
        save();
    }
}
