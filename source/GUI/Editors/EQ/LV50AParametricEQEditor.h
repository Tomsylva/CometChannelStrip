//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../../DSP/Processors/EQ/LV50AParametricEQProcessor.h"
#include "../BaseEditor.h"
#include "../../Widgets/BaseSlider.h"
#include "../../Style/ButtonLAF.h"

namespace viator
{
    class LV50AParametricEQEditor : public viator::BaseEditor {
    public:
        explicit LV50AParametricEQEditor(viator::LV50AParametricEQProcessor &);

        ~LV50AParametricEQEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

        enum Sliders
        {
            kGain1 = 0, kGain2, kGain3, kGain4,
            kQ1, kQ2, kQ3, kQ4,
            kCutoff1, kCutoff2, kCutoff3, kCutoff4,
            kDrive, kLP, kHP,
            num_sliders
        };

        enum Buttons {
            kLowBell = 0, kHighBell, kNumButtons
        };

    private:
        viator::LV50AParametricEQProcessor &processorRef;

        void setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items);

        std::array<BaseSlider, Sliders::num_sliders> m_main_sliders;
        std::array<juce::Label, Sliders::num_sliders> m_main_labels;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> > main_slider_attaches;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_hp_attach, m_lp_attach, m_drive_attach;

        void setSliderProps(juce::Slider &slider);

        std::array<juce::TextButton, kNumButtons> m_bell_buttons;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_low_bell_attach, m_high_bell_attach;
        void setBellButtonProps();

        viator::DialLAF m_dial_laf;
        viator::MenuLAF m_menu_laf;
        viator::InsetToggleLAF m_inset_toggle_laf;
    };
}
