//
// Created by Landon Viator on 1/24/26.
//
#pragma once
#include "Waveshapers.h"
#include "Filter.h"

namespace viator::dsp
{
    class Triode
    {
    public:
        Triode() = default;

        struct TriodeParameters
        {
            float low_shelf_hz = 100.0f;
            float low_shelf_gain = 1.0f;

            float dc_filter_hz = 10.0f;

            float miller_cap_hz = 17000.0f;

            float integrator_hz = 5.0f;

            float clip_point_positive = 4.0f;
            float clip_point_negative = -1.5f;
            float grid_thresh = -1.5f;

            float dc_shift_coeeficient = 1.0f;
        };

        void prepare(const juce::dsp::ProcessSpec &spec)
        {
            const auto sample_rate = static_cast<float>(spec.sampleRate);
            initSmoother(m_drive_smoothers, sample_rate);
            initSmoother(m_input_smoothers, sample_rate);
            initSmoother(m_output_smoothers, sample_rate);
            setSmoother(m_drive_smoothers, 10.0f);
            setSmoother(m_input_smoothers, 0.0f);
            setSmoother(m_output_smoothers, 0.0f);

            initFilter(m_dc_filters, spec, Filter::FilterType::kHighPass);
            initFilter(m_ls_filters, spec, Filter::FilterType::kLowShelf);
            initFilter(m_lp_filters, spec, Filter::FilterType::kLowPass);

            updateFilter(m_dc_filters, 0.0f, 0.3f, m_parameters.dc_filter_hz);
            updateFilter(m_ls_filters, m_parameters.low_shelf_gain, 0.3f, 100.0f);
            updateFilter(m_lp_filters, 0.0f, 0.3f, m_parameters.miller_cap_hz);

            for (auto& filter : m_band_filters)
            {
                filter.prepare(spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                filter.setCutoffFrequency(150.0f);
            }
        }

        void reset()
        {
            for (auto& filter : m_band_filters)
            {
                filter.reset();
            }
        }

        void process(const juce::dsp::AudioBlock<float> &block)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float drive = m_drive_smoothers[ch].getNextValue();
                    const float input = m_input_smoothers[ch].getNextValue();
                    const float output = m_output_smoothers[ch].getNextValue();
                    float xn = data[sample] * input;

                    float low = 0.0f;
                    float high = 0.0f;

                    m_band_filters[ch].processSample(ch, xn, low, high);

                    high = valveGridConduction(high, m_parameters.grid_thresh);
                    high = classAValve(high, drive, m_parameters.grid_thresh,
                                                  m_parameters.clip_point_positive,
                                                  m_parameters.clip_point_negative);
                    xn = low + high;
                    xn = m_dc_filters[channel].processSample(xn, static_cast<int>(channel));

                    xn = m_ls_filters[channel].processSample(xn, static_cast<int>(channel));
                    xn = m_lp_filters[channel].processSample(xn, static_cast<int>(channel));

                    data[sample] = xn * output;
                }
            }
        }

        void setDrive(const float drive)
        {
            constexpr auto min_drive = 0.0f;
            constexpr auto max_drive = 10.0f;

            const auto safe_drive = juce::jlimit(min_drive, max_drive, drive);

            setSmoother(m_drive_smoothers, safe_drive);
        }

        void setInput(const float input)
        {
            constexpr auto min_gain = -20.0f;
            constexpr auto max_gain = 20.0f;

            const auto safe_input = juce::jlimit(min_gain, max_gain, input);

            setSmoother(m_input_smoothers, safe_input);
        }

        void setOutput(const float output)
        {
            constexpr auto min_gain = -20.0f;
            constexpr auto max_gain = 20.0f;

            const auto safe_output = juce::jlimit(min_gain, max_gain, output);

            setSmoother(m_output_smoothers, safe_output);
        }

        void setTriodeParameters(const TriodeParameters &parameters)
        {
            m_parameters = parameters;
            updateFilter(m_dc_filters, 0.0f, 0.3f, m_parameters.dc_filter_hz);
            updateFilter(m_ls_filters, m_parameters.low_shelf_gain, 0.3f, 100.0f);
            updateFilter(m_lp_filters, 0.0f, 0.3f, m_parameters.miller_cap_hz);
        }

    private:
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers, m_input_smoothers, m_output_smoothers;
        TriodeParameters m_parameters;
        std::array<Filter, 2> m_dc_filters, m_ls_filters, m_lp_filters;
        std::array<juce::dsp::LinkwitzRileyFilter<float>, 2> m_band_filters;

        static void initSmoother(std::array<juce::SmoothedValue<float>, 2> &smoother, const float sample_rate)
        {
            for (auto &s: smoother) {
                s.reset(sample_rate, 0.02);
            }
        }

        static void setSmoother(std::array<juce::SmoothedValue<float>, 2> &smoother, const float value)
        {
            for (auto &s: smoother) {
                s.setTargetValue(juce::Decibels::decibelsToGain(value));
            }
        }

        static void initFilter(std::array<Filter, 2> &filters,
                               const juce::dsp::ProcessSpec &spec, const Filter::FilterType type)
        {
            for (auto &filter: filters) {
                filter.prepare(spec);
                filter.setFilterType(type);
            }
        }

        static void updateFilter(std::array<viator::dsp::Filter, 2> &filters,
                          const float gain,
                          const float q,
                          const float cutoff)
        {
            for (auto &filter: filters) {
                filter.setGain(juce::Decibels::decibelsToGain(gain));
                filter.setBandwidth(q);
                filter.setCutoffFrequency(cutoff);
            }
        }
    };
}
