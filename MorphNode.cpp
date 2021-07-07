//
// Created by st on 7/7/21.
//

#include "MorphNode.h"

PxVec3 getAnchorPosition(std::mt19937 gen){

    std::uniform_real_distribution<> dis(0, 1);
    std::normal_distribution<> positions(0, 0.25);

    float posX, posY, posZ;

    float randSign = 1;
    if(dis(gen) < 0.5)
        randSign = -1;

    float rand = dis(gen);

    if(rand < 0.33){
        posX = randSign;
        posY = positions(gen);
        posZ = positions(gen);
    }else if(rand < 0.66){
        posY = randSign;
        posX = positions(gen);
        posZ = positions(gen);
    }else{
        posZ = randSign;
        posX = positions(gen);
        posY = positions(gen);
    }

    return PxVec3(posX,posY,posZ);

}

ivc::MorphNode::MorphNode(std::mt19937 gen, unsigned int depth) {

    std::normal_distribution<> dimensions(MEAN_PART_SIZE, MEAN_PART_SIZE * STANDARD_DEVIATION_FACTOR);
    float x = dimensions(gen);
    float y = dimensions(gen);
    float z = dimensions(gen);
    m_dimension = PxVec3(x,y,z);

    m_recursionLimit = 0;   //TODO: randomize(?)

    m_parentAnchor = getAnchorPosition(gen);
    m_childAnchor = getAnchorPosition(gen);

    m_orientation = PxVec3(0,0,0);  //TODO: randomize(?)

    std::normal_distribution<> scales(MEAN_SCALE, MEAN_PART_SIZE * STANDARD_DEVIATION_FACTOR);
    float scaX = scales(gen);
    float scaY = scales(gen);
    float scaZ = scales(gen);
    m_scale;

    m_terminalOnly = false; //TODO: randomize(?)

    std::uniform_real_distribution<> dis(0, 1);

    for(int i = 0; i < MAX_CHILDREN; ++i){
        if(dis(gen) < (CHILD_CHANCE - CHILD_CHANCE_DECREASE * depth))
            m_childNodeVector.emplace_back(MorphNode(gen, depth+1 ));
    }

    //TODO: neurons

}