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

namespace ivc{
    class Evolver {
        private:
            PhysicsBase* m_base = nullptr;
            std::vector<std::pair<BaseNode*,float>> m_currentBestVector;
            float m_largestDistanceTravelled = -INFINITY;

            unsigned int m_numberGenerations = 0;

            unsigned int m_numThreads = 1;

            EvoConfig* m_config = nullptr;

            std::vector<EvoData*> m_dataVec;

            std::map<PhysicsScene*, std::pair<BaseNode*, std::pair<PxVec3, PxVec3>>> m_sceneMap;
            std::map<BaseNode*, float> m_fitnessMap;
            std::map<BaseNode*, float> m_noveltyMap;
            std::map<BaseNode*, std::vector<PxVec3>> m_currentGenNoveltyArchive;
            std::vector<std::vector<PxVec3>> m_noveltyArchive;

            void evolveNextGeneration();
            void createNextGeneration();
            void createNewGeneration();
            std::map<PhysicsScene*, std::pair<BaseNode*, std::pair<PxVec3, PxVec3>>> createNewGenerationFromParents(std::vector<std::pair<BaseNode*,unsigned int>>);
            void deleteLastGeneration(std::vector<BaseNode*>);
            std::vector<std::pair<BaseNode*, float>> getAllFitnessScores();
            std::vector<std::pair<BaseNode*, float>> getAllNoveltyScores();
            void calcFitness();
        public:
            int init(PhysicsBase*, EvoConfig*);
            void startContinuousEvolution();
            void stopEvolution();
            std::vector<std::pair<BaseNode*,float>> getCurrentBestVector();
            unsigned int getNumberGenerations();
            std::vector<EvoData*> getEvoDataVec();


    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
