//
// Created by Landon Viator on 11/14/25.
//

#include "BaseEditor.h"

namespace viator
{
    BaseEditor::BaseEditor(viator::BaseProcessor &p)
        : AudioProcessorEditor(&p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);

        const auto shadow_color = juce::Colours::black;
        auto shadow = juce::DropShadow(shadow_color, 10, {0, 4});

        m_drop_shadow = std::make_unique<juce::DropShadower>(shadow);
        m_drop_shadow->setOwner(this);

        // SLIDERS
        for (auto &slider: m_io_sliders) {
            BaseEditor::setSliderProps(slider);
        }

        m_io_sliders[kInput].setName("In");
        m_io_sliders[kOutput].setName("Out");

        m_in_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef
            .getTreeState(),
            "inputGainID" +
            juce::String(
                processorRef.getProcessorID()),
            m_io_sliders[kInput]);

        m_out_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef
            .getTreeState(),
            "outputGainID" +
            juce::String(
                processorRef.getProcessorID()),
            m_io_sliders[kOutput]);

        // MENUS
        juce::StringArray items = {"Presets"};
        BaseEditor::setComboBoxProps(m_preset_browser, items);

        items.clear();
        items = {"Off", "X2", "X4", "X8", "X16"};
        BaseEditor::setComboBoxProps(m_oversampling_menu, items);
        m_oversampling_menu_attach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            processorRef
            .getTreeState(),
            "oversamplingChoiceID" +
            juce::String(
                processorRef.getProcessorID()),
            m_oversampling_menu);

        // BUTTONS
        setButtonProps(m_buttons[kMute], "M");
        setButtonProps(m_buttons[kSolo], "S");
        setButtonProps(m_buttons[kDelete], "X");
        m_buttons[kMute].setClickingTogglesState(true);
        m_buttons[kSolo].setClickingTogglesState(true);
        m_buttons[kDelete].setClickingTogglesState(false);

        m_buttons[kDelete].onClick = [this]()
        {
            const auto ptrValue = reinterpret_cast<std::uintptr_t>(this);
            sendActionMessage(
                viator::globals::ActionCommands::editorDeleted + juce::String::toHexString(static_cast<juce::int64>(ptrValue)));
        };

        m_mute_attach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef
            .getTreeState(), "muteID" +
                             juce::String(
                                 processorRef.getProcessorID()),
            m_buttons[kMute]);

        for (auto &meter: m_input_meters) {
            addAndMakeVisible(meter);
        }

        for (auto &meter: m_output_meters) {
            addAndMakeVisible(meter);
        }

        m_output_meters[0].setFillDirection(LevelMeter::FillDirection::RightToLeft);
        m_output_meters[1].setFillDirection(LevelMeter::FillDirection::RightToLeft);

        startTimerHz(30);
    }

    BaseEditor::~BaseEditor()
    {
        stopTimer();

        m_io_sliders[kInput].setLookAndFeel(nullptr);
        m_io_sliders[kOutput].setLookAndFeel(nullptr);
        m_preset_browser.setLookAndFeel(nullptr);
        m_oversampling_menu.setLookAndFeel(nullptr);

        for (auto &button: m_buttons) {
            button.setLookAndFeel(nullptr);
        }

        m_in_attach.reset();
        m_out_attach.reset();
    }

    //==============================================================================
    void BaseEditor::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::black);

        const auto bounds = getLocalBounds();
        constexpr auto contrast = 0.2f;
        const auto center = static_cast<float>(bounds.getCentreX());
        auto y = static_cast<float>(bounds.getY());
        const auto bottom = static_cast<float>(bounds.getBottom());

        const juce::ColourGradient faceGrad(
            m_comp_bg.brighter(contrast),
            center, y,
            m_comp_bg.darker(contrast),
            center, bottom,
            false
        );

        g.setGradientFill(faceGrad);
        g.fillRect(bounds);

        g.drawImageAt(m_noise, 0, 0);

        constexpr auto padding = 1;
        const auto header_bottom = juce::roundToInt(getHeight() * 0.05) + 12;
        const auto footer_top = juce::roundToInt(getHeight() * 0.91);

        g.setColour(Colors::main_bg());
        g.fillRect(padding, 0, getWidth() - padding, header_bottom);
        g.fillRect(padding, footer_top, getWidth() - padding, juce::roundToInt(getHeight() * 0.09));

        g.setColour(Colors::medium_bg());
        g.drawRect(0, 0, getWidth(), getHeight(), padding);
        y = static_cast<float>(getHeight()) * 0.05f + 12.0f;
        g.drawLine(padding, y, static_cast<float>(getWidth()) - padding, y, padding);
        y = static_cast<float>(getHeight()) * 0.91f;
        g.drawLine(padding, y, static_cast<float>(getWidth()) - padding, y, padding);
    }

    void BaseEditor::resized()
    {
        if (getWidth() > 0 && getHeight() > 0)
            m_noise = makeNoiseImage(getWidth(), getHeight(), 0.02f);

        // SLIDERS
        auto width = juce::roundToInt(getHeight() * 0.1);
        auto height = width;
        auto x = 0;
        auto y = juce::roundToInt(getHeight() * 0.91);
        m_io_sliders[kInput].setBounds(x, y, width, height);
        m_io_sliders[kInput].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        // LABELS
        x = getWidth() - width;
        m_io_sliders[kOutput].setBounds(x, y, width, height);
        m_io_sliders[kOutput].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        constexpr auto padding = 1;

        x = juce::roundToInt(getWidth() * 0.16);
        y = juce::roundToInt(getHeight() * 0.942);
        width = juce::roundToInt(getWidth() * 0.26);
        height = juce::roundToInt(getHeight() * 0.018);
        m_input_meters[0].setBounds(x, y, width, height);
        y += height + 2;
        m_input_meters[1].setBounds(x, y, width, height);
        x += width + x;
        m_output_meters[0].setBounds(x, m_input_meters[0].getY(), width, height);
        m_output_meters[1].setBounds(x, m_input_meters[1].getY(), width, height);

        // MENUS
        x = 4;
        width = juce::roundToInt(getWidth() * 0.335);
        height = juce::roundToInt(getHeight() * 0.05);
        y = juce::roundToInt(getHeight() * 0.012);
        m_preset_browser.setBounds(x, y, width, height);
        x += width + padding;
        width = juce::roundToInt(width * 0.75);
        m_oversampling_menu.setBounds(x, y, width, height);
        width = juce::roundToInt(width * 0.5);
        x = m_oversampling_menu.getRight() + padding;
        for (auto &button: m_buttons) {
            button.setBounds(x, y, width, height);
            x += width + padding;
        }
    }

    void BaseEditor::setSliderProps(juce::Slider &slider)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::transparentBlack);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, Colors::dial_bg());
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);
        slider.setLookAndFeel(&m_io_laf);
        addAndMakeVisible(slider);
    }

    void BaseEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
    {
        box.addItemList(items, 1);
        box.setLookAndFeel(&m_menu_laf);
        box.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::whitesmoke.withAlpha(0.0f));
        box.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::backgroundColourId, Colors::light_bg());
        addAndMakeVisible(box);
        box.setSelectedId(1);
    }

    void BaseEditor::setButtonProps(juce::TextButton &button, const juce::String &name)
    {
        button.setButtonText(name);
        button.setColour(juce::ComboBox::ColourIds::outlineColourId,
                         juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::ColourIds::buttonOnColourId, Colors::light_bg().withAlpha(0.5f));
        button.setColour(juce::TextButton::ColourIds::textColourOffId, Colors::text());
        button.setColour(juce::TextButton::ColourIds::textColourOnId, Colors::text());
        button.setLookAndFeel(&m_button_laf);
        addAndMakeVisible(button);
    }

    void BaseEditor::timerCallback()
    {
        const auto in = processorRef.getInputLevelsStereo();
        const auto out = processorRef.getOutputLevelsStereo();

        m_input_meters[0].setLevel(in.first);
        m_input_meters[1].setLevel(in.second);
        m_output_meters[0].setLevel(out.first);
        m_output_meters[1].setLevel(out.second);
    }

    void BaseEditor::drawVerticalText(juce::Graphics &g,
                                      const juce::String &text,
                                      const juce::Rectangle<float> area,
                                      const juce::Justification just)
    {
        g.saveState();

        // Rotate around the area centre
        const auto c = area.getCentre();
        g.addTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi, c.x, c.y));

        // After rotation, drawText still uses the same area rect (now rotated in world space)
        g.drawText(text, area.toNearestInt(), just, true);

        g.restoreState();
    }

    juce::Image BaseEditor::makeNoiseImage(const int w, const int h, const float amount)
    {
        juce::Image img(juce::Image::ARGB, w, h, true);
        juce::Random rng;

        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                const float n = (rng.nextFloat() * 2.0f - 1.0f); // -1..1
                const float v = 0.5f + 0.5f * n; // 0..1

                // tiny brightness variation around neutral grey
                const auto c = static_cast<juce::uint8>(juce::jlimit(0, 255, static_cast<int>(std::round(v * 255.0f))));

                // amount controls opacity of the grain
                const auto a = static_cast<juce::uint8>(juce::jlimit(0, 255, static_cast<int>(std::round(amount * 255.0f))));

                img.setPixelAt(x, y, juce::Colour::fromRGBA(c, c, c, a));
            }

        return img;
    }
}
