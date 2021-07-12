//
// Created by st on 7/7/21.
//

#include "RootMorphNode.h"

ivc::RootMorphNode::RootMorphNode() {

    std::random_device rd;
    std::mt19937 gen(rd());

    std::normal_distribution<> dimensions(MEAN_PART_SIZE, MEAN_PART_SIZE * STANDARD_DEVIATION_FACTOR);
    float x = dimensions(gen);
    float y = dimensions(gen);
    float z = dimensions(gen);
    m_dimension = PxVec3(x,y,z);

    std::normal_distribution<> scales(MEAN_SCALE, MEAN_PART_SIZE * STANDARD_DEVIATION_FACTOR);
    float scaX = scales(gen);
    float scaY = scales(gen);
    float scaZ = scales(gen);
    m_scale = PxVec3(scaX, scaY, scaZ);

    m_recursionLimit = 0;   //TODO: randomize(?)

    if(m_recursionLimit > 0){
        setRecursionAnchor(gen);

        std::normal_distribution<> limits(MEAN_JOINT_LIMIT, MEAN_JOINT_LIMIT * STANDARD_DEVIATION_FACTOR);
        float twist = limits(gen);
        m_twistLimits = {-twist, twist};
        m_swingLimits = {limits(gen), limits(gen)};
    }

    std::uniform_real_distribution<> dis(0, 1);

    for(int i = 0; i < MAX_CHILDREN; ++i){
        if(dis(gen) < CHILD_CHANCE && !m_freeSides.empty())
            m_childNodeVector.emplace_back(new MorphNode(this, gen, 1));
    }

    //TODO: neurons and brain

}

PxVec3 ivc::RootMorphNode::getParentAnchor() {
    return m_recursionAnchor;
}

void ivc::RootMorphNode::setRecursionAnchor(std::mt19937 gen) {
    std::normal_distribution<> positions(0, 0.25);

    auto anchorSide = occupyRandomSide();

    if(anchorSide == NONE)
        return;

    float posX, posY, posZ;

    switch(anchorSide){
        case(POS_X):
            posX = 1;
            posY = positions(gen);
            posZ = positions(gen);
            setSideAsOccupied(POS_X);
            setSideAsOccupied(NEG_X);
            break;
        case(POS_Y):
            posY = 1;
            posX = positions(gen);
            posZ = positions(gen);
            setSideAsOccupied(POS_Y);
            setSideAsOccupied(NEG_Y);
            break;
        case(POS_Z):
            posZ = 1;
            posX = positions(gen);
            posY = positions(gen);
            setSideAsOccupied(POS_Y);
            setSideAsOccupied(NEG_Z);
            break;
        case(NEG_X):
            posX = -1;
            posY = positions(gen);
            posZ = positions(gen);
            setSideAsOccupied(NEG_X);
            setSideAsOccupied(POS_X);
            break;
        case(NEG_Y):
            posY = -1;
            posX = positions(gen);
            posZ = positions(gen);
            setSideAsOccupied(NEG_Y);
            setSideAsOccupied(POS_Y);
            break;
        case(NEG_Z):
            posZ = -1;
            posX = positions(gen);
            posY = positions(gen);
            setSideAsOccupied(NEG_Z);
            setSideAsOccupied(POS_Z);
            break;
    }

    m_recursionAnchor = PxVec3(posX, posY, posZ);
}