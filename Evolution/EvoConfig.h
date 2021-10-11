//
// Created by steven on 9/15/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H
#define INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H

#include "../Constants.h"
#include <vector>

namespace ivc{
    class EvoConfig {
    public:
        int m_creaturesPerGeneration = CREATURES_PER_GENERATION;
        int m_stepsPerGeneration = STEPS_PER_GENERATION;
        float m_mutChance = STANDARD_MUTATION_CHANCE;
        float m_reflChance = MUTATE_REFLECTION_FLAG_CHANCE;
        int m_numberDisplayedCreatures = 1;

        bool m_useSidewaysMP = false;
        float m_sidewaysMultiplier = 0.0f;
        bool m_forceDiversity = false;
        bool m_lockMorph = false;

        bool m_useNoveltySearch = false;
        int m_noveltyInterval = 100;
        int m_noveltyNearestNeighbors = 10;
        bool m_onlyUseEndPos = true;
        int m_noveltyWidth = 30;

        bool m_paused = true;

        std::vector<std::pair<glm::vec3, glm::vec3>> m_objVec;

    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H
