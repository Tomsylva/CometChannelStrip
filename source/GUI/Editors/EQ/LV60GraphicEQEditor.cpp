//
// Created by Landon Viator on 11/14/25.
//

#include "LV60GraphicEQEditor.h"

namespace viator
{
    LV60GraphicEQEditor::LV60GraphicEQEditor(viator::LV60GraphicEQProcessor &p)
            : viator::BaseEditor(p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);
        const auto id = juce::String(processorRef.getProcessorID());

        for (int i = 0; i < m_main_sliders.size(); ++i)
        {
            setSliderProps(m_main_sliders[i]);
            getSliders().push_back(&m_main_sliders[i]);
        }

        m_main_sliders[kGain1].setLookAndFeel(&m_slider_laf_no_top);
        m_main_sliders[kGain10].setLookAndFeel(&m_slider_laf_no_bottom);

        m_main_sliders[kGain5].setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(220, 60, 40));

        for (int i = 0; i < m_gain_labels.size(); i++)
        {
            setLabelProps(m_gain_labels[i]);
            m_gain_labels[i].setText(gain_labels[i], juce::dontSendNotification);
        }

        for (int i = 0; i < LV60GraphicEQParameters::numBands; ++i)
        {
            main_slider_attaches.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::gainIDs[i] + id, m_main_sliders[i]));
        }

        m_hp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::hpCutoffID + id, m_main_sliders[kHP]);
        m_lp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::lpCutoffID + id, m_main_sliders[kLP]);
        m_drive_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                        (processorRef.getTreeState(), LV60GraphicEQParameters::driveID + id, m_main_sliders[kDrive]);

        for (int i = 0; i < LV60GraphicEQParameters::gainIDs.size(); ++i)
        {
            m_main_sliders[i].setComponentID(LV60GraphicEQParameters::gainIDs[i] + id);
        }
        m_main_sliders[kHP].setComponentID(LV60GraphicEQParameters::hpCutoffID + id);
        m_main_sliders[kLP].setComponentID(LV60GraphicEQParameters::lpCutoffID + id);
        m_main_sliders[kDrive].setComponentID(LV60GraphicEQParameters::driveID + id);
        m_main_sliders[kHP].setName("HP");
        m_main_sliders[kLP].setName("LP");
        m_main_sliders[kDrive].setName("Drive");

        m_main_sliders[kLP].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::eq_footer_dials());
        m_main_sliders[kHP].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::eq_footer_dials());
        m_main_sliders[kDrive].setColour(juce::Slider::ColourIds::backgroundColourId, Colors::eq_footer_dials());
    }

    LV60GraphicEQEditor::~LV60GraphicEQEditor()
    {
        for (auto& slider : m_main_sliders)
        {
            slider.setLookAndFeel(nullptr);
        }
    }

//==============================================================================
    void LV60GraphicEQEditor::paint(juce::Graphics &g)
    {
        setBackgroundColor(juce::Colour(34, 40, 49));
        BaseEditor::paint(g);

        const auto text = "LV60 Graphic EQ";
        constexpr auto x = 2;
        const auto y = juce::roundToInt(getHeight() * 0.84);
        const auto font_size = static_cast<float>(getHeight()) * 0.027f;
        g.setColour(Colors::graphic_slider_blue());
        g.setFont(Fonts::bold(font_size));
        g.drawFittedText(text, x, y, getWidth() / 2, getHeight() / 10, juce::Justification::centredLeft, {});
    }

    void LV60GraphicEQEditor::resized()
    {
        auto y = juce::roundToInt(getHeight() * 0.07);
        auto width = juce::roundToInt(getWidth() * 0.6);
        const auto label_width = juce::roundToInt(getWidth() * 0.3);
        const auto height = juce::roundToInt(getHeight() * 0.063);
        const auto font_size = static_cast<float>(getWidth()) * 0.05f;

        for (int i = 0; i < m_gain_labels.size(); ++i)
        {
            m_gain_labels[i].setBounds(0, y, label_width, height);
            m_gain_labels[i].setFont(viator::Fonts::regular(font_size));
            m_main_sliders[i].setBounds(m_gain_labels[i].getRight(), y, width, height);
            y += height;
        }

        width = juce::roundToInt(getWidth() * 0.335);
        y = juce::roundToInt(getHeight() * 0.7);
        auto x = 0;
        const auto filter_size = juce::roundToInt(width * 0.85);
        m_main_sliders[kHP].setBounds(x, y, filter_size, filter_size);
        x += width;
        m_main_sliders[kDrive].setBounds(x, y, width, width);
        x += width + (width - filter_size);
        m_main_sliders[kLP].setBounds(x, y, filter_size, filter_size);

        BaseEditor::resized();
    }

    void LV60GraphicEQEditor::setSliderProps(juce::Slider &slider)
    {
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        if (&slider != &m_main_sliders[kDrive] &&
            &slider != &m_main_sliders[kHP] &&
            &slider != &m_main_sliders[kLP])
        {
            slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
            slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::black);
            slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::black);
            slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(240, 220, 200));
            slider.setLookAndFeel(&m_slider_laf);
        } else
        {
            slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
            slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(157, 178, 191));
            slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(215, 215, 215).withAlpha(0.85f));
            slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(5, 120, 190));
            slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
            slider.setLookAndFeel(&m_dial_laf);
        }

        addAndMakeVisible(slider);
    }

    void LV60GraphicEQEditor::setLabelProps(juce::Label &label)
    {
        label.setJustificationType(juce::Justification::centred);
        //label.setColour(juce::Label::outlineColourId, juce::Colours::white);
        addAndMakeVisible(label);
    }

    void LV60GraphicEQEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
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