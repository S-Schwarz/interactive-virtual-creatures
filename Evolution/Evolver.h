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
            float m_currentWorstDistance = INFINITY;
            float m_currentAverageDistance = 0;

            float m_currentBestFitnessScore = -INFINITY;
            float m_currentWorstFitnessScore = INFINITY;
            float m_currentAverageFitnessScore = 0;

            float m_currentBestNoveltyScore = -INFINITY;
            float m_currentWorstNoveltyScore = INFINITY;
            float m_currentAverageNoveltyScore = 0;

            float m_currentHighestComplexity = -INFINITY;
            float m_currentLowestComplexity = INFINITY;
            float m_currentAverageComplexity = 0;

            float m_currentMostActiveNeurons = -INFINITY;
            float m_currentLeastActiveNeurons= INFINITY;
            float m_currentAverageActiveNeurons = 0;

            float m_currentMostActiveBrainNeurons = -INFINITY;
            float m_currentLeastActiveBrainNeurons= INFINITY;
            float m_currentAverageActiveBrainNeurons = 0;

            float m_currentMostActiveJointSensors = -INFINITY;
            float m_currentLeastActiveJointSensors= INFINITY;
            float m_currentAverageActiveJointSensors = 0;

            float m_currentMostActiveContactSensors = -INFINITY;
            float m_currentLeastActiveContactSensors= INFINITY;
            float m_currentAverageActiveContactSensors = 0;

            float m_complexityOfCurrentBest = 0;
            float m_neuronsOfCurrentBest = 0;
            float m_brainOfCurrentBest = 0;
            float m_jointsOfCurrentBest = 0;
            float m_contactsOfCurrentBest = 0;

            unsigned int m_numberGenerations = 1;

            unsigned int m_numThreads = 1;

            std::shared_ptr<BaseNode> m_selectedNode = nullptr;

            std::shared_ptr<EvoConfig> m_config = nullptr;

            std::vector<std::shared_ptr<EvoData>> m_dataVec;

            std::vector<std::shared_ptr<PhysicsScene>> m_testSceneVec;
            std::map<std::shared_ptr<BaseNode>, std::pair<std::vector<PxVec3>, int>> m_testPosMap;

            std::vector<std::vector<PxVec3>> m_noveltyArchive;
            std::vector<std::vector<PxVec3>> m_noveltyArchiveCopy;

            int m_evalsSinceLastAddition = 0;
            float m_noveltyMinFactor = 1.0f;

            std::vector<std::pair<std::shared_ptr<BaseNode>, std::vector<PxVec3>>> m_currentViableCreaturesVec;
            std::map<std::shared_ptr<BaseNode>, float> m_currentFitnessMap;
            std::map<std::shared_ptr<BaseNode>, float> m_currentNoveltyMap;
            std::vector<std::pair<std::shared_ptr<BaseNode>, unsigned int>> m_nextParentVec;

            void createFirstGeneration();
            void createNewGenerationFromParents();
            void calcDistanceTravelled();
            void calcStats();
            void calcFitness();
            void calcNovelty();
            void chooseParents();
            void createEvoData();
            void testCurrentGeneration();
            void graft(std::shared_ptr<BaseNode>, std::shared_ptr<BaseNode>, std::shared_ptr<std::mt19937>);
        public:
            int init(std::shared_ptr<PhysicsBase>, std::shared_ptr<EvoConfig>);
            void clean();
            void startContinuousEvolution();
            std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>> getCurrentBestVector();
            unsigned int getNumberGenerations();
            std::vector<std::shared_ptr<EvoData>> getEvoDataVec();
            std::vector<std::vector<PxVec3>> getNoveltyArchive();
            void loadStartNode(std::shared_ptr<BaseNode>);

            void setUserSelection(std::shared_ptr<BaseNode>);

    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVOLVER_H
