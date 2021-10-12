//
// Created by steven on 8/31/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_EVODATA_H
#define INTERACTIVE_VIRTUAL_CREATURES_EVODATA_H

#include <vector>
#include "../Physics/PhysicsBase.h"
#include "../Body/BaseNode.h"

namespace ivc{
    class EvoData {
        private:
            unsigned int m_generationNumber = 0;

            std::vector<std::pair<std::shared_ptr<BaseNode>, unsigned int>> m_parentVec;

        public:
            float m_bestFitnessScore = 0;
            float m_worstFitnessScore = 0;
            float m_averageFitnessScore = 0;

            float m_bestNoveltyScore = 0;
            float m_worstNoveltyScore = 0;
            float m_averageNoveltyScore = 0;

            float m_largestDistanceTravelled = 0;
            float m_worstDistanceTravelled = 0;
            float m_averageDistanceTravelled = 0;

            float m_highestComplexity = 0;
            float m_lowestComplexity = 0;
            float m_averageComplexity = 0;

            float m_mostActiveNeurons = 0;
            float m_leastActiveNeurons= 0;
            float m_averageActiveNeurons = 0;

            float m_mostActiveBrainNeurons = 0;
            float m_leastActiveBrainNeurons= 0;
            float m_averageActiveBrainNeurons = 0;

            float m_mostActiveJointSensors = 0;
            float m_leastActiveJointSensors= 0;
            float m_averageActiveJointSensors = 0;

            float m_mostActiveContactSensors = 0;
            float m_leastActiveContactSensors= 0;
            float m_averageActiveContactSensors = 0;

            float m_complexityOfCurrentBest = 0;
            float m_neuronsOfCurrentBest = 0;
            float m_brainOfCurrentBest = 0;
            float m_jointsOfCurrentBest = 0;
            float m_contactsOfCurrentBest = 0;

            void calculateScoreData(std::vector<std::pair<std::shared_ptr<BaseNode>, float>>, int, bool, std::vector<std::pair<std::shared_ptr<BaseNode>, float>>, bool);

            unsigned int getGeneration();
            void setGeneration(unsigned int);

            std::vector<std::pair<std::shared_ptr<BaseNode>, unsigned int>> getParentVec();
            std::vector<std::shared_ptr<BaseNode>> getParents();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVODATA_H
