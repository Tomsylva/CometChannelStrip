//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../../DSP/Processors/EQ/LV60GraphicEQProcessor.h"
#include "../BaseEditor.h"
#include "../../Widgets/BaseSlider.h"

class SliderLAF : public juce::LookAndFeel_V4 {
public:
    SliderLAF(bool draw_top, bool draw_bottom)
    {
        m_draw_top_line = draw_top;
        m_draw_bottom_line = draw_bottom;
    }
    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height,
                                          float sliderPos,
                                          float minSliderPos,
                                          float maxSliderPos,
                                          const juce::Slider::SliderStyle style, juce::Slider &slider) override
    {
        auto line_x = static_cast<float>(slider.getLocalBounds().getWidth()) * 0.1f;
        const auto _height = static_cast<float>(slider.getLocalBounds().getHeight());
        const auto line_height = m_draw_bottom_line ? _height : _height / 2;
        const auto _y = m_draw_top_line ? 0 : _height / 2;

        g.setColour(viator::gui_utils::Colors::graphic_slider_blue());
        g.drawLine(line_x, _y, line_x, line_height, 2.0f);

        line_x = static_cast<float>(slider.getLocalBounds().getWidth()) * 0.3f;
        g.drawLine(line_x, _y, line_x, line_height, 2.0f);

        line_x = static_cast<float>(slider.getLocalBounds().getWidth()) * 0.5f;
        g.drawLine(line_x, _y, line_x, line_height, 2.0f);

        line_x = static_cast<float>(slider.getLocalBounds().getWidth()) * 0.7f;
        g.drawLine(line_x, _y, line_x, line_height, 2.0f);

        line_x = static_cast<float>(slider.getLocalBounds().getWidth()) * 0.9f;
        g.drawLine(line_x, _y, line_x, line_height, 2.0f);

        auto isThreeVal = (style == juce::Slider::SliderStyle::ThreeValueVertical || style == juce::Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = juce::jmin(6.0f, slider.isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);

        juce::Point<float> startPoint(slider.isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                                slider.isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

        juce::Point<float> endPoint(slider.isHorizontal() ? (float) (width + x) : startPoint.x,
                              slider.isHorizontal() ? startPoint.y : (float) y);

        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.strokePath(backgroundTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});

        juce::Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;


        auto kx = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

        minPoint = startPoint;
        maxPoint = {kx, ky};

        const auto thumbWidth = juce::roundToInt(width * 0.12f);

        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillRoundedRectangle (juce::Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth) / 2.0f).withCentre (maxPoint), 6.0f);
    }

private:
    bool m_draw_top_line {true};
    bool m_draw_bottom_line {true};
};

namespace viator::gui::editors
{
    class LV60GraphicEQEditor : public viator::gui::editors::BaseEditor {
    public:
        explicit LV60GraphicEQEditor(viator::dsp::processors::LV60GraphicEQProcessor &);

        ~LV60GraphicEQEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

        enum Sliders {
            kGain1 = 0, kGain2, kGain3, kGain4, kGain5,
            kGain6, kGain7, kGain8, kGain9, kGain10,
            kDrive, kLP, kHP, num_sliders
        };

    private:
        viator::dsp::processors::LV60GraphicEQProcessor &processorRef;

        void setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items);

        std::array<viator::gui::widgets::BaseSlider, Sliders::num_sliders> m_main_sliders;
        std::array<juce::Label, 10> m_gain_labels;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> > main_slider_attaches;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_hp_attach, m_lp_attach, m_drive_attach;

        void setSliderProps(juce::Slider &slider);

        void setLabelProps(juce::Label &label);

        void updateLabels();

        std::vector<juce::String> gain_labels =
        {
            "16 kHz", "8 kHz", "4 kHz", "2 kHz", "1 kHz", "500 Hz", "250 Hz", "125 Hz", "63 Hz", "31 Hz"
        };

        viator::laf::DialLAF m_dial_laf;
        viator::gui::laf::MenuLAF m_menu_laf;
        SliderLAF m_slider_laf {true, true};
        SliderLAF m_slider_laf_no_top {false, true};
        SliderLAF m_slider_laf_no_bottom {true, false};
    };
}
