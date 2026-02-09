//
// Created by Landon Viator on 11/14/25.
//

#include "LVPultecEQEditor.h"

namespace viator::gui::editors
{
    LVPultecEQEditor::LVPultecEQEditor(viator::dsp::processors::LVPultecEQProcessor &p)
        : viator::gui::editors::BaseEditor(p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);
        const auto id = juce::String(processorRef.getProcessorID());

        const juce::StringArray ids = {
            LVPultecEQParameters::lowBoostID, LVPultecEQParameters::lowAttenID, LVPultecEQParameters::highBoostID, LVPultecEQParameters::highAttenID,
            LVPultecEQParameters::lowFreqID, LVPultecEQParameters::bandwidthID, LVPultecEQParameters::highFreqID, LVPultecEQParameters::highAttenSelID
        };

        const juce::StringArray names = {
            LVPultecEQParameters::lowBoostName, LVPultecEQParameters::lowAttenName, LVPultecEQParameters::highBoostName,
            LVPultecEQParameters::highAttenName,
            LVPultecEQParameters::lowFreqName, LVPultecEQParameters::bandwidthName, LVPultecEQParameters::highFreqName,
            LVPultecEQParameters::highAttenSelName
        };

        for (int i = 0; i < num_sliders; ++i)
        {
            LVPultecEQEditor::setSliderProps(m_main_sliders[i]);
            getSliders().push_back(&m_main_sliders[i]);
            m_main_sliders[i].setComponentID(ids[i]);
            m_main_sliders[i].setName(names[i]);

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

        m_main_sliders[kLowFreq].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, gui_utils::Colors::dial_bg());
        m_main_sliders[kHighFreq].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, gui_utils::Colors::dial_bg());
        m_main_sliders[kHighAttenSel].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, gui_utils::Colors::dial_bg());

        m_main_sliders[kLowFreq].setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(57, 62, 70));
        m_main_sliders[kHighFreq].setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(57, 62, 70));
        m_main_sliders[kHighAttenSel].setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(57, 62, 70));

        m_main_sliders[kLowFreq].setLookAndFeel(&m_rect_dial_laf);
        m_main_sliders[kHighFreq].setLookAndFeel(&m_rect_dial_laf);
        m_main_sliders[kHighAttenSel].setLookAndFeel(&m_rect_dial_laf);

        m_tube_button.setButtonText("Tube");
        m_tube_button.setClickingTogglesState(true);
        m_tube_button.setColour(juce::ComboBox::ColourIds::outlineColourId,
                         juce::Colours::transparentBlack);
        m_tube_button.setColour(juce::TextButton::ColourIds::buttonColourId, m_bg_color);
        m_tube_button.setColour(juce::TextButton::ColourIds::buttonOnColourId, gui_utils::Colors::light_bg().withAlpha(0.5f));
        m_tube_button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour(122, 126, 130));
        m_tube_button.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour(128, 195, 255));
        m_tube_button.setLookAndFeel(&m_button_laf);
        addAndMakeVisible(m_tube_button);

        m_tube_attach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getTreeState(), LVPultecEQParameters::tubeButtonID + id, m_tube_button);

        setSize(1000, 600);
    }

    LVPultecEQEditor::~LVPultecEQEditor()
    {
        for (auto &slider: m_main_sliders)
        {
            slider.setLookAndFeel(nullptr);
        }

        m_tube_button.setLookAndFeel(nullptr);
    }

    //==============================================================================
    void LVPultecEQEditor::paint(juce::Graphics &g)
    {
        setBackgroundColor(m_bg_color);
        BaseEditor::paint(g);

        const auto text = "Program Tube Equalizer";
        const auto x = juce::roundToInt(getWidth() * 0.45);
        const auto y = juce::roundToInt(getHeight() * 0.84);
        g.setColour(juce::Colour(128, 195, 255));
        g.setFont(gui_utils::Fonts::bold(12.0f));
        g.drawFittedText(text, x, y, getWidth() / 2, getHeight() / 10, juce::Justification::centredRight, {});
    }

    void LVPultecEQEditor::resized()
    {
        auto width = juce::roundToInt(getWidth() * 0.34);
        auto x = juce::roundToInt(getWidth() * 0.05);
        auto y = getHeight() - juce::roundToInt(width * 1.5);

        for (int i = 0; i < 4; ++i)
        {
            m_main_sliders[i].setBounds(x, y, width, width);
            y -= width;
        }

        y = juce::roundToInt(getHeight() * 0.62);
        x = getWidth() - x - width;
        m_main_sliders[kLowFreq].setBounds(x, y, width, width);
        x = juce::roundToInt(getWidth() * 0.41);
        y = juce::roundToInt(getHeight() * 0.45);
        m_main_sliders[kBandwidth].setBounds(x, y, width, width);
        y = juce::roundToInt(getHeight() * 0.25);
        m_main_sliders[kHighFreq].setBounds(m_main_sliders[kLowFreq].getX(), y, width, width);
        x = juce::roundToInt(getWidth() * 0.405);
        y = m_main_sliders[kHighAtten].getY();
        m_main_sliders[kHighAttenSel].setBounds(x, y, width, width);

        width = juce::roundToInt(getWidth() * 0.2);
        x = getWidth() - x / 2;
        y = juce::roundToInt(getHeight() * 0.1);
        m_tube_button.setBounds(getLocalBounds().withSizeKeepingCentre(width, width / 2).withY(y).withX(x));

        BaseEditor::resized();
    }

    void LVPultecEQEditor::setSliderProps(juce::Slider &slider)
    {
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(215, 215, 215).withAlpha(0.85f));
        slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(211, 218, 217));
        slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(68, 68, 78));
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
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, viator::gui_utils::Colors::editor_minor_bg_color());
        box.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::backgroundColourId,
                                       viator::gui_utils::Colors::editor_minor_bg_color());
        addAndMakeVisible(box);
    }
}
