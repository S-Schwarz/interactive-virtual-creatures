//
// Created by st on 7/7/21.
//

#include "MorphNode.h"

PxVec3 ivc::MorphNode::getAnchorPosition(std::mt19937 gen){

    std::uniform_real_distribution<> dis(0, 1);
    std::normal_distribution<> positions(0, 0.25);

    auto parentSide = m_parentNode->occupyRandomSide();

    float posX, posY, posZ;

    switch(parentSide){
        case(POS_X):
            posX = 1;
            posY = positions(gen);
            posZ = positions(gen);
            setSideAsOccupied(NEG_X);
            break;
        case(POS_Y):
            posY = 1;
            posX = positions(gen);
            posZ = positions(gen);
            setSideAsOccupied(NEG_Y);
            break;
        case(POS_Z):
            posZ = 1;
            posX = positions(gen);
            posY = positions(gen);
            setSideAsOccupied(NEG_Z);
            break;
        case(NEG_X):
            posX = -1;
            posY = positions(gen);
            posZ = positions(gen);
            setSideAsOccupied(POS_X);
            break;
        case(NEG_Y):
            posY = -1;
            posX = positions(gen);
            posZ = positions(gen);
            setSideAsOccupied(POS_Y);
            break;
        case(NEG_Z):
            posZ = -1;
            posX = positions(gen);
            posY = positions(gen);
            setSideAsOccupied(POS_Z);
            break;
    }

    return PxVec3(posX,posY,posZ);

}

ivc::MorphNode::MorphNode(BaseNode* parent, std::mt19937 gen, unsigned int depth) {

    m_parentNode = parent;

    std::normal_distribution<> dimensions(MEAN_PART_SIZE, MEAN_PART_SIZE * STANDARD_DEVIATION_FACTOR);
    float x = dimensions(gen);
    float y = dimensions(gen);
    float z = dimensions(gen);
    m_dimension = PxVec3(x,y,z);

    m_recursionLimit = 0;   //TODO: randomize(?)

    m_parentAnchor = getAnchorPosition(gen);

    m_orientation = PxVec3(0,0,0);  //TODO: randomize(?)

    std::normal_distribution<> scales(MEAN_SCALE, MEAN_PART_SIZE * STANDARD_DEVIATION_FACTOR);
    float scaX = scales(gen);
    float scaY = scales(gen);
    float scaZ = scales(gen);
    m_scale = PxVec3(scaX, scaY, scaZ);

    m_terminalOnly = false; //TODO: randomize(?)

    std::normal_distribution<> limits(MEAN_JOINT_LIMIT, MEAN_JOINT_LIMIT * STANDARD_DEVIATION_FACTOR);
    float twist = limits(gen);
    m_twistLimits = {-twist, twist};
    m_swingLimits = {limits(gen), limits(gen)};

    std::uniform_real_distribution<> dis(0, 1);

    for(int i = 0; i < MAX_CHILDREN; ++i){
        if(dis(gen) < (CHILD_CHANCE - CHILD_CHANCE_DECREASE * depth))
            m_childNodeVector.emplace_back(new MorphNode(this, gen, depth+1 ));
    }

    //TODO: neurons

}

PxVec3 ivc::MorphNode::getOrientation() {
    return m_orientation;
}

PxVec3 ivc::MorphNode::getParentAnchor() {
    return m_parentAnchor;
}

PxVec3 ivc::MorphNode::getGlobalScale() {
    PxVec3 parentScale = m_parentNode->getGlobalScale();

    return PxVec3(parentScale.x * m_scale.x, parentScale.y * m_scale.y, parentScale.z * m_scale.z);
}

PxVec3 ivc::MorphNode::getScaledHalfExtents() {
    PxVec3 halfExtents = m_dimension/2;
    PxVec3 globalScale = getGlobalScale();

    return PxVec3(halfExtents.x * globalScale.x, halfExtents.y * globalScale.y, halfExtents.z * globalScale.z);
}

std::pair<float, float> ivc::MorphNode::getSwingLimits() {
    return m_swingLimits;
}

std::pair<float, float> ivc::MorphNode::getTwistLimits() {
    return m_twistLimits;
}