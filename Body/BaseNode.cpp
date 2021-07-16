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

PxVec3 ivc::BaseNode::getHalfExtents() {
    return m_dimension/2;
}

std::pair<float, float> ivc::BaseNode::getSwingLimits() {
    return m_swingLimits;
}

std::pair<float, float> ivc::BaseNode::getTwistLimits() {
    return m_twistLimits;
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

void ivc::BaseNode::mutate() {

    //TODO: ignoring recursion for now
    //TODO: chance to or not to mutate
    //TODO: limit values

    //mutate dimensions
    float newX = Mutator::mutateFloat(m_generator, m_dimension.x, INFINITY, MIN_PART_SIZE);
    float newY = Mutator::mutateFloat(m_generator, m_dimension.y, INFINITY, MIN_PART_SIZE);
    float newZ = Mutator::mutateFloat(m_generator, m_dimension.z, INFINITY, MIN_PART_SIZE);
    m_dimension = PxVec3(newX,newY,newZ);

    //mutate scale
    newX = Mutator::mutateFloat(m_generator, m_scale.x, INFINITY, MIN_SCALE);
    newY = Mutator::mutateFloat(m_generator, m_scale.y, INFINITY, MIN_SCALE);
    newZ = Mutator::mutateFloat(m_generator, m_scale.z, INFINITY, MIN_SCALE);
    m_scale = PxVec3(newX,newY,newZ);

    m_localNeurons->mutate();

    for(auto child : m_childNodeVector){
        child->mutate();
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