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

        const juce::Image &logo() { return m_logo; }

    private:

    protected:
        juce::Image m_logo{
            juce::ImageCache::getFromMemory(BinaryData::landon_png,
                                            BinaryData::landon_pngSize)
        };

        JUCE_DECLARE_NON_COPYABLE(Images)
    };
}
