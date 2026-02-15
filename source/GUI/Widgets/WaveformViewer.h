#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "BinaryData.h"
#include "../utils/FilmStripImage.h"

namespace viator::ui
{
    class WaveformViewer : public juce::Component,
                           private juce::Timer
    {
    public:
        WaveformViewer()
        {
            startTimerHz(30);
        }

        void paint(juce::Graphics& g) override
        {
            // BG
            const auto bg_image = juce::ImageCache::getFromMemory(BinaryData::BPM_screen_body_png, BinaryData::BPM_screen_body_pngSize);
            g.drawImage(bg_image, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);

            // WAVE
            const auto width = static_cast<float>(getWidth());
            const auto height = static_cast<float>(getHeight());

            const float amplitude = height;
            const float midHeight = height / 2.0f;

            const float frequencyFactor = 2.0f * juce::MathConstants<float>::pi * cutoff * 0.008f / width;

            juce::Path sinePath;
            sinePath.startNewSubPath(-1, midHeight);

            for (int x = 0; x < getWidth(); ++x)
            {
                const float normalizedX = static_cast<float>(x) * frequencyFactor;
                const float sineValue = std::sin(normalizedX);

                // Process and then scale + offset vertically
                float y = processSample(sineValue);
                y = midHeight - (y * amplitude) / 2; // Scale and center

                sinePath.lineTo(static_cast<float>(x), y);
            }

            g.setColour(wave_color);
            g.strokePath(sinePath, juce::PathStrokeType(3.0f));

            // GLOSS
            const auto glass_image = juce::ImageCache::getFromMemory(BinaryData::BPM_screen_gloss_png, BinaryData::BPM_screen_gloss_pngSize);
            g.drawImage(glass_image, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
        }

        void setFrequency(const float newCutoff)
        {
            cutoff = newCutoff;
            repaint();
        }

        void setWaveColor(const juce::Colour newColor)
        {
            wave_color = newColor;
            repaint();
        }

    public:

        using SampleProcessor = std::function<float(float)>;

        void setSampleProcessor(SampleProcessor processor)
        {
            sampleProcessor = std::move(processor);
        }

        float processSample(float input)
        {
            if (sampleProcessor)
                return sampleProcessor(input);

            return input;
        }

    private:
        SampleProcessor sampleProcessor;

        void timerCallback() override
        {
            repaint();
        }

        float cutoff {440.0f};
        juce::Colour wave_color = juce::Colours::green;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformViewer)
    };
}
