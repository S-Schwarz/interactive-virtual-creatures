//
// Created by steven on 8/31/21.
//

#include "EvoData.h"

void ivc::EvoData::setGeneration(unsigned int gen) {
    m_generationNumber = gen;
}

unsigned int ivc::EvoData::getGeneration() {
    return  m_generationNumber;
}

std::vector<std::pair<std::shared_ptr<ivc::BaseNode>, unsigned int>> ivc::EvoData::getParentVec() {
    return m_parentVec;
}

std::vector<std::shared_ptr<ivc::BaseNode>> ivc::EvoData::getParents() {
    std::vector<std::shared_ptr<BaseNode>> parents;
    for(auto pair : m_parentVec){
        parents.push_back(pair.first);
    }
    return parents;
}
