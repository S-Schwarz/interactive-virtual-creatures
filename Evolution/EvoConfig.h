//
// Created by steven on 9/15/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H
#define INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H

#include "../Constants.h"

namespace ivc{
    class EvoConfig {
    public:
        int m_creaturesPerGeneration = CREATURES_PER_GENERATION;
        int m_stepsPerGeneration = STEPS_PER_GENERATION;

        bool m_useSidewaysMP = false;
        float m_sidewaysMultiplier = 0.0f;

    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H
