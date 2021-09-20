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
        bool m_forceDiversity = false;

        bool m_useNoveltySearch = false;
        int m_noveltyInterval = 100;
        int m_noveltyNearestNeighbors = 10;
        bool m_onlyUseEndPos = false;
        bool m_useXAxis = true;
        bool m_useZAxis = true;

        bool m_paused = true;

    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H
