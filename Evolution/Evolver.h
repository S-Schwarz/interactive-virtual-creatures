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
            float m_largestDistanceTravelled = -INFINITY;

            unsigned int m_numberGenerations = 1;

            unsigned int m_numThreads = 1;

            EvoConfig* m_config = nullptr;

            std::vector<std::shared_ptr<EvoData>> m_dataVec;

            std::map<std::shared_ptr<PhysicsScene>, std::pair<std::shared_ptr<BaseNode>, std::pair<PxVec3, PxVec3>>> m_testSceneMap;
            std::vector<std::pair<std::shared_ptr<BaseNode>, std::pair<PxVec3, PxVec3>>> m_currentViableCreaturesVec;

            std::map<std::shared_ptr<BaseNode>, float> m_fitnessMap;
            std::map<std::shared_ptr<BaseNode>, float> m_noveltyMap;
            std::map<std::shared_ptr<BaseNode>, std::vector<PxVec3>> m_currentGenNoveltyArchive;
            std::vector<std::vector<PxVec3>> m_noveltyArchive;

            void evolveNextGeneration();
            void createNextGeneration();
            void createFirstGeneration();
            std::map<std::shared_ptr<PhysicsScene>, std::pair<std::shared_ptr<BaseNode>, std::pair<PxVec3, PxVec3>>> createNewGenerationFromParents(std::vector<std::pair<std::shared_ptr<BaseNode>,unsigned int>>);
            void deleteLastGeneration(std::vector<std::shared_ptr<BaseNode>>);
            std::vector<std::pair<std::shared_ptr<BaseNode>, float>> getAllFitnessScores();
            std::vector<std::pair<std::shared_ptr<BaseNode>, float>> getAllNoveltyScores();
            void calcFitness();
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
