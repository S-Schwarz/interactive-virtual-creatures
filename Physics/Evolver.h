//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
#define INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H

#include "PhysicsBase.h"
#include "../Body/RootMorphNode.h"
#include "PhysicsScene.h"
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

            std::map<PhysicsScene*, std::pair<RootMorphNode*, float>> sceneMap;
            void evolveNextGeneration();
            void createNextGeneration();
            void createNewGeneration();
            std::map<PhysicsScene*, std::pair<RootMorphNode*, float>> createNewGenerationFromParents(std::vector<std::pair<RootMorphNode*,unsigned int>>);
            void deleteLastGeneration(std::vector<std::pair<RootMorphNode*,float>>);
        public:
            int init(PhysicsBase*);
            RootMorphNode* evolveNewCreature();
            void startContinuousEvolution();
            void stopEvolution();
            RootMorphNode* getCurrentBest();
            unsigned int getNumberGenerations();


    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
