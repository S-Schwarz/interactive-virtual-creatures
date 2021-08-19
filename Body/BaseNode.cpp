//
// Created by st on 7/7/21.
//

#include <iostream>
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

PxVec3 ivc::BaseNode::getHalfExtents() {
    return m_dimension/2;
}

std::pair<float, float> ivc::BaseNode::getSwingLimitsY() {
    return m_jointLimitY;
}

std::pair<float, float> ivc::BaseNode::getSwingLimitsZ() {
    return m_jointLimitZ;
}

std::pair<float, float> ivc::BaseNode::getTwistLimits() {
    return m_jointLimitX;
}

ivc::NODE_SIDE ivc::BaseNode::occupyRandomSide() {

    if(m_freeSides.empty())
        return NONE;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(std::begin(m_freeSides), std::end(m_freeSides), rng);

    auto side = m_freeSides.back();
    m_freeSides.pop_back();
    return side;

}

int ivc::BaseNode::setSideAsOccupied(NODE_SIDE side) {

    std::vector<NODE_SIDE>::iterator position = std::find(m_freeSides.begin(), m_freeSides.end(), side);
    if (position != m_freeSides.end()){
        m_freeSides.erase(position);
        return 0;
    }

    return -1;
}

int ivc::BaseNode::getRecursionLimit() {
    return m_recursionLimit;
}

PxVec3 ivc::BaseNode::getScale() {
    return m_scale;
}

ivc::IDHandler *ivc::BaseNode::getIDHandler() {
    return nullptr;
}

std::vector<unsigned long> ivc::BaseNode::getAllAdjacentOutputs() {
    std::vector<unsigned long> inputVec;

    std::vector<unsigned long> parentVec;
    if(m_parentNode != nullptr)
        parentVec = m_parentNode->getLocalNeurons()->getOutputGates();

    std::vector<unsigned long> brainVec = getBrain()->getOutputGates();

    std::vector<unsigned long> childVec;
    for(auto child : m_childNodeVector){
        auto vec = child->getLocalNeurons()->getOutputGates();
        childVec.insert(childVec.end(), vec.begin(), vec.end());
    }

    inputVec.insert(inputVec.end(), parentVec.begin(), parentVec.end());
    inputVec.insert(inputVec.end(), brainVec.begin(), brainVec.end());
    inputVec.insert(inputVec.end(), childVec.begin(), childVec.end());

    return inputVec;
}

ivc::NeuronCluster *ivc::BaseNode::getLocalNeurons() {
    return m_localNeurons;
}

ivc::NeuronCluster *ivc::BaseNode::getBrain() {
    return nullptr;
}

std::vector<unsigned long> ivc::BaseNode::getAllChildOutputs() {
    std::vector<unsigned long> childVec;
    for(auto child : m_childNodeVector){
        auto vec = child->getLocalNeurons()->getOutputGates();
        childVec.insert(childVec.end(), vec.begin(), vec.end());
        auto vec2 = child->getAllChildOutputs();
        childVec.insert(childVec.end(), vec2.begin(), vec2.end());
    }
    return childVec;
}

void ivc::BaseNode::addNeuralConnections() {

}

void ivc::BaseNode::mutateBodyAndNeurons() {

    //TODO: ignoring recursion for now
    //TODO: chance to or not to mutateBodyAndNeurons
    //TODO: limit values

    std::uniform_real_distribution<> dis(0, 1);

    m_localNeurons->mutateNeurons();

    //mutateBodyAndNeurons dimensions
    float newX = m_dimension.x;
    float newY = m_dimension.y;
    float newZ = m_dimension.z;

    if(dis(*m_generator) <= MUTATE_DIMENSION_CHANCE)
        newX = Mutator::mutateFloat(m_generator, m_dimension.x, INFINITY, MIN_PART_SIZE);
    if(dis(*m_generator) <= MUTATE_DIMENSION_CHANCE)
        newY = Mutator::mutateFloat(m_generator, m_dimension.y, INFINITY, MIN_PART_SIZE);
    if(dis(*m_generator) <= MUTATE_DIMENSION_CHANCE)
        newZ = Mutator::mutateFloat(m_generator, m_dimension.z, INFINITY, MIN_PART_SIZE);

    m_dimension = PxVec3(newX,newY,newZ);

    //mutateBodyAndNeurons scale
    newX = m_scale.x;
    newY = m_scale.y;
    newZ = m_scale.z;

    if(dis(*m_generator) <= MUTATE_SCALE_CHANCE)
        newX = Mutator::mutateFloat(m_generator, m_scale.x, INFINITY, MIN_SCALE);
    if(dis(*m_generator) <= MUTATE_SCALE_CHANCE)
        newY = Mutator::mutateFloat(m_generator, m_scale.y, INFINITY, MIN_SCALE);
    if(dis(*m_generator) <= MUTATE_SCALE_CHANCE)
        newZ = Mutator::mutateFloat(m_generator, m_scale.z, INFINITY, MIN_SCALE);

    m_scale = PxVec3(newX,newY,newZ);

    for(auto child : m_childNodeVector){
        child->mutateBodyAndNeurons();
    }

}

void ivc::BaseNode::mutateNeuralConnections() {

    m_localNeurons->setPossibleInputs(getAllAdjacentOutputs());
    m_localNeurons->mutateConnections();

    for(auto child : m_childNodeVector){
        child->mutateNeuralConnections();
    }

}

void ivc::BaseNode::setChildren(std::vector<BaseNode *> children) {
    m_childNodeVector = children;
}

