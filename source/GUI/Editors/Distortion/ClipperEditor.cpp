//
// Created by Landon Viator on 11/14/25.
//

#include "../Distortion/ClipperEditor.h"

namespace viator
{
    ClipperEditor::ClipperEditor(viator::ClipperProcessor &p)
        : viator::BaseEditor(p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);

        m_drive_slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        m_drive_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        m_drive_slider.addMouseListener(this, true);
        m_drive_slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId,
                                 juce::Colours::transparentBlack);
        m_drive_slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(214, 189, 152));

        m_drive_slider.setComponentID(
            ClipperParameters::driveID + juce::String(processorRef.getProcessorID()));
        getSliders().push_back(&m_drive_slider);
        m_drive_slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::whitesmoke);
        m_drive_slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(190, 49, 68));
        m_drive_slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);
        m_drive_slider.setLookAndFeel(&m_dial_laf);
        addAndMakeVisible(m_drive_slider);

        m_drive_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef
            .getTreeState(),
            ClipperParameters::driveID +
            juce::String(
                processorRef.getProcessorID()),
            m_drive_slider);

        setTypeButtonProps();

        setSize(1000, 600);
    }

    ClipperEditor::~ClipperEditor()
    {
        m_drive_slider.setLookAndFeel(nullptr);

        for (auto &button: m_type_buttons)
        {
            button.setLookAndFeel(nullptr);
        }
    }

    //==============================================================================
    void ClipperEditor::paint(juce::Graphics &g)
    {
        setBackgroundColor(juce::Colour(64, 83, 76));
        BaseEditor::paint(g);
    }

    void ClipperEditor::resized()
    {
        const auto dial_x = juce::roundToInt(getWidth() * 0.015);
        const auto dial_y = juce::roundToInt(getHeight() * 0.63);
        const auto dial_size = juce::roundToInt(getHeight() * 0.3);
        m_drive_slider.setBounds(dial_x, dial_y, dial_size, dial_size);

        const auto button_x = juce::roundToInt(getWidth() * 0.61);
        auto button_y = juce::roundToInt(getHeight() * 0.725);
        const auto button_width = juce::roundToInt(getWidth() * 0.2);
        const auto button_height = button_width / 2;
        const auto padding = button_height / 10;
        for (auto &button: m_type_buttons)
        {
            button.setBounds(button_x, button_y, button_width, button_height);
            button_y += button_height + padding;
        }

        BaseEditor::resized();
    }

    void ClipperEditor::setTypeButtonProps()
    {
        for (auto &button: m_type_buttons)
        {
            button.setClickingTogglesState(true);
            button.setRadioGroupId(1000);
            button.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::whitesmoke);
            button.setColour(juce::TextButton::ColourIds::textColourOffId, Colors::light_bg());
            button.setLookAndFeel(&m_inset_toggle_laf);
            addAndMakeVisible(button);
        }

        m_type_buttons[kSoft].setButtonText("Soft");
        m_type_buttons[kHard].setButtonText("Hard");

        m_type_attach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getTreeState(),
                                                                                               ClipperParameters::clipTypeID +
                                                                                               juce::String(
                                                                                                   processorRef.getProcessorID()),
                                                                                               m_type_buttons[kSoft]);
    }
}
