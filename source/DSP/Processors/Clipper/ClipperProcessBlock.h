//
// Created by Landon Viator on 11/1/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>

namespace ClipperParameters
{
    inline const juce::String oversamplingChoiceID = "oversamplingChoiceID";
    inline const juce::String oversamplingChoiceName = "oversamplingChoiceName";

    inline const juce::String muteID = "muteID";
    inline const juce::String muteName = "Mute";

    inline const juce::String driveID = "driveID";
    inline const juce::String driveName = "Drive";

    inline const juce::String clipTypeID = "clipTypeID";
    inline const juce::String clipTypeName = "Type";

    inline const juce::String inputGainID = "inputGainID";
    inline const juce::String inputGainName = "Input";
    inline const juce::String outputGainID = "outputGainID";
    inline const juce::String outputGainName = "Output";

    struct parameters {
        explicit parameters(const juce::AudioProcessorValueTreeState &state, int id)
        {
            oversamplingParam = dynamic_cast<juce::AudioParameterChoice *>(state.getParameter(
                oversamplingChoiceID + juce::String(id)));
            driveParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                driveID + juce::String(id)));
            typeParam = dynamic_cast<juce::AudioParameterBool *>(state.getParameter(
                clipTypeID + juce::String(id)));
            muteParam = dynamic_cast<juce::AudioParameterBool *>(state.getParameter(
                muteID + juce::String(id)));

            inputParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                inputGainID + juce::String(id)));
            outputParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                outputGainID + juce::String(id)));
        }

        juce::AudioParameterChoice *oversamplingParam{nullptr};
        juce::AudioParameterFloat *driveParam{nullptr};
        juce::AudioParameterBool *typeParam{nullptr};
        juce::AudioParameterBool *muteParam{nullptr};

        juce::AudioParameterFloat *inputParam{nullptr};
        juce::AudioParameterFloat *outputParam{nullptr};
    };
}

namespace viator
{
    class ClipperProcessBlock {
    public:
        ClipperProcessBlock() = default;

        ~ClipperProcessBlock() = default;

        enum class DistortionType {
            kSoftClip,
            kHardClip
        };

        void prepare(const double sample_rate, const int samples_per_block, const int num_channels, int factor)
        {
            juce::dsp::ProcessSpec spec{};
            spec.sampleRate = sample_rate;
            spec.maximumBlockSize = samples_per_block;
            spec.numChannels = num_channels;

            m_oversampler = std::make_unique<juce::dsp::Oversampling<float> >(spec.numChannels,
                                                                              factor,
                                                                              juce::dsp::Oversampling<
                                                                                  float>::FilterType::filterHalfBandPolyphaseIIR,
                                                                              true);
            m_oversampler->initProcessing(spec.maximumBlockSize);

            for (auto &drive: m_drive_smoothers)
            {
                drive.reset(spec.sampleRate <= 0 ? 44100.0f : spec.sampleRate, 0.02);
            }

            for (auto &drive: m_drive_comp_smoothers)
            {
                drive.reset(spec.sampleRate <= 0 ? 44100.0f : spec.sampleRate, 0.02);
            }
        }

        void process(juce::AudioBuffer<float> &buffer, const int num_samples)
        {
            juce::dsp::AudioBlock<float> block(buffer);
            const auto up_sampled_block = m_oversampler->processSamplesUp(block);

            if (m_is_soft)
                softClip(up_sampled_block, buffer.getNumSamples());
            else
                hardClip(up_sampled_block, buffer.getNumSamples());

            m_oversampler->processSamplesDown(block);
        }

        void updateParameters(ClipperParameters::parameters &parameters)
        {
            for (auto &drive: m_drive_smoothers)
            {
                if (parameters.driveParam)
                {
                    const auto raw_drive = parameters.driveParam->get();
                    const auto db_drive = juce::Decibels::decibelsToGain(raw_drive);
                    drive.setTargetValue(db_drive);
                }
            }

            for (auto &drive: m_drive_comp_smoothers)
            {
                if (parameters.driveParam)
                {
                    const auto db_drive = juce::Decibels::decibelsToGain(parameters.driveParam->get() * -0.5f);
                    drive.setTargetValue(m_should_compensate ? db_drive : 1.0f);
                }
            }

            m_is_soft = parameters.typeParam->get();
        }

    private:
        std::unique_ptr<juce::dsp::Oversampling<float> > m_oversampler;
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers, m_drive_comp_smoothers;
        static constexpr float m_two_by_pi = 2.0f / juce::MathConstants<float>::pi;
        bool m_is_soft {true};
        int m_should_compensate{true};

        void softClip(const juce::dsp::AudioBlock<float> &block, const int num_samples)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
            {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < num_samples; ++sample)
                {
                    const float xn = data[sample] * m_drive_smoothers[channel].getNextValue();
                    const float yn = std::tanh(xn);
                    data[sample] = yn;
                }
            }
        }

        void hardClip(const juce::dsp::AudioBlock<float> &block, const int num_samples)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
            {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < num_samples; ++sample)
                {
                    const float xn = data[sample] * m_drive_smoothers[channel].getNextValue();
                    const float yn = std::clamp(xn, -1.0f, 1.0f);
                    data[sample] = yn;
                }
            }
        }
    };
}
