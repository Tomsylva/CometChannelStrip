//
// Created by Landon Viator on 11/21/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Globals/Globals.h"

namespace viator
{
    class MacroSlider final : public juce::Slider, public juce::ActionBroadcaster
    {
    public:
        MacroSlider();
        ~MacroSlider() override;

        using MacroState = globals::MacroLearnState;

        void paint(juce::Graphics &g) override;

        void toggleMacroState();
        void enableMacroState(bool shouldBeOn);
        MacroState getMacroState() const;

        enum class MacroLearnState
        {
            kOff,
            kOn
        };

    private:
        MacroState m_macro_learn_state{MacroState::kOff};

        void mouseDown(const juce::MouseEvent &event) override;
    };
}

