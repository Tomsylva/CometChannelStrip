//
// Created by Landon Viator on 11/26/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Images.h"

namespace viator
{
    class ButtonLAF : public juce::LookAndFeel_V4 {
    public:
        void drawButtonBackground(juce::Graphics &g,
                                  juce::Button &button,
                                  const juce::Colour &backgroundColour,
                                  const bool shouldDrawButtonAsHighlighted,
                                  const bool shouldDrawButtonAsDown) override
        {
            constexpr auto cornerSize = 3.0f;
            const auto bounds = button.getLocalBounds().toFloat();

            auto baseColour = backgroundColour.withMultipliedSaturation(2.0f).withMultipliedAlpha(1.0f);

            if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
                baseColour = button.findColour(juce::TextButton::ColourIds::buttonOnColourId);

            g.setColour(baseColour);
            g.fillRoundedRectangle(bounds, cornerSize);

            g.setColour(button.findColour(juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
        }

        void drawButtonText(juce::Graphics &g,
                            juce::TextButton &button,
                            bool /*isMouseOverButton*/,
                            bool /*isButtonDown*/) override
        {
            const auto font = getTextButtonFont(button, button.getHeight());
            g.setFont(font);

            auto textCol = button.findColour(button.getToggleState()
                                                 ? juce::TextButton::textColourOnId
                                                 : juce::TextButton::textColourOffId);

            if (!button.isEnabled())
                textCol = textCol.withMultipliedAlpha(0.5f);

            auto r = button.getLocalBounds();

            r = r.translated(0, -1);

            g.setColour(textCol);
            g.drawFittedText(button.getButtonText(), r, juce::Justification::centred, 1);
        }

        juce::Font getTextButtonFont(juce::TextButton & /*button*/, const int buttonHeight) override
        {
            return {juce::FontOptions(static_cast<float>(buttonHeight) * 0.45f, juce::Font::bold)};
        }
    };

    class InsetToggleLAF final : public juce::LookAndFeel_V4 {
    public:
        void drawButtonBackground(juce::Graphics &g,
                                  juce::Button &button,
                                  const juce::Colour &,
                                  bool isOver,
                                  bool isDown) override
        {
            const auto r = button.getLocalBounds().toFloat().reduced(3.0f);
            constexpr float corner = 2.0f;
            const bool isOn = button.getToggleState();
            const auto base = button.findColour(juce::TextButton::buttonColourId);

            g.setColour(isOn ? base.darker(0.25f).withAlpha(0.5f) : base.withAlpha(0.5f));
            g.fillRoundedRectangle(r, corner);

            g.setColour(juce::Colours::black.withAlpha(0.25f));
            g.drawRoundedRectangle(r, corner, 1.0f);

            // light bevel just under the top outline
            {
                g.saveState();

                constexpr float bevelH = 1.0f;
                g.reduceClipRegion(r.withHeight(bevelH).toNearestInt());

                g.setColour(isOn ? juce::Colours::black.withAlpha(0.25f) : juce::Colours::white.withAlpha(0.18f));
                g.drawRoundedRectangle(r.translated(0.0f, 1.0f), corner, 1.0f);

                g.restoreState();
            }

            // bottom bevel BELOW the outline (outside)
            {
                g.saveState();

                constexpr float bevelH = 1.0f;
                constexpr float yOffset = 0.0f;

                const auto band = juce::Rectangle<float>(r.getX(),
                                                   r.getBottom() + yOffset,
                                                   r.getWidth(),
                                                   bevelH);

                g.reduceClipRegion(band.toNearestInt());

                g.setColour(juce::Colours::white.withAlpha(0.14f)); // tweak
                g.drawRoundedRectangle(r.translated(0.0f, yOffset), corner, 1.0f);

                g.restoreState();
            }
        }

        void drawButtonText(juce::Graphics &g,
                    juce::TextButton &button,
                    bool /*isMouseOverButton*/,
                    bool /*isButtonDown*/) override
        {
        }

        juce::Font getTextButtonFont(juce::TextButton & /*button*/, const int buttonHeight) override
        {
            return {juce::FontOptions(static_cast<float>(buttonHeight) * 0.45f, juce::Font::bold)};
        }
    };
}
