//
// Created by Landon Viator on 11/1/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>
#include "../../modules/Triode.h"

namespace LVPultecEQParameters
{
    inline const juce::String oversamplingChoiceID = "oversamplingChoiceID";
    inline const juce::String oversamplingChoiceName = "oversamplingChoiceName";

    inline const juce::String muteID = "muteID";
    inline const juce::String muteName = "Mute";

    inline const juce::String lowBoostID = "lowBoostID";
    inline const juce::String lowBoostName = "Low Boost";

    inline const juce::String lowAttenID = "lowAttenID";
    inline const juce::String lowAttenName = "Low Atten";

    inline const juce::String highBoostID = "highBoostID";
    inline const juce::String highBoostName = "High Boost";

    inline const juce::String highAttenID = "highAttenID";
    inline const juce::String highAttenName = "High Atten";

    inline const juce::String highAttenSelID = "highAttenSelID";
    inline const juce::String highAttenSelName = "Sel";

    inline const juce::String lowFreqID = "lowFreqID";
    inline const juce::String lowFreqName = "Low Freq";

    inline const juce::String highFreqID = "highFreqID";
    inline const juce::String highFreqName = "High Freq";

    inline const juce::String bandwidthID = "bandwidthID";
    inline const juce::String bandwidthName = "Bandwidth";

    inline const juce::String tubeButtonID = "tubeButtonID";
    inline const juce::String tubeButtonName = "Tube";

    inline const juce::String powerButtonID = "powerButtonID";
    inline const juce::String powerButtonName = "Power";

    struct parameters {
        explicit parameters(const juce::AudioProcessorValueTreeState &state, const int id)
        {
            oversamplingParam = dynamic_cast<juce::AudioParameterChoice *>(state.getParameter(
                oversamplingChoiceID + juce::String(id)));

            muteParam = dynamic_cast<juce::AudioParameterBool *>(state.getParameter(
                muteID + juce::String(id)));

            lowBoostParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                lowBoostID + juce::String(id)));
            highBoostParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                highBoostID + juce::String(id)));
            lowAttenParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                lowAttenID + juce::String(id)));
            highAttenParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                highAttenID + juce::String(id)));
            highAttenSelParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                highAttenSelID + juce::String(id)));
            lowFreqParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                lowFreqID + juce::String(id)));
            highFreqParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                highFreqID + juce::String(id)));
            bandwidthParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                bandwidthID + juce::String(id)));

            tubeButtonParam = dynamic_cast<juce::AudioParameterBool *>(state.getParameter(
                tubeButtonID + juce::String(id)));
            powerButtonParam = dynamic_cast<juce::AudioParameterBool *>(state.getParameter(
                powerButtonID + juce::String(id)));
        }

        juce::AudioParameterChoice *oversamplingParam{nullptr};
        juce::AudioParameterBool *muteParam{nullptr};

        juce::AudioParameterFloat *lowBoostParam{nullptr};
        juce::AudioParameterFloat *lowAttenParam{nullptr};
        juce::AudioParameterFloat *highBoostParam{nullptr};
        juce::AudioParameterFloat *highAttenParam{nullptr};
        juce::AudioParameterFloat *highAttenSelParam{nullptr};
        juce::AudioParameterFloat *lowFreqParam{nullptr};
        juce::AudioParameterFloat *highFreqParam{nullptr};
        juce::AudioParameterFloat *bandwidthParam{nullptr};

        juce::AudioParameterBool *tubeButtonParam{nullptr};
        juce::AudioParameterBool *powerButtonParam{nullptr};
    };
}

namespace viator::dsp
{
    class LVPultecEQProcessBlock {
    public:
        LVPultecEQProcessBlock() = default;

        ~LVPultecEQProcessBlock() = default;

        enum {
            kLowBoost = 0, kLowAtten, kHighBoost, kHighAtten, kHighAttenSel,
            num_filters
        };

