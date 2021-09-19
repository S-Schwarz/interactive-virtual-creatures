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

            BaseNode* m_bestCreature = nullptr;
            std::vector<std::pair<BaseNode*, unsigned int>> m_parentVec;

            float m_bestFitnessScore = 0;
            float m_worstFitnessScore = 0;
            float m_averageFitnessScore = 0;

            float m_bestNoveltyScore = 0;
            float m_worstNoveltyScore = 0;
            float m_averageNoveltyScore = 0;

            float m_largestDistanceTravelled = 0;
        public:
            void calculateScoreData(std::vector<std::pair<BaseNode*, float>>, int, bool, std::vector<std::pair<BaseNode*, float>>, bool);
            void setGeneration(unsigned int);
            void setLargestDistance(float);

            unsigned int getGeneration();
            float getLargestDistance();
            float getBestFitnessScore();
            float getWorstFitnessScore();
            float getAverageFitnessScore();
            float getBestNoveltyScore();
            float getWorstNoveltyScore();
            float getAverageNoveltyScore();
            BaseNode* getBestCreature();
            std::vector<std::pair<BaseNode*, unsigned int>> getParentVec();
            std::vector<BaseNode*> getParents();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVODATA_H
