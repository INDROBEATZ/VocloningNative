#include "MainComponent.h"
#include "SoproTTSBackend.h"
#include "Utf8.h"
#include <BinaryData.h>
#include <cmath>
#include <array>

namespace
{
 const juce::Colour bg(0xff090909), header(0xff111111), side(0xff111111), card(0xff171717),
 field(0xff090909), edge(0xff414141), red(0xffed0808), white(0xfff7f7f7),
 muted(0xffc3c3c3), dim(0xff858585);
 class VocloningLook final : public juce::LookAndFeel_V4
 {
 public:
  void drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour& c, bool over, bool down) override
  {
   auto r=b.getLocalBounds().toFloat().reduced(1); bool p=b.getComponentID()=="primary";
   if(p && b.isEnabled()) for(int i=4;i>0;--i){g.setColour(red.withAlpha(over?0.05f:0.018f));g.drawRoundedRectangle(r.expanded((float)i),8+i,1);}
   auto f=down?c.darker(.18f):(over?c.brighter(.12f):c);
   g.setColour(f);g.fillRoundedRectangle(r,7);
   g.setColour(p?red:(over?juce::Colour(0xff777777):edge));g.drawRoundedRectangle(r,7,p?1.4f:1.0f);
  }
  void drawButtonText(juce::Graphics& g, juce::TextButton& b, bool, bool) override
  {
   const auto colour = b.findColour(b.getToggleState() ? juce::TextButton::textColourOnId
                                                        : juce::TextButton::textColourOffId);
   g.setColour(colour);
   g.setFont(juce::Font(juce::FontOptions(19.0f, juce::Font::bold)));
   g.drawFittedText(b.getButtonText(), b.getLocalBounds().reduced(10, 2),
                    juce::Justification::centred, 1);
  }
 };
}
class MainComponent::VoiceGrid final : public juce::Component
{
public:
 explicit VoiceGrid(MainComponent& m):owner(m){}
 void paint(juce::Graphics& g) override
 {
  g.fillAll(bg); const auto& v=owner.library.getVoices();
  int cols=getWidth()>=1180?3:(getWidth()>=700?2:1), gap=20, cw=(getWidth()-gap*(cols-1))/cols, ch=220;
  for(int i=0;i<v.size();++i){
   int x=(i%cols)*(cw+gap),y=(i/cols)*(ch+gap); auto r=juce::Rectangle<float>((float)x,(float)y,(float)cw,(float)ch);
   g.setColour(card);g.fillRoundedRectangle(r,10);g.setColour(i==selected?red:edge);g.drawRoundedRectangle(r,10,i==selected?1.5f:1);
   g.setColour(dim);g.setFont(17);g.drawText(Vocloning::utf8("Referência"),x+18,y+12,cw-36,20,juce::Justification::left);
   g.setColour(white);g.setFont(juce::Font(juce::FontOptions(23,juce::Font::bold)));g.drawText(v.getReference(i).name,x+18,y+38,cw-36,28,juce::Justification::left);
   g.setColour(muted);g.setFont(17.0f);g.drawText(v.getReference(i).referenceFile.getFileName(),x+18,y+72,cw-36,22,juce::Justification::left);
   g.setColour(i==selected?red:juce::Colour(0xffaaaaaa));juce::Path p;p.startNewSubPath((float)x+20,y+132);
   for(int n=0;n<18;++n)p.lineTo(x+20+n*(cw-50)/17.0f,y+132+std::sin((float)(n*1.7+i))*9);
   g.strokePath(p,juce::PathStrokeType(1.4f));
   g.setColour(juce::Colour(0xff303030));g.fillRoundedRectangle((float)x+14,y+174,(float)cw-28,40,8);
   g.setColour(white);g.setFont(17);g.drawText(Vocloning::utf8("Escrever texto"),x+22,y+181,cw-100,26,juce::Justification::centredLeft);
   g.setColour(dim);g.setFont(juce::Font(juce::FontOptions(13, juce::Font::bold)));
   g.drawText("OUVIR", x+cw-104, y+181, 48, 24, juce::Justification::centred);
   g.drawText("APAGAR", x+cw-58, y+181, 52, 24, juce::Justification::centred);
  }
  if(v.isEmpty()){g.setColour(card);g.fillRoundedRectangle(getLocalBounds().toFloat(),10);g.setColour(muted);g.setFont(14);g.drawText(Vocloning::utf8("Ainda não tens referências. Carrega um WAV ou MP3 para começar."),getLocalBounds().reduced(18),juce::Justification::centred);}
 }
 void mouseDown(const juce::MouseEvent& e) override
 {
  const auto& v=owner.library.getVoices();int cols=getWidth()>=1180?3:(getWidth()>=700?2:1),gap=20,cw=(getWidth()-gap*(cols-1))/cols,ch=220;
  int col=juce::jlimit(0,cols-1,e.x/(cw+gap)),row=juce::jmax(0,e.y/(ch+gap)),i=row*cols+col;
  if(!juce::isPositiveAndBelow(i,v.size()))return;selected=i;owner.selectedVoiceIndex=i;owner.updateVoiceControls();
  int lx=e.x-col*(cw+gap),ly=e.y-row*(ch+gap);
  if(ly>=174&&lx>=cw-48){owner.library.removeVoice(i);selected=-1;owner.selectedVoiceIndex=juce::jmax(0, i-1);owner.refreshGrid();owner.updateVoiceControls();}
  else if(ly>=174&&lx>=cw-82)owner.playReference(i);
  else if(ly>=174)owner.showPage(Page::Text);
  repaint();
 }
private: MainComponent& owner;int selected=-1;
};

