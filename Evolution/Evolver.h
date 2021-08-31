//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
#define INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H

#include "../Physics/PhysicsBase.h"
#include "../Body/RootMorphNode.h"
#include "../Physics/PhysicsScene.h"
#include "EvoData.h"
#include <vector>
#include <map>
#include <thread>

namespace ivc{
    class Evolver {
        private:
            PhysicsBase* m_base = nullptr;
            RootMorphNode* currentBest = nullptr;
            unsigned int m_numberGenerations = 0;

            unsigned int m_numThreads = 1;
            bool m_pauseEvolution = false;

            std::vector<EvoData*> m_dataVec;

            std::map<PhysicsScene*, std::pair<RootMorphNode*, float>> sceneMap;
            void evolveNextGeneration();
            void createNextGeneration();
            void createNewGeneration();
            std::map<PhysicsScene*, std::pair<RootMorphNode*, float>> createNewGenerationFromParents(std::vector<std::pair<RootMorphNode*,unsigned int>>);
            void deleteLastGeneration(std::vector<RootMorphNode*>);
            std::vector<std::pair<RootMorphNode*, float>> getAllScores();
        public:
            int init(PhysicsBase*);
            RootMorphNode* evolveNewCreature();
            void startContinuousEvolution();
            void stopEvolution();
            RootMorphNode* getCurrentBest();
            unsigned int getNumberGenerations();
            std::vector<EvoData*> getEvoDataVec();


    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