        void prepare(const double sample_rate, const int samples_per_block, const int num_channels, int factor)
        {
            juce::dsp::ProcessSpec spec{};
            m_sample_rate = static_cast<float>(sample_rate);
            spec.sampleRate = m_sample_rate;
            spec.maximumBlockSize = samples_per_block;
            spec.numChannels = num_channels;

            m_oversampler = std::make_unique<juce::dsp::Oversampling<float> >(spec.numChannels,
                                                                              factor,
                                                                              juce::dsp::Oversampling<
                                                                                  float>::FilterType::filterHalfBandPolyphaseIIR,
                                                                              true);
            m_oversampler->initProcessing(spec.maximumBlockSize);

            for (auto &filter: m_filters)
            {
                filter.prepare(spec);
            }

            for (auto &tube: m_tubes)
            {
                tube.prepare(spec);
            }

            setTriodeParams(m_tubes[0], 10.0f, 0.0f, 17000.0f, 8.0f);
            setTriodeParams(m_tubes[1], 10.0f, 0.0f,  17000.0f, 32.0f);
            setTriodeParams(m_tubes[2], 10.0f, 0.0f,  17000.0f, 40.0f);

            m_tubes[0].setInput(-18.0f);
            m_tubes[0].setOutput(-6.0f);
            m_tubes[1].setOutput(-3.0f);
            m_tubes[2].setOutput(27.0f);
        }

        void process(juce::AudioBuffer<float> &buffer, const int num_samples)
        {
            juce::dsp::AudioBlock<float> block(buffer);
            const auto up_sampled_block = m_oversampler->processSamplesUp(block);

            // if (m_tube_on)
            // {
            //     for (auto &tube: m_tubes)
            //     {
            //         tube.process(up_sampled_block);
            //     }
            // }

            m_oversampler->processSamplesDown(block);

            for (auto& filter: m_filters)
            {
                filter.process(juce::dsp::ProcessContextReplacing<float>(block));
            }
        }

        void updateParameters(const LVPultecEQParameters::parameters &parameters)
        {
            const auto low_boost = parameters.lowBoostParam->get();
            const auto high_boost = parameters.highBoostParam->get();
            const auto low_freq = parameters.lowFreqParam->get();
            const auto high_freq = parameters.highFreqParam->get();
            const auto low_atten = parameters.lowAttenParam->get();
            const auto high_atten = parameters.highAttenParam->get();
            const auto high_atten_select = parameters.highAttenSelParam->get();
            const auto bandwidth = parameters.bandwidthParam->get();
            m_tube_on = parameters.tubeButtonParam->get();

            *m_filters[kLowBoost].state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                m_sample_rate, low_freq * 2.0f, bandwidth, juce::Decibels::decibelsToGain(low_boost));
            *m_filters[kLowAtten].state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                m_sample_rate, low_freq * 2.0f * 1.45f, bandwidth, juce::Decibels::decibelsToGain(-low_atten * 0.5f));

            *m_filters[kHighBoost].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(m_sample_rate, high_freq, bandwidth,
                                                                                              juce::Decibels::decibelsToGain(high_boost));
            *m_filters[kHighAtten].state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(m_sample_rate, high_freq, bandwidth,
                                                                                              juce::Decibels::decibelsToGain(-high_atten));

            *m_filters[kHighAttenSel].state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(m_sample_rate, high_atten_select, 0.4f,
                juce::Decibels::decibelsToGain(-16.0f));
        }

    private:
        std::unique_ptr<juce::dsp::Oversampling<float> > m_oversampler;
        using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float> >;
        std::array<Filter, num_filters> m_filters;
        std::array<Triode, 3> m_tubes;
        float m_sample_rate{44100.0f};
        bool m_tube_on{false};

        static inline void setTriodeParams(Triode& triode,
                    const float low_shelf_hz,
                    const float low_shelf_gain,
                    const float miller_cap_hz,
                    const float dc_filter_hz)
        {
            Triode::TriodeParameters params;
            params.low_shelf_hz = low_shelf_hz;
            params.low_shelf_gain = low_shelf_gain;
            params.miller_cap_hz = miller_cap_hz;
            params.dc_filter_hz = dc_filter_hz;
            triode.setTriodeParameters(params);
        }
    };
}
