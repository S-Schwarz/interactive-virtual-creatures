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
            PhysicsBase* m_base = nullptr;
            std::vector<std::pair<std::shared_ptr<BaseNode>,float>> m_currentBestVector;

            float m_currentLargestDistance = -INFINITY;
            float m_currentBestFitnessScore = -INFINITY;
            float m_currentWorstFitnessScore = INFINITY;
            float m_currentAverageFitnessScore = 0;

            unsigned int m_numberGenerations = 1;

            unsigned int m_numThreads = 1;

            EvoConfig* m_config = nullptr;

            std::vector<std::shared_ptr<EvoData>> m_dataVec;

            std::vector<std::shared_ptr<PhysicsScene>> m_testSceneVec;
            std::map<std::shared_ptr<BaseNode>, std::vector<PxVec3>> m_testPosMap;
            std::vector<std::vector<PxVec3>> m_noveltyArchive;

            std::vector<std::pair<std::shared_ptr<BaseNode>, std::vector<PxVec3>>> m_currentViableCreaturesVec;
            std::map<std::shared_ptr<BaseNode>, float> m_currentFitnessMap;
            std::map<std::shared_ptr<BaseNode>, float> m_currentNoveltyMap;
            std::map<std::shared_ptr<BaseNode>, std::vector<PxVec3>> m_currentGenNoveltyArchive;
            std::vector<std::pair<std::shared_ptr<BaseNode>, unsigned int>> m_nextParentVec;



            void createFirstGeneration();
            void createNewGenerationFromParents();
            void calcFitness();
            void calcNovelty();
            void chooseParents();
            void createEvoData();
            void testCurrentGeneration();
        public:
            int init(PhysicsBase*, EvoConfig*);
            void startContinuousEvolution();
            void stopEvolution();
            std::vector<std::pair<std::shared_ptr<BaseNode>,float>> getCurrentBestVector();
            unsigned int getNumberGenerations();
            std::vector<std::shared_ptr<EvoData>> getEvoDataVec();


    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