MainComponent::MainComponent()
{
 // Construct the custom LookAndFeel only after JUCE has initialized its GUI runtime.
 look = std::make_unique<VocloningLook>();
 brandLogo=juce::ImageCache::getFromMemory(BinaryData::VocloningLogo_png,BinaryData::VocloningLogo_pngSize);
 auto nav=[this](juce::TextButton& b){b.setLookAndFeel(look.get());b.setColour(juce::TextButton::buttonColourId,side);b.setColour(juce::TextButton::textColourOffId,muted);b.setColour(juce::TextButton::textColourOnId,white);b.addListener(this);addAndMakeVisible(b);};
 for(auto* b:{&navReference,&navText,&navGuide,&navSettings,&topGuide}){nav(*b);b->setClickingTogglesState(false);b->setColour(juce::TextButton::textColourOffId,white);b->setColour(juce::TextButton::textColourOnId,white);b->setLookAndFeel(look.get());}
 navReference.setButtonText(Vocloning::utf8("Referência"));navText.setButtonText("Texto");navGuide.setButtonText("Guia");navSettings.setButtonText(Vocloning::utf8("Definições"));topGuide.setButtonText("Guia");
 auto action=[this](juce::TextButton& b,bool p){b.setLookAndFeel(look.get());b.setColour(juce::TextButton::buttonColourId,p?red:juce::Colour(0xff242424));b.setColour(juce::TextButton::textColourOffId,white);b.setColour(juce::TextButton::textColourOnId,white);if(p)b.setComponentID("primary");b.addListener(this);addAndMakeVisible(b);};
 for(auto* b:{&importButton,&recordButton,&removeButton,&generateButton,&playButton,&exportButton,&guideStep1,&guideStep2,&guideStep3,&guidePrevious,&guideNext,&guideOpenReference,&guideOpenText}){action(*b,b==&importButton||b==&generateButton||b==&guideNext);}
 importButton.setButtonText(Vocloning::utf8("Carregar ficheiro WAV ou MP3"));recordButton.setButtonText(Vocloning::utf8("Gravar referência"));generateButton.setButtonText(Vocloning::utf8("Gerar voz clonada"));playButton.setButtonText(Vocloning::utf8("Ouvir novamente"));exportButton.setButtonText(Vocloning::utf8("Descarregar WAV"));
 guideStep1.setButtonText(Vocloning::utf8("1. Como funciona"));guideStep2.setButtonText(Vocloning::utf8("2. A referência"));guideStep3.setButtonText(Vocloning::utf8("3. Texto"));guidePrevious.setButtonText(Vocloning::utf8("Anterior"));guideNext.setButtonText(Vocloning::utf8("Seguinte"));guideOpenReference.setButtonText(Vocloning::utf8("Ir à referência"));guideOpenText.setButtonText(Vocloning::utf8("Abrir o texto"));
 auto lab=[this](juce::Label& l,float s,juce::Colour c,bool bold=false){l.setFont(juce::Font(juce::FontOptions(s,bold?juce::Font::bold:juce::Font::plain)));l.setColour(juce::Label::textColourId,c);l.setJustificationType(juce::Justification::topLeft);addAndMakeVisible(l);};
 for(auto* l:{&pageEyebrow,&pageHeadline,&pageDescription,&promptLabel,&sampleTextPreview,&voiceNameLabel,&modelLabel,&charCounter,&status,&engineInfo,&settingsIdentity,&settingsEquality,&settingsHosting,&guideStepLabel,&guideCardTitle,&guideBody1,&guideBody2})lab(*l,18,muted);
 pageHeadline.setFont(juce::Font(juce::FontOptions(46,juce::Font::bold)));pageHeadline.setColour(juce::Label::textColourId,white);
 pageEyebrow.setColour(juce::Label::textColourId,red);pageEyebrow.setFont(juce::Font(juce::FontOptions(17,juce::Font::bold)));
 voiceNameLabel.setText(Vocloning::utf8("Nome da voz"),juce::dontSendNotification);promptLabel.setText(Vocloning::utf8("Lê isto em voz alta, em português europeu"),juce::dontSendNotification);
 sampleTextPreview.setText(Vocloning::utf8("Olá. Esta amostra é em português de Portugal. O comboio chegou à estação às oito e um quarto. Gostava de um café e de um pastel de nata, se fizer favor. O telemóvel ficou na secretária, ao lado da janela."),juce::dontSendNotification);
 sampleTextPreview.setColour(juce::Label::backgroundColourId,field);
 sampleTextPreview.setColour(juce::Label::textColourId,white);
 sampleTextPreview.setBorderSize(juce::BorderSize<int>(10,14,10,14));
 sampleTextPreview.setFont(juce::Font(juce::FontOptions(19)));
 sampleTextPreview.setJustificationType(juce::Justification::centredLeft);
 voiceName.setFont(juce::Font(juce::FontOptions(21)));voiceName.setTextToShowWhenEmpty(Vocloning::utf8("Por exemplo, Ana"),dim);voiceName.setColour(juce::TextEditor::backgroundColourId,field);voiceName.setColour(juce::TextEditor::textColourId,white);voiceName.setColour(juce::TextEditor::outlineColourId,edge);voiceName.setColour(juce::TextEditor::focusedOutlineColourId,red);addAndMakeVisible(voiceName);
 consent.setButtonText(Vocloning::utf8("Confirmo que esta voz é minha, ou que tenho autorização de quem fala, e que a amostra está em português de Portugal — não do Brasil."));consent.setColour(juce::ToggleButton::textColourId,muted);consent.setColour(juce::ToggleButton::tickColourId,red);addAndMakeVisible(consent);
 textToSpeak.setMultiLine(true);textToSpeak.setReturnKeyStartsNewLine(true);textToSpeak.setInputRestrictions(2000);textToSpeak.setScrollbarsShown(true);textToSpeak.setFont(juce::Font(juce::FontOptions(21)));textToSpeak.setText(Vocloning::utf8("Olá. Esta é a minha voz, clonada a partir da referência, em português de Portugal."));textToSpeak.setColour(juce::TextEditor::backgroundColourId,field);textToSpeak.setColour(juce::TextEditor::textColourId,white);textToSpeak.setColour(juce::TextEditor::outlineColourId,edge);textToSpeak.setColour(juce::TextEditor::focusedOutlineColourId,red);textToSpeak.setColour(juce::CaretComponent::caretColourId,red);textToSpeak.setIndents(12,10);textToSpeak.addListener(this);addAndMakeVisible(textToSpeak);
 for(auto* c:{&voiceSelector,&modelSelector}){c->setColour(juce::ComboBox::backgroundColourId,field);c->setColour(juce::ComboBox::textColourId,white);c->setColour(juce::ComboBox::outlineColourId,edge);c->addListener(this);addAndMakeVisible(*c);}
 modelSelector.addItem(Vocloning::utf8("Sopro V2 Turbo - Português europeu"),1);
 modelSelector.addItem(Vocloning::utf8("Pocket TTS - 24 línguas (alternativo)"),2);
 modelSelector.setSelectedId(1,juce::dontSendNotification);
 modelLabel.setText(Vocloning::utf8("Motor de voz"),juce::dontSendNotification);
 engineInfo.setText(SoproTTSBackend::isAvailable()
     ? Vocloning::utf8("PREDEFINIDO: Sopro V2 Turbo\nClonagem local zero-shot. Português europeu (pt). CPU. WAV mono 24 kHz.")
     : Vocloning::utf8("Sopro V2 Turbo selecionado, mas não foi encontrado. Confirma a instalação local em C:\\AI\\Sopro-PT-PT\\audio.cpp."),juce::dontSendNotification);
 status.setText(SoproTTSBackend::isAvailable()
     ? Vocloning::utf8("Sopro V2 Turbo disponível - motor predefinido.")
     : Vocloning::utf8("Sopro selecionado; motor ou modelo não encontrado."),juce::dontSendNotification);
 settingsIdentity.setText(Vocloning::utf8("IDENTIDADE FIXA\nTimbre, tom e mistura ficam a 100% na referência. Nada é puxado para outro sotaque.\n\nPT-PT"),juce::dontSendNotification);
 settingsEquality.setText(Vocloning::utf8("Como a igualdade é feita\nA frase passa pela voz registada a partir do teu WAV ou MP3, com a língua pt-PT. O sotaque pedido ao motor é de Portugal, nunca do Brasil."),juce::dontSendNotification);
 settingsHosting.setText(Vocloning::utf8("Processamento local\nSopro V2 Turbo corre através do motor audio.cpp instalado no computador. A referência e o texto são processados localmente; não é utilizada API externa nem WebView."),juce::dontSendNotification);
 voiceGrid=std::make_unique<VoiceGrid>(*this);
  addAndMakeVisible(*voiceGrid);

  // All child components must exist before setSize() triggers resized().
  // In particular, resized() positions voiceGrid; calling setSize earlier
  // dereferenced a null unique_ptr during startup.
  setSize(1440,900);

  updateVoiceControls();
  textEditorTextChanged(textToSpeak);
  showPage(Page::Reference);
}
MainComponent::~MainComponent()
{
 if (audioReady)
     audioDeviceManager.removeAudioCallback(&audioSourcePlayer);
 audioTransport.stop();
 audioTransport.setSource(nullptr);
 audioSourcePlayer.setSource(nullptr);
 for(auto* b:{&navReference,&navText,&navGuide,&navSettings,&topGuide,&importButton,&recordButton,&removeButton,&generateButton,&playButton,&exportButton,&guideStep1,&guideStep2,&guideStep3,&guidePrevious,&guideNext,&guideOpenReference,&guideOpenText})b->setLookAndFeel(nullptr);
}
juce::Rectangle<int> MainComponent::contentBounds() const
{
    const int sidebar = 280;
    const int available = juce::jmax(600, getWidth() - sidebar - 64);
    const int width = juce::jmin(1420, available);
    // Content begins below the top header; the previous 132 px offset
    // pushed the page heading and cards too far down, causing clipping.
    return { sidebar + 32, 18, width, juce::jmax(420, getHeight() - 50) };
}
void MainComponent::paint(juce::Graphics& g)
{
 g.fillAll(bg);int w=getWidth(),h=getHeight();g.setColour(header);g.fillRect(0,0,w,112);g.setColour(edge);g.drawLine(0,111,w,111,1);g.setColour(side);g.fillRect(0,112,280,h-112);g.drawLine(279,112,279,h,1);
 if(brandLogo.isValid()) g.drawImageWithin(brandLogo,20,5,250,102,juce::RectanglePlacement::centred);
 else {g.setColour(red);g.drawRoundedRectangle(18,20,58,58,11,1.8f);g.fillEllipse(37,39,18,18);g.setColour(white);g.setFont(juce::Font(juce::FontOptions(23,juce::Font::bold)));g.drawText("VOC",88,19,70,36,juce::Justification::left);g.setColour(red);g.drawText("LONING",158,19,120,36,juce::Justification::left);g.setColour(muted);g.setFont(15.0f);g.drawText(Vocloning::utf8("PT-PT - VOICE STUDIO"),88,57,190,24,juce::Justification::left);}
 auto c=contentBounds();g.setColour(red);g.setFont(juce::Font(juce::FontOptions(17.0f,juce::Font::bold)));g.drawText(Vocloning::utf8(currentPage==Page::Reference?"REFERÊNCIA":currentPage==Page::Text?"TEXTO PARA VOZ":currentPage==Page::Guide?"GUIA":"DEFINIÇÕES"),c.getX(),c.getY(),c.getWidth(),22,juce::Justification::left);
 if(currentPage==Page::Reference||currentPage==Page::Text)
 {
     const int y = c.getY() + 166;
     if (currentPage == Page::Text)
     {
         const bool sideBySide = c.getWidth() >= 740;
         const int leftW = sideBySide ? c.getWidth() - 250 : c.getWidth();
         const int panelH = juce::jlimit(360, 520, c.getHeight() - 184);
         g.setColour(card);
         g.fillRoundedRectangle((float)c.getX(), (float)y, (float)leftW, (float)panelH, 12);
         g.setColour(edge);
         g.drawRoundedRectangle((float)c.getX(), (float)y, (float)leftW, (float)panelH, 12, 1);
         const int sideX = sideBySide ? c.getX() + leftW + 14 : c.getX();
         const int sideY = sideBySide ? y : y + panelH + 14;
         const int sideW = sideBySide ? c.getWidth() - leftW - 14 : c.getWidth();
         g.setColour(card);
         g.fillRoundedRectangle((float)sideX, (float)sideY, (float)sideW, 154.0f, 10);
         g.setColour(edge);
         g.drawRoundedRectangle((float)sideX, (float)sideY, (float)sideW, 154.0f, 10, 1);
     }
     else
     {
         g.setColour(card);
         g.fillRoundedRectangle((float)c.getX(), (float)y, (float)c.getWidth(), 320.0f, 12);
         g.setColour(edge);
         g.drawRoundedRectangle((float)c.getX(), (float)y, (float)c.getWidth(), 320.0f, 12, 1);
     }
 }
 if(currentPage==Page::Guide){int y=c.getY()+180;g.setColour(card);g.fillRoundedRectangle((float)c.getX(),(float)y,(float)c.getWidth(),280,10);g.setColour(edge);g.drawRoundedRectangle((float)c.getX(),(float)y,(float)c.getWidth(),280,10,1);}
 if(currentPage==Page::Settings){for(int i=0;i<3;++i){int y=c.getY()+150+i*150;g.setColour(card);g.fillRoundedRectangle((float)c.getX(),(float)y,(float)c.getWidth(),128,12);g.setColour(edge);g.drawRoundedRectangle((float)c.getX(),(float)y,(float)c.getWidth(),128,12,1);}}
}
void MainComponent::resized()
{
    const int w = getWidth(), h = getHeight();
    navReference.setBounds(14, 132, 252, 54);
    navText.setBounds(14, 196, 252, 54);
    navGuide.setBounds(14, 260, 252, 54);
    navSettings.setBounds(14, 324, 252, 54);
    topGuide.setBounds(w - 164, 28, 142, 48);

    auto c = contentBounds();
    pageEyebrow.setBounds(c.getX(), c.getY(), c.getWidth(), 24);
    pageHeadline.setBounds(c.getX(), c.getY() + 32, c.getWidth(), 62);
    pageDescription.setBounds(c.getX(), c.getY() + 96, c.getWidth(), 52);

    const int y = c.getY() + 166;
    const int pad = 26;

    promptLabel.setBounds(c.getX() + pad, y + 18, c.getWidth() - pad * 2, 28);
    sampleTextPreview.setBounds(c.getX() + pad, y + 54, c.getWidth() - pad * 2, 82);
    voiceNameLabel.setBounds(c.getX() + pad, y + 146, c.getWidth() - pad * 2, 26);
    voiceName.setBounds(c.getX() + pad, y + 176, c.getWidth() - pad * 2, 50);
    consent.setBounds(c.getX() + pad, y + 236, c.getWidth() - pad * 2, 48);
    const int refButtonW = (c.getWidth() - pad * 2 - 14) / 2;
    importButton.setBounds(c.getX() + pad, y + 292, refButtonW, 56);
    recordButton.setBounds(c.getX() + pad + refButtonW + 14, y + 292, refButtonW, 56);
    if (voiceGrid != nullptr)
        voiceGrid->setBounds(c.getX(), y + 366, c.getWidth(), juce::jmax(150, h - y - 400));

    const bool sideBySide = c.getWidth() >= 740;
    const int leftW = sideBySide ? c.getWidth() - 250 : c.getWidth();
    const int innerW = leftW - pad * 2;
    const int panelH = juce::jlimit(360, 520, c.getHeight() - 184);
    voiceSelector.setBounds(c.getX() + pad, y + 54, innerW, 52);
    const int textY = y + 122;
    const int textH = juce::jmax(132, panelH - 228);
    textToSpeak.setBounds(c.getX() + pad, textY, innerW, textH);
    charCounter.setBounds(c.getX() + pad, textY + textH + 8, innerW, 24);

    const int buttonY = y + panelH - 64;
    const int gap = 12;
    const int buttonW = (innerW - gap * 2) / 3;
    generateButton.setBounds(c.getX() + pad, buttonY, buttonW, 48);
    playButton.setBounds(c.getX() + pad + buttonW + gap, buttonY, buttonW, 48);
    exportButton.setBounds(c.getX() + pad + (buttonW + gap) * 2, buttonY, innerW - (buttonW + gap) * 2, 48);

    const int sideX = sideBySide ? c.getX() + leftW + 14 : c.getX();
    const int sideY = sideBySide ? y : y + panelH + 14;
    const int sideW = sideBySide ? c.getWidth() - leftW - 14 : c.getWidth();
    modelLabel.setBounds(sideX + 20, sideY + 14, sideW - 40, 24);
    modelSelector.setBounds(sideX + 16, sideY + 44, sideW - 32, 46);
    engineInfo.setBounds(sideX + 20, sideY + 96, sideW - 40, 52);

    guideStep1.setBounds(c.getX(), c.getY() + 150, 150, 48);
    guideStep2.setBounds(c.getX() + 164, c.getY() + 150, 170, 48);
    guideStep3.setBounds(c.getX() + 348, c.getY() + 150, 130, 48);
    guideStepLabel.setBounds(c.getX() + 24, c.getY() + 224, c.getWidth() - 48, 28);
    guideCardTitle.setBounds(c.getX() + 24, c.getY() + 266, c.getWidth() - 48, 42);
    guideBody1.setBounds(c.getX() + 24, c.getY() + 322, c.getWidth() - 48, 78);
    guideBody2.setBounds(c.getX() + 24, c.getY() + 414, c.getWidth() - 48, 68);
    guideOpenReference.setBounds(c.getX() + 24, c.getY() + 500, 210, 48);
    guideOpenText.setBounds(c.getX() + 24, c.getY() + 500, 210, 48);
    guidePrevious.setBounds(c.getX(), c.getY() + 570, 140, 48);
    guideNext.setBounds(c.getX() + 154, c.getY() + 570, 170, 48);

    settingsIdentity.setBounds(c.getX() + 24, c.getY() + 150, c.getWidth() - 48, 112);
    settingsEquality.setBounds(c.getX() + 24, c.getY() + 286, c.getWidth() - 48, 112);
    settingsHosting.setBounds(c.getX() + 24, c.getY() + 422, c.getWidth() - 48, 112);
    status.setBounds(c.getX(), h - 38, c.getWidth(), 26);
}

