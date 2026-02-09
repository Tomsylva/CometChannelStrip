//
// Created by Landon Viator on 10/15/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"


namespace viator::gui
{
    class Images final {
    public:
        Images()
        {
            m_tube_icon = juce::Drawable::createFromImageData(BinaryData::tube2_svg,
                                                 BinaryData::tube2_svgSize);
        }

        static const juce::Image &logo() { return images().m_logo; }

        static const std::unique_ptr<juce::Drawable> &tube_icon() { return images().m_tube_icon; }

    private:
        static Images &images()
        {
            static Images instance;
            return instance;
        }

    protected:
        juce::Image m_logo{
            juce::ImageCache::getFromMemory(BinaryData::landon_png,
                                            BinaryData::landon_pngSize)
        };

        std::unique_ptr<juce::Drawable> m_tube_icon;

        JUCE_DECLARE_NON_COPYABLE(Images)
    };
}
