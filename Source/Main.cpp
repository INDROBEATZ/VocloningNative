#include <JuceHeader.h>
#include "MainComponent.h"
#include <BinaryData.h>

#if JUCE_WINDOWS
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
#endif

namespace
{
    const juce::Colour splashBg (0xff08090c);
    const juce::Colour splashRed (0xffff3347);
    const juce::Colour splashWhite (0xfff7f7fa);
    const juce::Colour splashMuted (0xffd0d2d8);

    class SplashComponent final : public juce::Component, private juce::Timer
    {
    public:
        explicit SplashComponent(std::function<void()> finished) : onFinished(std::move(finished))
        {
            setSize(620, 360);
            startTimerHz(60);
        }

        void paint(juce::Graphics& g) override
        {
            g.fillAll(splashBg);
            auto area = getLocalBounds().toFloat();
            juce::ColourGradient haze(splashRed.withAlpha(0.13f), area.getCentreX(), 95.0f,
                                      splashBg.withAlpha(0.0f), area.getRight(), area.getBottom(), true);
            g.setGradientFill(haze);
            g.fillEllipse(95.0f, -110.0f, 430.0f, 390.0f);

            // Embedded VOC LONING brand mark. The image is compiled into the native executable.
            if (auto logo = juce::ImageCache::getFromMemory(BinaryData::VocloningLogo_png, BinaryData::VocloningLogo_pngSize); logo.isValid())
                g.drawImageWithin(logo, 70, 8, 480, 270, juce::RectanglePlacement::centred);

            g.setColour(splashMuted);
            g.setFont(juce::Font(juce::FontOptions(11.5f, juce::Font::bold).withName("Bahnschrift")));
            g.drawText("PT-PT   /   VOICE STUDIO   /   NATIVE JUCE", 0, 265, getWidth(), 20, juce::Justification::centred);

            const float barW = 250.0f;
            const float barX = (area.getWidth() - barW) * 0.5f;
            const float barY = 305.0f;
            g.setColour(juce::Colour(0xff20262d));
            g.fillRoundedRectangle(barX, barY, barW, 3.0f, 1.5f);
            g.setColour(splashRed);
            g.fillRoundedRectangle(barX, barY, barW * progress, 3.0f, 1.5f);
            g.setColour(splashMuted);
            g.setFont(juce::Font(juce::FontOptions(10.5f)));
            g.drawText("A INICIALIZAR O ESTÚDIO DE VOZ…", 0, 318, getWidth(), 20, juce::Justification::centred);
        }

    private:
        void timerCallback() override
        {
            elapsed += 16;
            // Hold long enough to show the complete brand animation, then fade
            // the splash fully away before creating the main application window.
            progress = juce::jmin(1.0f, (float) elapsed / 1850.0f);
            if (elapsed >= 1850)
            {
                const float fadeProgress = juce::jlimit(0.0f, 1.0f,
                    (float) (elapsed - 1850) / 520.0f);
                setAlpha(1.0f - fadeProgress);
                if (fadeProgress >= 1.0f)
                {
                    stopTimer();
                    if (onFinished) onFinished();
                    return;
                }
            }
            repaint();
        }

        std::function<void()> onFinished;
        float progress = 0.0f;
        int elapsed = 0;
    };
}

class VocloningApplication final : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Vocloning"; }
    const juce::String getApplicationVersion() override { return "0.2.9.24"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override
    {
        splashWindow = std::make_unique<juce::DocumentWindow>("Vocloning",
            splashBg, 0);
        splashWindow->setUsingNativeTitleBar(false);
        splashWindow->setResizable(false, false);
        splashWindow->setContentOwned(new SplashComponent([this]
        {
            juce::MessageManager::callAsync([this]
            {
                mainWindow = std::make_unique<MainWindow>(getApplicationName());
                splashWindow.reset();
            });
        }), true);
        splashWindow->centreWithSize(620, 360);
        splashWindow->setAlpha(1.0f);
        splashWindow->setVisible(true);
    }

    void shutdown() override
    {
        mainWindow.reset();
        splashWindow.reset();
    }

    class MainWindow final : public juce::DocumentWindow
    {
    public:
        explicit MainWindow(const juce::String& name)
            : DocumentWindow(name, splashBg, juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            setResizable(true, true);
            setResizeLimits(1120, 700, 0, 0);
            centreWithSize(1440, 940);
            setVisible(true);
            // Request a real native Windows maximised state after the peer exists.
           #if JUCE_WINDOWS
            if (auto* peer = getPeer())
                if (auto hwnd = static_cast<HWND>(peer->getNativeHandle()))
                    ShowWindow(hwnd, SW_MAXIMIZE);
           #else
            if (auto* display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay())
                setBounds(display->userArea);
           #endif
        }
        void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }
    };

private:
    std::unique_ptr<juce::DocumentWindow> splashWindow;
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(VocloningApplication)
