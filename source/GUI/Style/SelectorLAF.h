//
// Created by Landon Viator on 11/24/25.
//

#pragma once
#include "Fonts.h"

namespace viator
{
    class SelectorLAF final : public juce::LookAndFeel_V4
    {
    public:
        void drawComboBox(juce::Graphics &g, const int width, const int height, bool,
                          int, int, int, int, juce::ComboBox &box) override
        {
            const auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
            const juce::Rectangle<int> boxBounds = box.getLocalBounds().reduced(2.0f);

            auto baseColour = box.findColour(juce::ComboBox::backgroundColourId).withMultipliedSaturation(2.0f).
                    withMultipliedAlpha(1.0f);

            if (box.isMouseOverOrDragging())
                baseColour = Colors::light_bg().withAlpha(0.5f);

            g.setColour(baseColour);
            g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

            g.setColour(box.findColour(juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

            g.setColour(Colors::text());
            g.setFont(getComboBoxFont(box));
            const auto text = "Add Plugin Module";
            g.drawFittedText(text, 0, 0, width, height, juce::Justification::centred, 1);
        }

        juce::Font getComboBoxFont(juce::ComboBox &box) override
        {
            return Fonts::regular(16.0f);
        }

        void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override
        {
        }

        void drawPopupMenuBackground(juce::Graphics &g, [[maybe_unused]] int width, [[maybe_unused]] int height)
        override
        {
            g.fillAll(Colors::medium_bg());
        }

        juce::Font getPopupMenuFont() override
        {
            return Fonts::regular(20.0f);
        }

        juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox &box, juce::Label &label) override
        {
            auto opts = juce::PopupMenu::Options()
                    .withItemThatMustBeVisible(box.getSelectedId())
                    .withInitiallySelectedItem(box.getSelectedId())
                    .withMinimumWidth(box.getWidth())
                    .withMaximumNumColumns(1)
                    .withStandardItemHeight(label.getHeight());

            const auto b = box.getScreenBounds();
            const juce::Rectangle<int> anchor(1, 1, 1, 1); // tiny anchor rect
            opts = opts.withTargetScreenArea(anchor.withCentre(b.getCentre()));

            return opts;
        }
    };
}
