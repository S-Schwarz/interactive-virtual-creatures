//
// Created by st on 7/7/21.
//

#include "BaseNode.h"

unsigned int ivc::BaseNode::getNumberOfParts() {

    unsigned int number = 1;
    for(auto child : m_childNodeVector){
        number += child->getNumberOfParts();
    }

    return number;

}

std::vector<ivc::BaseNode*> ivc::BaseNode::getChildren() {
    return m_childNodeVector;
}

PxVec3 ivc::BaseNode::getDimensions() {
    return m_dimension;
}

PxVec3 ivc::BaseNode::getOrientation() {
    return PxVec3(0,0,0);
}

PxVec3 ivc::BaseNode::getParentAnchor() {
    return PxVec3(0,0,0);
}

ivc::BaseNode* ivc::BaseNode::getParentNode() {
    return m_parentNode;
}

PxVec3 ivc::BaseNode::getScaledHalfExtents() {
    return m_dimension/2;
}

PxVec3 ivc::BaseNode::getGlobalScale() {
    return PxVec3(1,1,1);
}

std::pair<float, float> ivc::BaseNode::getSwingLimits() {
    return {0,0};
}

std::pair<float, float> ivc::BaseNode::getTwistLimits() {
    return {0,0};
}

ivc::NODE_SIDE ivc::BaseNode::occupyRandomSide() {

    static auto rng = std::default_random_engine();
    std::shuffle(std::begin(m_freeSides), std::end(m_freeSides), rng);

    auto side = m_freeSides.back();
    m_freeSides.pop_back();
    return side;

}

void ivc::BaseNode::setSideAsOccupied(NODE_SIDE side) {
    std::vector<NODE_SIDE>::iterator position = std::find(m_freeSides.begin(), m_freeSides.end(), side);
    if (position != m_freeSides.end())
        m_freeSides.erase(position);
}