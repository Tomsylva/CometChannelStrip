//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>

namespace viator::dsp
{
    static constexpr float two_pi = 2.0f * juce::MathConstants<float>::pi;
    static constexpr float two_by_pi = 2.0f / juce::MathConstants<float>::pi;

    inline float mixSamples(const float xn, const float yn, const float mix_zero_to_one)
    {
        return (1.0f - mix_zero_to_one) * xn + yn * mix_zero_to_one;
    }

    inline float circleMapWaveshaper(const float xn, const float drive)
    {
        return xn + (drive / two_pi) * std::sin(xn * two_pi);
    }

    inline float hindmarshRoseShaper(float x, float drive)
    {
        // Canonical Hindmarsh–Rose parameters
        constexpr float a = 1.0f;
        constexpr float b = 3.0f;
        constexpr float c = 1.0f;
        constexpr float d = 5.0f;
        constexpr float s = 4.0f;
        constexpr float xR = -1.6f;

        // Choose I to cancel DC: (c + s*xR + I) = 0
        constexpr float I = -(c + s * xR); // = 5.4

        x = std::fmax(-1.0f, std::fmin(1.0f, x));

        // "h" is your distortion strength / drive scaling
        const float h = (0.002f + 0.08f) * drive;

        const float x2 = x * x;
        const float x3 = x2 * x;

        // Exact 1D nullcline-reduced HR: F(x) = -a*x^3 + (b-d)*x^2 - s*x + (c + s*xR + I)
        const float F =
                (-a * x3) +
                ((b - d) * x2) +
                (-s * x) +
                (c + s * xR + I); // becomes 0 by design

        // Euler-step style shaping (still memoryless, but HR-consistent form)
        float y = x + h * F;
        y *= 1.5f;
        y = std::tanh(y * drive); // whether you tanh it or not, the first two original overtones are preserved, which is good
        return y;
    }

    inline float chua(const float x, const float drive)
    {
        constexpr auto m0 = 0.5f;
        constexpr auto m1 = 0.55f;
        constexpr auto bp = 0.4f;
        auto y = m1 * x + 0.5f * (m0 - m1) * (std::abs(x + bp) - std::abs(x - bp));
        y *= 2.0f;
        return std::tanh(y * drive);
    }

    inline float tanh_waveshaper(const float x, const float drive)
    {
        return std::tanh(x * drive);
    }

    inline float atan_waveshaper(const float x, const float drive)
    {
        return two_by_pi * std::atan(x * drive);
    }

    inline float hard_clip(const float x, const float drive)
    {
        return juce::jlimit(-1.0f, 1.0f, x * drive);
    }

    inline float arraya(float x, const float drive)
    {
        x *= drive;
        return (3.0f * x) / 2.0f * (1.0f - (x * x * 0.33f));
    }

    inline float sigmoid(const float x, const float drive)
    {
        return 2.0f * (1.0f / (1.0f + exp(x * -drive))) - 1.0f;
    }

    inline float half_wave(const float x, const float drive)
    {
        return 0.5f * (x + std::abs(x));
    }

    inline float full_wave(const float x, const float drive)
    {
        return std::abs(x);
    }

    inline float polettiWaveshaper(const float xn, const float drive, const float ln = 0.6f, const float lp = 6.6f)
    {
        constexpr float eps = 1.0e-12f;
        const float numerator = xn * drive;
        float denomNeg = 1.0f - numerator / ln;
        float denomPos = 1.0f + numerator / lp;

        // “soft sign epsilon injection” for stability
        denomNeg += copysignf(eps, denomNeg);
        denomPos += copysignf(eps, denomPos);

        const float negative = numerator / denomNeg;
        const float positive = numerator / denomPos;

        const auto mask = static_cast<float>(xn >= 0.0f);

        return negative + (positive - negative) * mask;
    }

    inline float valveGridConduction(const float xn, const float thresh)
    {
        const auto mask = static_cast<float>(xn >= 0.0f);
        const float xn_n = xn;

        float clip_delta = xn - thresh;
        clip_delta = std::fmax(clip_delta, 0.0f);

        const float compression_factor = 0.4473253f + 0.5451584f *
                juce::dsp::FastMathApproximations::exp(-0.3241584f * clip_delta);

        const float xn_p = xn * compression_factor;

        return xn_n + (xn_p - xn_n) * mask;
    }

    inline float classAValve(float xn,
                             const float k,
                             const float thresh,
                             const float clipPos,
                             const float clipNeg)
    {
        float yn = 0.0f;

        if (xn > thresh)
        {
            if (xn > clipPos)
            {
                yn = clipPos;
            } else
            {
                xn -= thresh;

                constexpr float eps = 1.0e-12f;
                const auto is_zero = clipPos - thresh == 0.0f;
                xn /= is_zero ? (clipPos - thresh) : (clipPos - thresh + eps);

                yn = arraya(xn, 1.0f);
                yn *= (clipPos - thresh);
                yn += thresh;
            }
        } else if (xn > 0.0f)
        {
            yn = xn;
        } else
        {
            if (xn < clipNeg)
            {
                yn = clipNeg;
            } else
            {
                xn /= std::fabs(clipNeg);
                yn = std::tanh(xn * k) / tanh(k);
                yn *= std::fabs(clipNeg);
            }
        }

        return yn;
    }

    inline float foldback(float x, const float drive)
    {
        constexpr float thresh = 1.0f;
        float over = 0.0f;

        x *= drive;

        if (std::abs(x) > thresh)
        {
            over = (std::abs(x) - thresh) * 2.0f;
        }

        x < -thresh ? x += over : x -= over;

        return x;
    }

    inline float overdrive(const float x, const float drive)
    {
        constexpr float t_p = 0.25f;
        constexpr float t_n = 0.2f;

        const float xn = x;

        if (xn >= t_p)
            return t_p + (1.0f - t_p) * std::tanh(drive * ((xn - t_p) / (1.0f - t_p)));

        if (xn > -t_n)
            return xn;

        return -(t_n + (1.0f - t_n) * std::tanh(drive * ((-xn - t_n) / (1.0f - t_n))));
    }
}
