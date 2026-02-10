//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../../DSP/Processors/EQ/LVPultecEQProcessor.h"
#include "../BaseEditor.h"
#include "../../Widgets/BaseSlider.h"

namespace viator::gui::editors
{
    class LVPultecEQEditor : public BaseEditor {
    public:
        explicit LVPultecEQEditor(dsp::processors::LVPultecEQProcessor &);

        ~LVPultecEQEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

        enum Sliders
        {
            kLowBoost = 0, kLowAtten, kHighBoost, kHighAtten,
            kLowFreq, kBandwidth, kHighFreq, kHighAttenSel,
            kDrive, kLP, kHP, num_sliders
        };

    private:
        dsp::processors::LVPultecEQProcessor &processorRef;

        void setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items);

        std::array<widgets::BaseSlider, num_sliders> m_main_sliders;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> > main_slider_attaches;

        void setSliderProps(juce::Slider &slider);

        void setLabelProps(juce::Label &label);

        viator::laf::DialLAF m_dial_laf;
        viator::laf::PultecHandleDialLAF m_rect_dial_laf;
        viator::gui::laf::MenuLAF m_menu_laf;

        const juce::Colour m_bg_color {47, 57, 70};
    };
}
