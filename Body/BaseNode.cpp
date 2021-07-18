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

    m_localNeurons->mutateNeurons(getIDHandler());

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
