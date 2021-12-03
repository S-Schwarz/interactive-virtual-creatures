//
// Created by steven on 9/15/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H
#define INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H

#include "../Constants.h"
#include <vector>
#include <string>

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
        bool m_useGeneralNeurons = false;
        bool m_forbidOscis = false;

        bool m_useNoveltySearch = false;
        int m_noveltyInterval = 100;
        int m_noveltyNearestNeighbors = 10;
        int m_noveltyWidth = 30;

        bool m_shouldStart = false;
        bool m_shouldEnd = false;
        bool m_running = false;

        bool m_shouldSave = false;
        bool m_shouldLoad = false;
        std::string m_fileName = "";

        bool m_clearBestVec = false;
        bool m_refreshLiveEnvironment = false;

        bool m_useVision = true;

        bool m_useTarget = false;
        bool m_useRewards = false;
        float m_rewardBonus = 100.0f;
        std::vector<std::pair<glm::vec3, glm::vec3>> m_foodVec;
        std::vector<std::pair<glm::vec3, glm::vec3>> m_objVec;

        bool m_userSelection = false;
        float m_creatureOffset = 30.0f;

        bool m_useMaze = false;

        float m_mazeWidth = 40.0f;
        float m_mazeHeight = 10.0f;
        float m_mazeLength = 150.0f;
        float m_obstacleWidth = (m_mazeWidth/4) * 1.2f;
        float m_mazeStart = -20.0f;
        std::vector<std::pair<glm::vec3, glm::vec3>> m_mazeVec = {
                {glm::vec3(0,0,10), glm::vec3(m_mazeWidth, m_mazeHeight,1)},     //back
                {glm::vec3(-m_mazeWidth/2,0,10 - m_mazeLength/2), glm::vec3(1, m_mazeHeight, m_mazeLength)},     //left
                {glm::vec3(m_mazeWidth/2,0,10 - m_mazeLength/2), glm::vec3(1, m_mazeHeight, m_mazeLength)},     //right
                {glm::vec3(0,0,m_mazeStart  - m_mazeLength/2), glm::vec3(1, m_mazeHeight, m_mazeLength)},     //middle

                {glm::vec3(m_mazeWidth/4,0,m_mazeStart * 2.5f), glm::vec3(m_mazeWidth/2, m_mazeHeight,1)},     //dead end

                {glm::vec3(-m_obstacleWidth/2,0,m_mazeStart), glm::vec3(m_obstacleWidth, m_mazeHeight,1)},     //obstacle
                {glm::vec3((-m_mazeWidth/2) + m_obstacleWidth/2,0,m_mazeStart*1.75f), glm::vec3(m_obstacleWidth, m_mazeHeight,1)},     //obstacle
                {glm::vec3(-m_obstacleWidth/2,0,m_mazeStart*2.5f), glm::vec3(m_obstacleWidth, m_mazeHeight,1)},     //obstacle
                {glm::vec3((-m_mazeWidth/2) + m_obstacleWidth/2,0,m_mazeStart*3.25f), glm::vec3(m_obstacleWidth, m_mazeHeight,1)},     //obstacle
                {glm::vec3(-m_obstacleWidth/2,0,m_mazeStart*4.0f), glm::vec3(m_obstacleWidth, m_mazeHeight,1)},     //obstacle
                {glm::vec3((-m_mazeWidth/2) + m_obstacleWidth/2,0,m_mazeStart*4.75f), glm::vec3(m_obstacleWidth, m_mazeHeight,1)},     //obstacle
                {glm::vec3(-m_obstacleWidth/2,0,m_mazeStart*5.5f), glm::vec3(m_obstacleWidth, m_mazeHeight,1)}     //obstacle
        };

        std::vector<std::pair<glm::vec3, glm::vec3>> getObjVec(){
            auto objVec = m_objVec;
            if(m_useMaze){
                objVec.insert( objVec.end(), m_mazeVec.begin(), m_mazeVec.end() );
            }
            return objVec;
        }

        bool useTargetForEvo(){
            return m_useTarget && !m_foodVec.empty();
        }
        bool useRewardsForEvo(){
            return m_useRewards && !m_foodVec.empty();
        }
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOCONFIG_H