void MainComponent::buttonClicked(juce::Button* b)
{
 if(b==&navReference)showPage(Page::Reference);else if(b==&navText)showPage(Page::Text);else if(b==&navGuide||b==&topGuide)showPage(Page::Guide);else if(b==&navSettings)showPage(Page::Settings);
 else if(b==&importButton)importReference();else if(b==&recordButton)updateStatus(Vocloning::utf8("A gravação por microfone ainda não está disponível. Carrega um ficheiro WAV ou MP3."));
 else if(b==&generateButton)generateSpeech();else if(b==&playButton){if(generatedAudio.existsAsFile())playAudioFile(generatedAudio);else updateStatus(Vocloning::utf8("Ainda não existe áudio gerado."));}else if(b==&exportButton)exportGeneratedAudio();
 else if(b==&guideStep1)showGuideStep(0);else if(b==&guideStep2)showGuideStep(1);else if(b==&guideStep3)showGuideStep(2);else if(b==&guidePrevious)showGuideStep(guideStep-1);else if(b==&guideNext){if(guideStep==2)showPage(Page::Reference);else showGuideStep(guideStep+1);}else if(b==&guideOpenReference)showPage(Page::Reference);else if(b==&guideOpenText)showPage(Page::Text);
}
void MainComponent::showPage(Page p)
{
 currentPage=p;for(auto* b:{&navReference,&navText,&navGuide,&navSettings}){b->setColour(juce::TextButton::buttonColourId,side);b->setColour(juce::TextButton::textColourOffId,muted);}
 auto active=[](juce::TextButton& b){b.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff252525));b.setColour(juce::TextButton::textColourOffId,white);};
 if(p==Page::Reference)active(navReference);if(p==Page::Text)active(navText);if(p==Page::Guide)active(navGuide);if(p==Page::Settings)active(navSettings);
 pageHeadline.setText(p==Page::Reference?Vocloning::utf8("Clona a voz a partir desta amostra."):p==Page::Text?Vocloning::utf8("Escreve. A voz clonada diz."):p==Page::Guide?Vocloning::utf8("Português de Portugal, preso à referência."):Vocloning::utf8("A voz sai igual à referência."),juce::dontSendNotification);
 pageDescription.setText(p==Page::Reference?Vocloning::utf8("A voz fica clonada no momento em que a carregas ou gravas, e abre logo em Texto. É português de Portugal. Não há voz brasileira neste sítio."):p==Page::Text?Vocloning::utf8("O Sopro V2 Turbo usa a amostra para clonar a identidade vocal e gera texto em português europeu."):p==Page::Guide?Vocloning::utf8("Três passos. A língua não se escolhe: é europeia."):Vocloning::utf8("Língua fechada em português de Portugal. Não há opção de português do Brasil, nem de outra língua."),juce::dontSendNotification);
 bool r=p==Page::Reference,t=p==Page::Text,g=p==Page::Guide,s=p==Page::Settings;
 std::array<juce::Component*, 7> referenceControls { &promptLabel, &voiceNameLabel, &voiceName, &consent, &importButton, &recordButton, voiceGrid.get() };
 for(auto* c:referenceControls)c->setVisible(r);
 std::array<juce::Component*, 9> textControls { &voiceSelector, &textToSpeak, &charCounter, &modelLabel, &modelSelector, &generateButton, &playButton, &exportButton, &engineInfo };
 for(auto* c:textControls)c->setVisible(t);
 std::array<juce::Component*, 9> guideControls { &guideStep1, &guideStep2, &guideStep3, &guideStepLabel, &guideCardTitle, &guideBody1, &guideBody2, &guidePrevious, &guideNext };
 for(auto* c:guideControls)c->setVisible(g);
 guideOpenReference.setVisible(g&&guideStep==1);guideOpenText.setVisible(g&&guideStep==2);
 settingsIdentity.setVisible(s);settingsEquality.setVisible(s);settingsHosting.setVisible(s);
 resized();repaint();if(t)updateActionState();
}
void MainComponent::showGuideStep(int step)
{
 guideStep=juce::jlimit(0,2,step);guideStepLabel.setText(Vocloning::utf8("Passo ")+juce::String(guideStep+1)+Vocloning::utf8(" de 3"),juce::dontSendNotification);
 const juce::String titles[]={Vocloning::utf8("A voz clonada diz o que escreves."),Vocloning::utf8("Fala em português europeu, sem ruído."),Vocloning::utf8("A amostra clona-se e abre pronta a falar.")};
 const juce::String body1[]={Vocloning::utf8("Gravas ou carregas uma amostra de quem queres clonar. Essa fala é a referência. O clone não inventa outro timbre, nem troca o sotaque para português do Brasil."),Vocloning::utf8("Usa uma amostra limpa, com uma única voz e sem música. Uma referência clara ajuda a preservar melhor a identidade vocal."),Vocloning::utf8("Depois de carregar a amostra, abre Texto, escolhe a voz, escreve a frase e seleciona Dizer na voz clonada.")};
 const juce::String body2[]={Vocloning::utf8("O Sopro V2 Turbo gera a fala localmente a partir da referência, em português europeu, sem serviço externo."),Vocloning::utf8("Confirma que tens autorização para utilizar a voz. Formatos aceites: WAV e MP3."),Vocloning::utf8("Podes ouvir novamente ou descarregar o WAV gerado. Se a voz não estiver correta, regressa à Referência e carrega uma amostra melhor.")};
 guideCardTitle.setText(titles[guideStep],juce::dontSendNotification);guideBody1.setText(body1[guideStep],juce::dontSendNotification);guideBody2.setText(body2[guideStep],juce::dontSendNotification);
 guideStep1.setColour(juce::TextButton::buttonColourId,guideStep==0?red:card);guideStep2.setColour(juce::TextButton::buttonColourId,guideStep==1?red:card);guideStep3.setColour(juce::TextButton::buttonColourId,guideStep==2?red:card);
 guidePrevious.setEnabled(guideStep>0);guideNext.setButtonText(guideStep==2?Vocloning::utf8("Concluir"):Vocloning::utf8("Seguinte"));guideNext.setEnabled(true);
 guideOpenReference.setVisible(currentPage==Page::Guide&&guideStep==1);guideOpenText.setVisible(currentPage==Page::Guide&&guideStep==2);repaint();
}
void MainComponent::importReference()
{
 juce::Component::SafePointer<MainComponent> safe(this);
 updateStatus(Vocloning::utf8("Escolhe um ficheiro WAV ou MP3 para continuar."));
 chooser.launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
     [safe](const juce::FileChooser& fc)
     {
         if (safe == nullptr) return;
         const auto file = fc.getResult();
         if (!file.existsAsFile()) { safe->updateStatus(Vocloning::utf8("Nenhum ficheiro foi selecionado.")); return; }
         if (!file.hasFileExtension("wav") && !file.hasFileExtension("mp3"))
         { safe->updateStatus(Vocloning::utf8("Formato inválido. Seleciona um ficheiro WAV ou MP3.")); return; }
         if (!safe->consent.getToggleState())
         { safe->updateStatus(Vocloning::utf8("Ficheiro selecionado. Confirma a autorização antes de guardar a referência.")); return; }
         auto name = safe->voiceName.getText().trim();
         if (name.isEmpty()) name = file.getFileNameWithoutExtension();
         juce::String error;
         if (safe->library.addVoice(name, file, error))
         {
             safe->refreshGrid(); safe->updateVoiceControls(); safe->showPage(Page::Text);
             safe->updateStatus(Vocloning::utf8("Referência adicionada à biblioteca."));
         }
         else safe->updateStatus(error);
     });
}
void MainComponent::refreshGrid(){if(voiceGrid)voiceGrid->repaint();}
void MainComponent::updateVoiceControls(){voiceSelector.clear(juce::dontSendNotification);const auto& v=library.getVoices();for(int i=0;i<v.size();++i)voiceSelector.addItem(v.getReference(i).name,i+1);if(!v.isEmpty()){selectedVoiceIndex=juce::jlimit(0,v.size()-1,selectedVoiceIndex);voiceSelector.setSelectedId(selectedVoiceIndex+1,juce::dontSendNotification);}updateActionState();}
void MainComponent::updateActionState(){bool has=juce::isPositiveAndBelow(voiceSelector.getSelectedId()-1,library.getVoices().size());
 const bool engineReady = modelSelector.getSelectedId()==1 ? SoproTTSBackend::isAvailable() : PocketTTSBackend::isRuntimeAvailable();
 generateButton.setEnabled(has&&engineReady&&!generationRunning.load());playButton.setEnabled(generatedAudio.existsAsFile());exportButton.setEnabled(generatedAudio.existsAsFile());}