void ivc::BaseNode::setParent(ivc::BaseNode *parent) {
    m_parentNode = parent;
}

void ivc::BaseNode::setLocalNeurons(ivc::NeuronCluster * cluster) {
    m_localNeurons = cluster;
}

std::mt19937 *ivc::BaseNode::getGenerator() {
    return m_generator;
}

void ivc::BaseNode::setGenerator(std::mt19937 *gen) {
    m_generator = gen;
    m_localNeurons->setGenerator(gen);
    for(auto child : m_childNodeVector){
        child->setGenerator(gen);
    }
}

ivc::BaseNode::~BaseNode() {

    //std::cout << __FUNCTION__ << " at " << this << std::endl;

    delete m_localNeurons;

    for(auto child : m_childNodeVector){
            delete child;
    }

}

void ivc::BaseNode::mutateNewBodyAndNewNeurons() {

    std::uniform_real_distribution<> dis(0, 1);
    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    //reflect child node
    if(dis(*m_generator) <= MUTATE_REFLECTION_CHANCE){
        std::shuffle(std::begin(m_childNodeVector), std::end(m_childNodeVector), rng);
        for(auto child : m_childNodeVector){
            //check if opposite side is free
            NODE_SIDE oppositeSide = getOppositeSide(child->getParentSide());
            if(std::find(m_freeSides.begin(), m_freeSides.end(), oppositeSide) != m_freeSides.end()){
                setSideAsOccupied(oppositeSide);
                auto reflectedChild = reflectChild(child);
                m_childNodeVector.push_back(reflectedChild);
                break;
            }else{
                continue;
            }

        }
    }

}

ivc::BaseNode *ivc::BaseNode::reflectChild(ivc::BaseNode* child) {

    //copy child
    auto reflectedChild = child->copy();
    NODE_SIDE oldSide = child->getParentSide();

    //flip children recursively along axis
    reflectedChild->reflectAlongAxis(oldSide);

    //choose new outputs
    std::map<unsigned long,unsigned long> newOutputIDs;
    reflectedChild->chooseNewNeuronIDs(&newOutputIDs);

    //rewire inputs
    reflectedChild->rewireInputs(&newOutputIDs);

    return reflectedChild;
}

void ivc::BaseNode::reflectAlongAxis(ivc::NODE_SIDE side) {

    m_parentSide = getOppositeSide(m_parentSide);
    m_parentAnchor = flipAnchor(m_parentAnchor);

    //correctly set sides as free/occupied along axis
    auto oppositeSide = getOppositeSide(side);
    bool sideFree;
    bool oppositeFree;

    if(isFree(side)){
        oppositeFree = true;
    }else{
        oppositeFree = false;
    }
    if(isFree(oppositeSide)){
        sideFree = true;
    }else{
        sideFree = false;
    }

    if(sideFree){
        setSideAsFree(side);
    }else{
        setSideAsOccupied(side);
    }
    if(oppositeFree){
        setSideAsFree(oppositeSide);
    }else{
        setSideAsOccupied(oppositeSide);
    }

    //do for all children along axis
    for(auto child : m_childNodeVector){
        if(child->getParentSide() == side || child->getParentSide() == oppositeSide){
            child->reflectAlongAxis(side);
        }
    }

}

void ivc::BaseNode::chooseNewNeuronIDs(std::map<unsigned long, unsigned long>* map) {

    m_localNeurons->chooseNewNeuronIDs(map, getIDHandler());

    for(auto child : m_childNodeVector){
        child->chooseNewNeuronIDs(map);
    }
}

void ivc::BaseNode::rewireInputs(std::map<unsigned long, unsigned long>* map) {

    m_localNeurons->rewireInputs(map);

    for(auto child : m_childNodeVector){
        child->rewireInputs(map);
    }

}

void ivc::BaseNode::setSideAsFree(ivc::NODE_SIDE side) {

    if(std::find(m_freeSides.begin(), m_freeSides.end(), side) == m_freeSides.end()){
        m_freeSides.push_back(side);
    }

}

void ivc::BaseNode::setParentAnchor(PxVec3 newAnchor) {
    m_parentAnchor = newAnchor;
}

void ivc::BaseNode::setParentSide(ivc::NODE_SIDE side) {
    m_parentSide = side;
}

ivc::NODE_SIDE ivc::BaseNode::getParentSide() {
    return m_parentSide;
}

PxVec3 ivc::BaseNode::flipAnchor(PxVec3 oldVec) {
    auto newVec = oldVec;
    if(oldVec.x == 1 || oldVec.x == -1){
        newVec.x = -1 * oldVec.x;
    }else if(oldVec.y == 1 || oldVec.y == -1){
        newVec.y = -1 * oldVec.y;
    }else if(oldVec.z == 1 || oldVec.z == -1){
        newVec.z = -1 * oldVec.z;
    }
    return newVec;
}

ivc::NODE_SIDE ivc::BaseNode::getOppositeSide(ivc::NODE_SIDE old) {
    switch(old){
        case POS_X:
            return NEG_X;
        case NEG_X:
            return POS_X;
        case POS_Y:
            return NEG_Y;
        case NEG_Y:
            return POS_Y;
        case POS_Z:
            return NEG_Z;
        case NEG_Z:
            return POS_Z;
        default:
            throw std::invalid_argument("INVALID NODE_SIDE");
    }
}

bool ivc::BaseNode::isFree(ivc::NODE_SIDE side) {
    if(std::find(m_freeSides.begin(), m_freeSides.end(), side) == m_freeSides.end()){
        return false;
    }else{
        return true;
    }
}
