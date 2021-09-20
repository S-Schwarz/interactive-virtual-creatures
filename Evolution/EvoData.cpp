//
// Created by steven on 8/31/21.
//

#include "EvoData.h"

void ivc::EvoData::calculateScoreData(std::vector<std::pair<BaseNode*, float>> scoreVec, int cpg, bool forceDiversity, std::vector<std::pair<BaseNode*, float>> noveltyVec, bool useNovelty) {

    // fitness calc
    m_bestFitnessScore = -INFINITY;
    m_worstFitnessScore = INFINITY;
    float scoreSum = 0;

    for(auto pair : scoreVec){
        auto score = pair.second;
        if(score > m_bestFitnessScore)
            m_bestFitnessScore = score;
        if(score < m_worstFitnessScore)
            m_worstFitnessScore = score;
        scoreSum += score;
    }
    m_averageFitnessScore = scoreSum / scoreVec.size();

    // novelty calc
    m_bestNoveltyScore = -INFINITY;
    m_worstNoveltyScore = INFINITY;
    float noveltySum = 0;

    for(auto pair : noveltyVec){
        auto score = pair.second;
        if(score > m_bestNoveltyScore)
            m_bestNoveltyScore = score;
        if(score < m_worstNoveltyScore)
            m_worstNoveltyScore = score;
        noveltySum += score;
    }
    m_averageNoveltyScore = noveltySum / noveltyVec.size();

    //normalize scores
    std::vector<std::pair<BaseNode*, float>> normalizedScores;
    if(!useNovelty){
        for(auto pair : scoreVec){
            auto score = pair.second;
            if(m_bestFitnessScore == m_worstFitnessScore){
                normalizedScores.push_back({pair.first,1.0f});
            }else{
                normalizedScores.push_back({pair.first, Mutator::normalize(score, m_worstFitnessScore, m_bestFitnessScore)});
            }
        }
    }else{
        for(auto pair : noveltyVec){
            auto score = pair.second;
            if(m_bestNoveltyScore == m_worstNoveltyScore){
                normalizedScores.push_back({pair.first,1.0f});
            }else{
                normalizedScores.push_back({pair.first, Mutator::normalize(score, m_worstNoveltyScore, m_bestNoveltyScore)});
            }
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

    if(!useNovelty && forceDiversity){
        //choose and add by morphologic diversity
        std::map<unsigned int, std::pair<BaseNode*,float>> diverseMap;
        for(auto pair : normalizedScores){
            auto numNodes = pair.first->getNumberOfParts();
            if(diverseMap.find(numNodes) == diverseMap.end()){
                diverseMap[numNodes] = pair;
            }else{
                auto score = pair.second;
                if(score > diverseMap[numNodes].second){
                    diverseMap[numNodes] = pair;
                }
            }
        }

        for (auto const& [key, val] : diverseMap){
            if(std::find(bestVec.begin(), bestVec.end(), val) == bestVec.end()){
                bestVec.push_back(val);
            }
        }
    }

    //choose amount of children per root
    std::vector<std::pair<BaseNode*,unsigned int>> amountVec;
    float total = 0;
    for(auto pair : bestVec){
        total += pair.second;
    }
    float partSize = cpg / total;

    for(auto pair : bestVec){
        auto amountChildren = std::max(1.0f, floor(pair.second * partSize));
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

float ivc::EvoData::getBestFitnessScore() {
    return m_bestFitnessScore;
}

float ivc::EvoData::getWorstFitnessScore() {
    return m_worstFitnessScore;
}

float ivc::EvoData::getAverageFitnessScore() {
    return m_averageFitnessScore;
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

float ivc::EvoData::getBestNoveltyScore() {
    return m_bestNoveltyScore;
}

float ivc::EvoData::getWorstNoveltyScore() {
    return m_worstNoveltyScore;
}

float ivc::EvoData::getAverageNoveltyScore() {
    return m_averageNoveltyScore;
}

void ivc::EvoData::setLargestDistance(float distance) {
    m_largestDistanceTravelled = distance;
}

float ivc::EvoData::getLargestDistance() {
    return m_largestDistanceTravelled;
}

void ivc::EvoData::setBestCreature(ivc::BaseNode *node) {
    m_bestCreature = node;
}
