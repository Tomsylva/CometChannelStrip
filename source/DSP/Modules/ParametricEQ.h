#pragma once

#include <juce_dsp/juce_dsp.h>

namespace viator
{
    template<typename SampleType>
    class ParametricEq
    {
    public:
        ParametricEq() = default;

        void prepare(juce::dsp::ProcessSpec& spec)
        {
            for (auto& drive : m_drive_smoothers)
            {
                drive.reset(spec.sampleRate, 0.02);
                drive.setCurrentAndTargetValue(1.0f);
            }

            for (auto& filter : m_filters)
                filter.prepare(spec);

            m_sample_rate = static_cast<float>(spec.sampleRate);

            updateBandTypes();
        }

        void processBlock(juce::dsp::AudioBlock<float>& block, const int /*num_samples*/)
        {
            for (auto& filter : m_filters)
                filter.process(juce::dsp::ProcessContextReplacing<float>(block));
        }

        void setFilterParameters(const float gain,
                                 const float cutoff,
                                 const float q,
                                 const int index) const
        {
            jassert(juce::isPositiveAndBelow(index, (int)m_filters.size()));

            const float gain_scaled = juce::Decibels::decibelsToGain(gain);
            *m_filters[(size_t) index].state =
                *m_makers[(size_t) index](m_sample_rate, cutoff, q, gain_scaled);
        }

        void setLowBell(const bool shouldLowBell)
        {
            m_low_bell = shouldLowBell;
            updateBandTypes();
        }

        void setHighBell(const bool shouldHighBell)
        {
            m_high_bell = shouldHighBell;
            updateBandTypes();
        }

    private:
        using CoeffPtr = juce::dsp::IIR::Coefficients<float>::Ptr;
        using Maker = CoeffPtr (*)(double, float, float, float);

        static CoeffPtr makePeak(double sr, float cutoff, float q, float g)
        {
            return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, cutoff, q, g);
        }

        static CoeffPtr makeLowShelf(double sr, float cutoff, float q, float g)
        {
            return juce::dsp::IIR::Coefficients<float>::makeLowShelf(sr, cutoff, q, g);
        }

        static CoeffPtr makeHighShelf(double sr, float cutoff, float q, float g)
        {
            return juce::dsp::IIR::Coefficients<float>::makeHighShelf(sr, cutoff, q, g);
        }

        void updateBandTypes()
        {
            m_makers[0] = m_low_bell  ? &makePeak : &makeLowShelf;
            m_makers[1] = &makePeak;
            m_makers[2] = &makePeak;
            m_makers[3] = m_high_bell ? &makePeak : &makeHighShelf;
        }

        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;

        using NotchFilter = juce::dsp::ProcessorDuplicator<
            juce::dsp::IIR::Filter<float>,
            juce::dsp::IIR::Coefficients<float>>;

        std::array<NotchFilter, 4> m_filters;

        float m_sample_rate { 44100.0f };

        bool m_low_bell  { false };
        bool m_high_bell { false };

        std::array<Maker, 4> m_makers { &makeLowShelf, &makePeak, &makePeak, &makeHighShelf };
    };
}