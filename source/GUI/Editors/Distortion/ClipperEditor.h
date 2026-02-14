//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../../DSP/Processors/Clipper/ClipperProcessor.h"
#include "../BaseEditor.h"
#include "../../Widgets/BaseSlider.h"

namespace viator
{
    class ClipperEditor : public viator::BaseEditor
    {
    public:
        explicit ClipperEditor(viator::ClipperProcessor &);

        ~ClipperEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

        enum ClipperType {
            kSoft = 0,
            kHard, kNumTypes
        };

    private:
        ClipperProcessor &processorRef;

        BaseSlider m_drive_slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_drive_attach;

        std::array<juce::TextButton, kNumTypes> m_type_buttons;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_type_attach;
        void setTypeButtonProps();

        viator::DialLAF m_dial_laf;
        viator::MenuLAF m_menu_laf;
        viator::InsetToggleLAF m_inset_toggle_laf;
    };
}