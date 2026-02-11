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

    private:
        ClipperProcessor &processorRef;

        BaseSlider m_drive_slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_drive_attach;

        juce::ComboBox m_clipper_type_menu;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_clipper_type_attach;
        void setComboBoxProps(juce::ComboBox& box, const juce::StringArray& items);

        viator::DialLAF m_dial_laf;
        viator::MenuLAF m_menu_laf;
    };
}