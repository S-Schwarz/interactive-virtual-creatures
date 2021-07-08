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

    m_recursionLimit = 0;   //TODO: randomize(?)

    std::uniform_real_distribution<> dis(0, 1);

    for(int i = 0; i < MAX_CHILDREN; ++i){
        if(dis(gen) < CHILD_CHANCE)
            m_childNodeVector.emplace_back(new MorphNode(gen, 1));
    }

    //TODO: neurons and brain

}