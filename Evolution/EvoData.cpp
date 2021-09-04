//
// Created by steven on 8/31/21.
//

#include "EvoData.h"

void ivc::EvoData::calculateScoreData(std::vector<std::pair<BaseNode*, float>> scoreVec) {

    float bestScore = -INFINITY;
    float worstScore = INFINITY;
    float scoreSum = 0;

    for(auto pair : scoreVec){
        auto score = pair.second;
        if(score > bestScore)
            bestScore = score;
        if(score < worstScore)
            worstScore = score;
        scoreSum += score;
    }

    m_bestScore = bestScore;
    m_worstScore = worstScore;
    m_averageScore = scoreSum / scoreVec.size();

    //normalize scores
    std::vector<std::pair<BaseNode*, float>> normalizedScores;
    for(auto pair : scoreVec){
        auto score = pair.second;
        if(bestScore == worstScore){
            normalizedScores.push_back({pair.first,1.0f});
        }else{
            normalizedScores.push_back({pair.first,(score-worstScore)/(bestScore-worstScore)});
        }
    }

    //choose best creatures
    std::vector<std::pair<BaseNode*,float>> bestVec;
    for(auto pair : normalizedScores){
        auto score = pair.second;
        if(score > EVOLUTION_MIN_SCORE){
            bestVec.push_back(pair);
        }
    }

    //sort by score
    std::sort(bestVec.begin(), bestVec.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });

    //cap number of parents
    if(bestVec.size() > EVOLUTION_MAX_PARENTS){
        bestVec.resize(EVOLUTION_MAX_PARENTS);
    }

    m_bestCreature = bestVec.front().first;

    //choose amount of children per root
    std::vector<std::pair<BaseNode*,unsigned int>> amountVec;
    float total = 0;
    for(auto pair : bestVec){
        total += pair.second;
    }
    float partSize = CREATURES_PER_GENERATION / total;

    for(auto pair : bestVec){
        auto amountChildren = floor(pair.second * partSize);
        amountVec.push_back({pair.first, amountChildren});
    }

    m_parentVec = amountVec;

}

void ivc::EvoData::setGeneration(unsigned int gen) {
    m_generationNumber = gen;
}

unsigned int ivc::EvoData::getGeneration() {
    return  m_generationNumber;
}

float ivc::EvoData::getBestScore() {
    return m_bestScore;
}

float ivc::EvoData::getWorstScore() {
    return m_worstScore;
}

float ivc::EvoData::getAverageScore() {
    return m_averageScore;
}

ivc::BaseNode *ivc::EvoData::getBestCreature() {
    return m_bestCreature;
}

std::vector<std::pair<ivc::BaseNode*, unsigned int>> ivc::EvoData::getParentVec() {
    return m_parentVec;
}

std::vector<ivc::BaseNode*> ivc::EvoData::getParents() {
    std::vector<BaseNode*> parents;
    for(auto pair : m_parentVec){
        parents.push_back(pair.first);
    }
    return parents;
}
