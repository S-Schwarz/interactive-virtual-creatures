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

            float m_bestFitnessScore = 0;
            float m_worstFitnessScore = 0;
            float m_averageFitnessScore = 0;

            float m_bestNoveltyScore = 0;
            float m_worstNoveltyScore = 0;
            float m_averageNoveltyScore = 0;

            float m_largestDistanceTravelled = 0;
        public:
            void calculateScoreData(std::vector<std::pair<std::shared_ptr<BaseNode>, float>>, int, bool, std::vector<std::pair<std::shared_ptr<BaseNode>, float>>, bool);

            unsigned int getGeneration();
            void setGeneration(unsigned int);

            float getLargestDistance();
            void setLargestDistance(float);

            float getBestFitnessScore();
            void setBestFitnessScore(float);
            float getWorstFitnessScore();
            void setWorstFitnessScore(float);
            float getAverageFitnessScore();
            void setAverageFitnessScore(float);

            float getBestNoveltyScore();
            void setBestNoveltyScore(float);
            float getWorstNoveltyScore();
            void setWorstNoveltyScore(float);
            float getAverageNoveltyScore();
            void setAverageNoveltyScore(float);

            std::vector<std::pair<std::shared_ptr<BaseNode>, unsigned int>> getParentVec();
            std::vector<std::shared_ptr<BaseNode>> getParents();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVODATA_H
