#pragma once
#include "Utf8.h"
#include <JuceHeader.h>
#include "VoiceLibrary.h"
#include "PocketTTSBackend.h"
#include <thread>
#include <atomic>

class MainComponent final : public juce::Component,
                            private juce::Button::Listener,
                            private juce::TextEditor::Listener,
                            private juce::ComboBox::Listener
{
public:
    MainComponent();
    ~MainComponent() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    enum class Page { Reference, Text, Guide, Settings };
    class VoiceGrid;
    void buttonClicked(juce::Button*) override;
    void textEditorTextChanged(juce::TextEditor&) override;
    void comboBoxChanged(juce::ComboBox*) override;
    void showPage(Page);
    void showGuideStep(int);
    void importReference();
    void updateStatus(const juce::String&);
    void updateVoiceControls();
    void updateActionState();
    void generateSpeech();
    void exportGeneratedAudio();
    void playReference(int index);
    void playAudioFile(const juce::File& file);
    void refreshGrid();
    void styleButton(juce::TextButton&, bool primary = false);
    juce::Rectangle<int> contentBounds() const;

    // Owned by MainComponent: avoids JUCE LookAndFeel construction during global startup.
    std::unique_ptr<juce::LookAndFeel> look;
    juce::Image brandLogo;
    VoiceLibrary library;
    std::unique_ptr<VoiceGrid> voiceGrid;

    juce::TextButton navReference { "Referência" }, navText { "Texto" },
        navGuide { "Guia" }, navSettings { "Definições" }, topGuide { "Guia" };
    juce::TextButton importButton { "Carregar WAV ou MP3" }, recordButton { "Gravar referência" },
        removeButton { "Remover voz" }, generateButton { "Dizer na voz clonada" },
        playButton { "Ouvir outra vez" }, exportButton { "Descarregar" };
    juce::TextButton guideStep1 { "1. O que faz" }, guideStep2 { "2. A referência" },
        guideStep3 { "3. Texto" }, guidePrevious { "Anterior" }, guideNext { "Seguinte" },
        guideOpenReference { "Ir à referência" }, guideOpenText { "Abrir o texto" };
    juce::TextEditor voiceName, textToSpeak;
    juce::ComboBox voiceSelector, modelSelector;
    juce::Label pageEyebrow, pageHeadline, pageDescription, promptLabel, sampleTextPreview, voiceNameLabel,
        modelLabel, charCounter, status, engineInfo, settingsIdentity,
        settingsEquality, settingsHosting, guideStepLabel, guideCardTitle,
        guideBody1, guideBody2;
    juce::ToggleButton consent;
    juce::FileChooser chooser { "Escolhe uma amostra de voz", juce::File{}, "*.wav;*.mp3" };
    Page currentPage = Page::Reference;
    int guideStep = 0;
    int selectedVoiceIndex = 0;
    juce::File generatedAudio;
    juce::AudioDeviceManager audioDeviceManager;
    bool audioReady = false;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioTransportSource audioTransport;
    std::unique_ptr<juce::AudioFormatReaderSource> currentAudioSource;
    std::atomic<bool> generationRunning { false };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
