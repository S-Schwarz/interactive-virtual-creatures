//
// Created by st on 7/7/21.
//

#include <iostream>
#include "MorphNode.h"

PxVec3 ivc::MorphNode::getAnchorPosition(std::mt19937* gen){

    std::uniform_real_distribution<> dis(0, 1);
    std::normal_distribution<> positions(0, 0.25);

    auto parentSide = m_parentNode->occupyRandomSide();
    m_parentSide = parentSide;

    float posX, posY, posZ;

    switch(parentSide){
        case(POS_X):
            posX = 1;
            posY = positions(*gen);
            posZ = positions(*gen);
            setSideAsOccupied(NEG_X);
            if(m_recursionLimit > 0)
                setSideAsOccupied(POS_X);
            break;
        case(POS_Y):
            posY = 1;
            posX = positions(*gen);
            posZ = positions(*gen);
            setSideAsOccupied(NEG_Y);
            if(m_recursionLimit > 0)
                setSideAsOccupied(POS_Y);
            break;
        case(POS_Z):
            posZ = 1;
            posX = positions(*gen);
            posY = positions(*gen);
            setSideAsOccupied(NEG_Z);
            if(m_recursionLimit > 0)
                setSideAsOccupied(POS_Z);
            break;
        case(NEG_X):
            posX = -1;
            posY = positions(*gen);
            posZ = positions(*gen);
            setSideAsOccupied(POS_X);
            if(m_recursionLimit > 0)
                setSideAsOccupied(NEG_X);
            break;
        case(NEG_Y):
            posY = -1;
            posX = positions(*gen);
            posZ = positions(*gen);
            setSideAsOccupied(POS_Y);
            if(m_recursionLimit > 0)
                setSideAsOccupied(NEG_Y);
            break;
        case(NEG_Z):
            posZ = -1;
            posX = positions(*gen);
            posY = positions(*gen);
            setSideAsOccupied(POS_Z);
            if(m_recursionLimit > 0)
                setSideAsOccupied(NEG_Z);
            break;
    }

    return PxVec3(posX,posY,posZ);

}

PxVec3 ivc::MorphNode::getOrientation() {
    return m_orientation;
}

PxVec3 ivc::MorphNode::getParentAnchor() {
    return m_parentAnchor;
}

ivc::IDHandler *ivc::MorphNode::getIDHandler() {
    return m_parentNode->getIDHandler();
}

void ivc::MorphNode::init(BaseNode* parent, std::mt19937* gen, unsigned int depth) {

    m_generator = gen;
    m_parentNode = parent;
    m_localNeurons = new NeuronCluster(gen, false,false, getIDHandler());

    m_recursionLimit = 0;   //TODO: randomize(?)

    m_parentAnchor = getAnchorPosition(gen);
    m_recursionAnchor = m_parentAnchor;     //TODO: always same (?)

    m_orientation = PxVec3(0,0,0);  //TODO: randomize(?)

    m_terminalOnly = false; //TODO: randomize(?)

    m_dimension = parent->getHalfExtents() * 1.75f;

    std::uniform_real_distribution<> dis(0, 1);

    for(int i = 0; i < MAX_CHILDREN; ++i){
        if(dis(*gen) < (CHILD_CHANCE - CHILD_CHANCE_DECREASE * depth) && !m_freeSides.empty()) {
            MorphNode *newChild = new MorphNode();
            newChild->init(this, gen, depth + 1);
            m_childNodeVector.emplace_back(newChild);
        }
    }

    mutateBodyAndNeurons();

    m_isInitialized = true;
}

ivc::NeuronCluster *ivc::MorphNode::getBrain() {
    return m_parentNode->getBrain();
}

void ivc::MorphNode::addNeuralConnections() {
    m_localNeurons->setPossibleInputs(getAllAdjacentOutputs());
    m_localNeurons->randomizeConnections();
    for(auto child : m_childNodeVector){
        child->addNeuralConnections();
    }
}

