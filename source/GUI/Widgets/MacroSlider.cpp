//
// Created by Landon Viator on 11/21/25.
//

#include "MacroSlider.h"

namespace viator
{
    MacroSlider::MacroSlider()
    = default;

    MacroSlider::~MacroSlider()
    = default;

    void MacroSlider::paint(juce::Graphics &g)
    {
        juce::Slider::paint(g);

        g.setColour(juce::Colours::purple.withAlpha(m_macro_learn_state == MacroState::kOn ? 0.25f : 0.0f));
        g.fillRect(getLocalBounds());
    }

    void MacroSlider::mouseDown(const juce::MouseEvent &event)
    {
        if (event.mods.isRightButtonDown())
        {
            sendActionMessage(viator::globals::ActionCommands::macroToggled);
        }

        juce::Slider::mouseDown(event);
    }

    void MacroSlider::toggleMacroState()
    {
        m_macro_learn_state = static_cast<MacroState>(!static_cast<bool>(m_macro_learn_state));
        repaint();
    }

    void MacroSlider::enableMacroState(const bool shouldBeOn)
    {
        m_macro_learn_state = static_cast<MacroState>(shouldBeOn);
        repaint();
    }

    MacroSlider::MacroState MacroSlider::getMacroState() const
    {
        return m_macro_learn_state;
    }
}