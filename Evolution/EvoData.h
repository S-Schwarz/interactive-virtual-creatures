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

            float m_bestScore = 0;
            float m_worstScore = 0;
            float m_averageScore = 0;
        public:
            void calculateScoreData(std::vector<std::pair<BaseNode*, float>>, int);
            void setGeneration(unsigned int);

            unsigned int getGeneration();
            float getBestScore();
            float getWorstScore();
            float getAverageScore();
            BaseNode* getBestCreature();
            std::vector<std::pair<BaseNode*, unsigned int>> getParentVec();
            std::vector<BaseNode*> getParents();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_EVODATA_H