void MainComponent::comboBoxChanged(juce::ComboBox* box)
{
 if (box == &modelSelector)
 {
     if (modelSelector.getSelectedId() == 1)
         engineInfo.setText(SoproTTSBackend::isAvailable()
             ? Vocloning::utf8("PREDEFINIDO: Sopro V2 Turbo\nPortuguês europeu (pt), clonagem zero-shot local, CPU, 24 kHz mono.")
             : Vocloning::utf8("Sopro não encontrado. Verifica C:\\AI\\Sopro-PT-PT\\audio.cpp e os ficheiros do modelo."), juce::dontSendNotification);
     else
         engineInfo.setText(PocketTTSBackend::isRuntimeAvailable()
             ? Vocloning::utf8("Pocket TTS 24L - motor alternativo instalado.")
             : Vocloning::utf8("Pocket TTS alternativo não está instalado."), juce::dontSendNotification);
 }
 updateActionState();
}
void MainComponent::textEditorTextChanged(juce::TextEditor& e){if(&e==&textToSpeak)charCounter.setText(juce::String(textToSpeak.getText().length())+Vocloning::utf8(" / 2000 - pt-PT"),juce::dontSendNotification);}
void MainComponent::generateSpeech()
{
 if(generationRunning.exchange(true))return;
 int row=voiceSelector.getSelectedId()-1;selectedVoiceIndex=row;
 if(!juce::isPositiveAndBelow(row,library.getVoices().size())){generationRunning=false;updateStatus(Vocloning::utf8("Seleciona uma voz válida."));return;}
 const int selectedEngine = modelSelector.getSelectedId();
 if(selectedEngine==1 && !SoproTTSBackend::isAvailable()){generationRunning=false;updateStatus(Vocloning::utf8("Sopro V2 Turbo não encontrado. Verifica a instalação do motor e do modelo."));return;}
 if(selectedEngine==2 && !PocketTTSBackend::isRuntimeAvailable()){generationRunning=false;updateStatus(Vocloning::utf8("Pocket TTS não encontrado na pasta Runtime."));return;}
 auto ref=library.getVoices().getReference(row).referenceFile;auto text=textToSpeak.getText();
 auto suffix = selectedEngine==1 ? "_sopro_pt.wav" : "_pocket_pt24l.wav";
 auto out=juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("VocloningNative").getChildFile("generated_"+juce::Uuid().toString()+suffix);
 updateStatus(selectedEngine==1?Vocloning::utf8("A gerar com Sopro V2 Turbo - português europeu…"):Vocloning::utf8("A gerar com Pocket TTS…"));
 generateButton.setEnabled(false);juce::Component::SafePointer<MainComponent> safe(this);
 std::thread([safe,ref,text,out,selectedEngine]()
 {
     struct UnifiedGenerationResult
      {
          bool success = false;
          juce::String message;
          juce::File outputFile;
      };

      UnifiedGenerationResult result;

      if (selectedEngine == 1)
      {
          const auto engineResult =
              SoproTTSBackend::generatePortugueseEuropean(ref, text, out);

          result.success = engineResult.success;
          result.message = engineResult.message;
          result.outputFile = engineResult.outputFile;
      }
      else
      {
          const auto engineResult =
              PocketTTSBackend::generatePortuguese24L(ref, text, out);

          result.success = engineResult.success;
          result.message = engineResult.message;
          result.outputFile = engineResult.outputFile;
      }

      juce::MessageManager::callAsync([safe, result]()
     {
         if (safe == nullptr) return;
         safe->generationRunning = false;
         if (result.success)
         {
             safe->generatedAudio = result.outputFile;
             safe->updateActionState();
             // Generation must be audible immediately; do not merely report success.
             safe->playAudioFile(result.outputFile);
         }
         else
         {
             safe->updateActionState();
             safe->updateStatus(result.message);
         }
     });
 }).detach();
}
void MainComponent::playReference(int i){if(juce::isPositiveAndBelow(i,library.getVoices().size()))playAudioFile(library.getVoices().getReference(i).referenceFile);}
void MainComponent::playAudioFile(const juce::File& file)
{
 if(!file.existsAsFile()){updateStatus(Vocloning::utf8("O ficheiro de áudio já não existe."));return;}
 if (!audioReady)
 {
     const auto deviceError = audioDeviceManager.initialise(0, 2, nullptr, true);
     if (deviceError.isNotEmpty())
     {
         updateStatus(Vocloning::utf8("Não foi possível inicializar o dispositivo de áudio: ") + deviceError);
         return;
     }
     auto* device = audioDeviceManager.getCurrentAudioDevice();
     if (device == nullptr || device->getActiveOutputChannels().countNumberOfSetBits() == 0)
     {
         updateStatus(Vocloning::utf8("Não existe uma saída de áudio ativa. Verifica as colunas, auscultadores e o dispositivo predefinido do Windows."));
         return;
     }
     audioSourcePlayer.setSource(&audioTransport);
     audioDeviceManager.addAudioCallback(&audioSourcePlayer);
     audioReady = true;
 }
 juce::AudioFormatManager formats;formats.registerBasicFormats();
 std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
 if(!reader){updateStatus(Vocloning::utf8("Não foi possível abrir este ficheiro de áudio."));return;}
 auto source=std::make_unique<juce::AudioFormatReaderSource>(reader.release(),true);
 audioTransport.stop();audioTransport.setSource(nullptr);
 currentAudioSource=std::move(source);
 audioTransport.setSource(currentAudioSource.get(),0,nullptr,currentAudioSource->getAudioFormatReader()->sampleRate);
 audioTransport.setPosition(0.0);
 audioTransport.start();
 if (!audioTransport.isPlaying())
 {
     updateStatus(Vocloning::utf8("O áudio foi aberto, mas a reprodução não arrancou. Verifica o dispositivo de saída."));
     return;
 }
 updateStatus(Vocloning::utf8("A reproduzir: ") + file.getFileName());
}
void MainComponent::exportGeneratedAudio()
{
 if(!generatedAudio.existsAsFile()){updateStatus(Vocloning::utf8("Ainda não existe áudio gerado."));return;}auto* fc=new juce::FileChooser(Vocloning::utf8("Descarregar WAV"),juce::File::getSpecialLocation(juce::File::userMusicDirectory).getChildFile("Vocloning.wav"),"*.wav");juce::Component::SafePointer<MainComponent> safe(this);
 fc->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles,[safe,fc](const juce::FileChooser& f){auto dest=f.getResult();if(safe!=nullptr&&dest!=juce::File())safe->updateStatus(safe->generatedAudio.copyFileTo(dest)?Vocloning::utf8("WAV descarregado."):Vocloning::utf8("Não foi possível guardar o WAV."));delete fc;});
}
void MainComponent::updateStatus(const juce::String& s){status.setText(s,juce::dontSendNotification);}
