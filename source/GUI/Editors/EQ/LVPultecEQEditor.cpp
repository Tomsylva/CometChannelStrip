//
// Created by Landon Viator on 11/14/25.
//

#include "LVPultecEQEditor.h"

namespace viator
{
    LVPultecEQEditor::LVPultecEQEditor(viator::LVPultecEQProcessor &p)
        : viator::BaseEditor(p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);
        const auto id = juce::String(processorRef.getProcessorID());

        const juce::StringArray ids = {
            LVPultecEQParameters::lowBoostID, LVPultecEQParameters::lowAttenID, LVPultecEQParameters::highBoostID, LVPultecEQParameters::highAttenID,
            LVPultecEQParameters::lowFreqID, LVPultecEQParameters::bandwidthID, LVPultecEQParameters::highFreqID, LVPultecEQParameters::highAttenSelID,
            LVPultecEQParameters::driveID, LVPultecEQParameters::lpCutoffID, LVPultecEQParameters::hpCutoffID
        };

        const juce::StringArray names = {
            LVPultecEQParameters::lowBoostName, LVPultecEQParameters::lowAttenName, LVPultecEQParameters::highBoostName,
            LVPultecEQParameters::highAttenName, LVPultecEQParameters::lowFreqName, LVPultecEQParameters::bandwidthName,
            LVPultecEQParameters::highFreqName, LVPultecEQParameters::highAttenSelName,
            LVPultecEQParameters::driveName, LVPultecEQParameters::lpCutoffName, LVPultecEQParameters::hpCutoffName
        };

        for (int i = 0; i < num_sliders; ++i)
        {
            LVPultecEQEditor::setDialProps(m_main_sliders[i]);
            m_main_sliders[i].setComponentID(ids[i] + id);
            m_main_sliders[i].setName(names[i]);
            getSliders().push_back(&m_main_sliders[i]);

            main_slider_attaches.emplace_back(
                std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getTreeState(), ids[i] + id, m_main_sliders[i]));
        }

        m_main_sliders[kLowBoost].setTextValueSuffix(" dB");
        m_main_sliders[kHighBoost].setTextValueSuffix(" dB");
        m_main_sliders[kLowAtten].setTextValueSuffix(" dB");
        m_main_sliders[kHighAtten].setTextValueSuffix(" dB");
        m_main_sliders[kLowFreq].setTextValueSuffix(" Hz");
        m_main_sliders[kHighFreq].setTextValueSuffix(" Hz");
        m_main_sliders[kHighAttenSel].setTextValueSuffix(" Hz");
        m_main_sliders[kBandwidth].setTextValueSuffix(" Q");
        m_main_sliders[kHP].setTextValueSuffix(" Hz");
        m_main_sliders[kDrive].setTextValueSuffix(" dB");
        m_main_sliders[kLP].setTextValueSuffix(" Hz");

        m_main_sliders[kLowFreq].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, Colors::dial_bg());
        m_main_sliders[kHighFreq].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, Colors::dial_bg());
        m_main_sliders[kHighAttenSel].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, Colors::dial_bg());

        m_main_sliders[kLowFreq].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::pultec_dial());
        m_main_sliders[kHighFreq].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::pultec_dial());
        m_main_sliders[kHighAttenSel].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::pultec_dial());

        m_main_sliders[kHP].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::eq_footer_dials());
        m_main_sliders[kDrive].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::eq_footer_dials());
        m_main_sliders[kLP].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::eq_footer_dials());

        m_main_sliders[kLowFreq].setLookAndFeel(&m_rect_dial_laf);
        m_main_sliders[kHighFreq].setLookAndFeel(&m_rect_dial_laf);
        m_main_sliders[kHighAttenSel].setLookAndFeel(&m_rect_dial_laf);

        setSize(1000, 600);
    }

    LVPultecEQEditor::~LVPultecEQEditor()
    {
        for (auto &slider: m_main_sliders)
        {
            slider.setLookAndFeel(nullptr);
        }
    }

    //==============================================================================
    void LVPultecEQEditor::paint(juce::Graphics &g)
    {
        setBackgroundColor(m_bg_color);
        BaseEditor::paint(g);

        const auto text = "Program Tube \nEqualizer";
        const auto x = juce::roundToInt(getWidth() * 0.58);
        const auto y = juce::roundToInt(getHeight() * 0.325);
        const auto font_size = static_cast<float>(getHeight()) * 0.027f;
        g.setColour(juce::Colour(120, 185, 181));
        g.setFont(Fonts::bold(font_size));
        g.drawFittedText(text, x, y, getWidth() / 2, getHeight() / 10, juce::Justification::centred, 2);
    }

    void LVPultecEQEditor::resized()
    {
        auto width = juce::roundToInt(getWidth() * 0.34);
        auto x = 0;
        auto y = juce::roundToInt(getHeight() * 0.07);

        m_main_sliders[kLowAtten].setBounds(x, y, width, width);
        y += width;
        m_main_sliders[kLowBoost].setBounds(x, y, width, width);
        y += width;
        m_main_sliders[kLowFreq].setBounds(x, y, width, width);
        y = juce::roundToInt(getHeight() * 0.07);
        x += width;
        m_main_sliders[kHighAtten].setBounds(x, y, width, width);
        y += width;
        m_main_sliders[kHighBoost].setBounds(x, y, width, width);
        y += width;
        m_main_sliders[kHighFreq].setBounds(x, y, width, width);
        y = juce::roundToInt(getHeight() * 0.07);
        x += juce::roundToInt(width * 0.9);
        m_main_sliders[kHighAttenSel].setBounds(x, y, width, width);
        y += width * 2;
        m_main_sliders[kBandwidth].setBounds(x, y, width, width);

        width = juce::roundToInt(getWidth() * 0.335);
        y = juce::roundToInt(getHeight() * 0.7);
        x = 0;
        const auto filter_size = juce::roundToInt(width * 0.85);
        m_main_sliders[kHP].setBounds(x, y, filter_size, filter_size);
        x += width;
        m_main_sliders[kDrive].setBounds(x, y, width, width);
        x += width + (width - filter_size);
        m_main_sliders[kLP].setBounds(x, y, filter_size, filter_size);

        BaseEditor::resized();
    }

    void LVPultecEQEditor::setDialProps(juce::Slider &slider)
    {
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(215, 215, 215).withAlpha(0.85f));
        slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(211, 218, 217));
        slider.setColour(juce::Slider::ColourIds::backgroundColourId, Colors::pultec_dial());
        slider.setLookAndFeel(&m_dial_laf);

        addAndMakeVisible(slider);
    }

    void LVPultecEQEditor::setLabelProps(juce::Label &label)
    {
        label.setJustificationType(juce::Justification::centred);
        //label.setColour(juce::Label::outlineColourId, juce::Colours::white);
        addAndMakeVisible(label);
    }

    void LVPultecEQEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
    {
        box.addItemList(items, 1);
        box.setSelectedId(1, juce::dontSendNotification);
        box.setLookAndFeel(&m_menu_laf);
        box.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, viator::Colors::editor_minor_bg_color());
        box.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::backgroundColourId,
                                       viator::Colors::editor_minor_bg_color());
        addAndMakeVisible(box);
    }
}
