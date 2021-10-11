//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
#define INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H

#include "../Physics/PhysicsBase.h"
#include "../Body/BaseNode.h"
#include "../Physics/PhysicsScene.h"
#include "EvoData.h"
#include "EvoConfig.h"
#include <vector>
#include <map>
#include <thread>
#include <memory>

namespace ivc{
    class Evolver {
        private:
            std::shared_ptr<PhysicsBase> m_base = nullptr;
            std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>> m_currentBestVector;

            float m_currentLargestDistance = -INFINITY;

            float m_currentBestFitnessScore = -INFINITY;
            float m_currentWorstFitnessScore = INFINITY;
            float m_currentAverageFitnessScore = 0;

            float m_currentBestNoveltyScore = -INFINITY;
            float m_currentWorstNoveltyScore = INFINITY;
            float m_currentAverageNoveltyScore = 0;

            unsigned int m_numberGenerations = 1;

            unsigned int m_numThreads = 1;

            std::shared_ptr<EvoConfig> m_config = nullptr;
            bool m_clearBestVec = false;

            std::vector<std::shared_ptr<EvoData>> m_dataVec;

            std::vector<std::shared_ptr<PhysicsScene>> m_testSceneVec;
            std::map<std::shared_ptr<BaseNode>, std::vector<PxVec3>> m_testPosMap;
            std::vector<std::vector<PxVec3>> m_noveltyArchive;
            std::vector<std::vector<PxVec3>> m_noveltyArchiveCopy;

            std::vector<std::pair<std::shared_ptr<BaseNode>, std::vector<PxVec3>>> m_currentViableCreaturesVec;
            std::map<std::shared_ptr<BaseNode>, float> m_currentFitnessMap;
            std::map<std::shared_ptr<BaseNode>, float> m_currentNoveltyMap;
            std::vector<std::pair<std::shared_ptr<BaseNode>, unsigned int>> m_nextParentVec;

            void createFirstGeneration();
            void createNewGenerationFromParents();
            void calcFitness();
            void calcNovelty();
            void chooseParents();
            void createEvoData();
            void testCurrentGeneration();
            void graft(std::shared_ptr<BaseNode>, std::shared_ptr<BaseNode>, std::shared_ptr<std::mt19937>);
        public:
            int init(std::shared_ptr<PhysicsBase>, std::shared_ptr<EvoConfig>);
            void startContinuousEvolution();
            void stopEvolution();
            std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>> getCurrentBestVector();
            unsigned int getNumberGenerations();
            std::vector<std::shared_ptr<EvoData>> getEvoDataVec();
            void clearBestVec();
            std::vector<std::vector<PxVec3>> getNoveltyArchive();


    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
