//
// Created by Landon Viator on 10/15/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"


namespace viator
{
    class Images final {
    public:
        Images()
        {
        }

        juce::Image& getNoiseImage() { return m_noise; }

        static juce::Image makeNoiseImage(const int w, const int h, const float amount)
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

        const juce::Image &logo() { return m_logo; }

    private:
        juce::Image m_noise;

    protected:
        juce::Image m_logo{
            juce::ImageCache::getFromMemory(BinaryData::landon_png,
                                            BinaryData::landon_pngSize)
        };

        JUCE_DECLARE_NON_COPYABLE(Images)
    };
}
