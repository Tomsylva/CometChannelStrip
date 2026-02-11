//
// Created by Landon Viator on 12/22/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>

namespace viator
{
    class Filter {
    public:
        Filter()
        {
        }

        void prepare(const juce::dsp::ProcessSpec &spec)
        {
            m_spec = spec;
            m_filter_duplicator.prepare(m_spec);

            for (auto &filter: m_filters)
            {
                filter.prepare(m_spec);
            }
        }

        void process(juce::dsp::AudioBlock<float> &block)
        {
            m_filter_duplicator.process(juce::dsp::ProcessContextReplacing<float>{block});
        }

        float processSample(const float xn, const int channel)
        {
            return m_filters[channel].processSample(xn);
        }

        enum class FilterType {
            kLowPass,
            kHighPass,
            kLowShelf,
            kHighShelf,
            kBandShelf
        };

        void setFilterType(const FilterType value)
        {
            m_type = value;
            updateFilter();
        }

        void setCutoffFrequency(const float value)
        {
            const auto safe_cutoff = juce::jlimit(20.0f, 20000.0f, value);
            m_cutoff = safe_cutoff;
            updateFilter();
        }

        void setGain(const float value)
        {
            const auto safe_gain = juce::jlimit(-100.0f, 100.0f, value);
            m_gain = juce::Decibels::decibelsToGain(safe_gain);
            updateFilter();
        }

        void setBandwidth(const float value)
        {
            const auto safe_Q = juce::jlimit(0.01f, 9.99f, value);
            m_bandwidth = safe_Q;
            updateFilter();
        }

        void updateFilter() const
        {
            if (m_spec.sampleRate <= 0.0f)
                return;

            switch (m_type)
            {
                case FilterType::kLowPass: {
                    *m_filter_duplicator.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(m_spec.sampleRate, m_cutoff);
                    for (auto &filter: m_filters)
                    {
                        *filter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(m_spec.sampleRate, m_cutoff);
                    }
                }
                break;
                case FilterType::kHighPass: {
                    *m_filter_duplicator.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(m_spec.sampleRate, m_cutoff);
                    for (auto &filter: m_filters)
                    {
                        *filter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(m_spec.sampleRate, m_cutoff);
                    }
                }
                break;
                case FilterType::kLowShelf: {
                    *m_filter_duplicator.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                        m_spec.sampleRate, m_cutoff, m_bandwidth, m_gain);
                    for (auto &filter: m_filters)
                    {
                        *filter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                        m_spec.sampleRate, m_cutoff, m_bandwidth, m_gain);
                    }
                }
                break;
                case FilterType::kHighShelf: {
                    *m_filter_duplicator.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                        m_spec.sampleRate, m_cutoff, m_bandwidth, m_gain);
                    for (auto &filter: m_filters)
                    {
                        *filter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                        m_spec.sampleRate, m_cutoff, m_bandwidth, m_gain);
                    }
                }
                break;
                case FilterType::kBandShelf: {
                    *m_filter_duplicator.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                        m_spec.sampleRate, m_cutoff, m_bandwidth, m_gain);
                    for (auto &filter: m_filters)
                    {
                        *filter.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                        m_spec.sampleRate, m_cutoff, m_bandwidth, m_gain);
                    }
                }
                break;
            }
        }

    private:
        juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float> > m_filter_duplicator;

        std::array<juce::dsp::IIR::Filter<float>, 2> m_filters;
        std::array<juce::dsp::IIR::Coefficients<float>::Ptr, 2> m_coeffs;

        float m_cutoff{1000.0f}, m_gain{1.0f}, m_bandwidth{0.707f};

        FilterType m_type{FilterType::kLowPass};

        juce::dsp::ProcessSpec m_spec{};
    };
}