void ivc::MorphNode::mutateBodyAndNeurons() {

    std::uniform_real_distribution<> dis(0, 1);

    BaseNode::mutateBodyAndNeurons();

    //mutate orientation
    float newX = m_orientation.x;
    float newY = m_orientation.y;
    float newZ = m_orientation.z;

    if(dis(*m_generator) <= MUTATE_ORIENTATION_CHANCE)
        newX = Mutator::mutateFloat(m_generator, m_orientation.x, MAX_ROTATION, -MAX_ROTATION);
    if(dis(*m_generator) <= MUTATE_ORIENTATION_CHANCE)
        newY = Mutator::mutateFloat(m_generator, m_orientation.y, MAX_ROTATION, -MAX_ROTATION);
    if(dis(*m_generator) <= MUTATE_ORIENTATION_CHANCE)
        newZ = Mutator::mutateFloat(m_generator, m_orientation.z, MAX_ROTATION, -MAX_ROTATION);

    m_orientation = PxVec3(newX,newY,newZ);

    //mutateBodyAndNeurons joint
    if(dis(*m_generator) <= MUTATE_JOINT_CHANCE){
        std::pair<float,float> newLimitX = {Mutator::mutateFloat(m_generator, m_jointLimitX.first, JOINT_LIMIT, -JOINT_LIMIT), Mutator::mutateFloat(m_generator, m_jointLimitX.second, JOINT_LIMIT, -JOINT_LIMIT)};
        m_jointLimitX = newLimitX;
    }

    if(dis(*m_generator) <= MUTATE_JOINT_CHANCE){
        std::pair<float,float> newLimitY = {Mutator::mutateFloat(m_generator, m_jointLimitY.first, JOINT_LIMIT, -JOINT_LIMIT), Mutator::mutateFloat(m_generator, m_jointLimitY.second, JOINT_LIMIT, -JOINT_LIMIT)};
        m_jointLimitY = newLimitY;
    }

    if(dis(*m_generator) <= MUTATE_JOINT_CHANCE){
        std::pair<float,float> newLimitZ = {Mutator::mutateFloat(m_generator, m_jointLimitZ.first, JOINT_LIMIT, -JOINT_LIMIT), Mutator::mutateFloat(m_generator, m_jointLimitZ.second, JOINT_LIMIT, -JOINT_LIMIT)};
        m_jointLimitZ = newLimitZ;
    }

    //mutateBodyAndNeurons parent anchor
    if(m_parentSide == POS_X || m_parentSide == NEG_X){
        if(dis(*m_generator) <= MUTATE_ANCHOR_CHANCE)
            m_parentAnchor.y = Mutator::mutateFloat(m_generator,m_parentAnchor.y,0.99,0.01);
        if(dis(*m_generator) <= MUTATE_ANCHOR_CHANCE)
            m_parentAnchor.z = Mutator::mutateFloat(m_generator,m_parentAnchor.z,0.99,0.01);
    }else if(m_parentSide == POS_Y || m_parentSide == NEG_Y){
        if(dis(*m_generator) <= MUTATE_ANCHOR_CHANCE)
            m_parentAnchor.x = Mutator::mutateFloat(m_generator,m_parentAnchor.x,0.99,0.01);
        if(dis(*m_generator) <= MUTATE_ANCHOR_CHANCE)
            m_parentAnchor.z = Mutator::mutateFloat(m_generator,m_parentAnchor.z,0.99,0.01);
    }else{
        if(dis(*m_generator) <= MUTATE_ANCHOR_CHANCE)
            m_parentAnchor.y = Mutator::mutateFloat(m_generator,m_parentAnchor.y,0.99,0.01);
        if(dis(*m_generator) <= MUTATE_ANCHOR_CHANCE)
            m_parentAnchor.x = Mutator::mutateFloat(m_generator,m_parentAnchor.x,0.99,0.01);
    }

}

void ivc::MorphNode::mutateNewBodyAndNewNeurons() {

    BaseNode::mutateNewBodyAndNewNeurons();

    std::uniform_real_distribution<> dis(0, 1);

    // remove child node
    if(!m_childNodeVector.empty() && dis(*m_generator) <= MUTATE_REMOVE_BODY_CHILD_CHANCE){
        std::uniform_int_distribution<> remDis(0, m_childNodeVector.size()-1);
        auto index = remDis(*m_generator);
        auto childNode = m_childNodeVector[index];
        auto anchor = childNode->getParentAnchor();
        delete childNode;

        m_childNodeVector.erase(m_childNodeVector.begin() + index);

        if(anchor.x == 1)
            m_freeSides.push_back(POS_X);
        else if(anchor.x == -1)
            m_freeSides.push_back(NEG_X);
        else if(anchor.y == 1)
            m_freeSides.push_back(POS_Y);
        else if(anchor.y == -1)
            m_freeSides.push_back(NEG_Y);
        else if(anchor.z == 1)
            m_freeSides.push_back(POS_Z);
        else if(anchor.z == -1)
            m_freeSides.push_back(NEG_Z);

    }

    // TODO: change mean values relative to parent (?)
    if(dis(*m_generator) <= MUTATE_BODY_CONNECTION_CHANCE && !m_freeSides.empty()){
        MorphNode *newChild = new MorphNode();
        newChild->init(this, m_generator, INFINITY);
        m_childNodeVector.emplace_back(newChild);
    }

    m_localNeurons->mutateNewNeurons(getIDHandler());

    for(auto child : m_childNodeVector){
        child->mutateNewBodyAndNewNeurons();
    }

}

ivc::BaseNode *ivc::MorphNode::copy() {
    auto copiedNode = new MorphNode(*this);

    std::vector<BaseNode*> copiedChildren;
    for(auto child : m_childNodeVector){
        auto newChild = child->copy();
        newChild->setParent(copiedNode);
        copiedChildren.push_back(newChild);
    }
    copiedNode->setChildren(copiedChildren);

    copiedNode->setLocalNeurons(m_localNeurons->copy());

    return copiedNode;
}

ivc::MorphNode::~MorphNode() {
   // std::cout << __FUNCTION__ << " at " << this << std::endl;
}
