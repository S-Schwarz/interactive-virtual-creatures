//
// Created by st on 7/7/21.
//

#include "RootMorphNode.h"

PxVec3 ivc::RootMorphNode::getParentAnchor() {
    return m_recursionAnchor;
}

void ivc::RootMorphNode::setRecursionAnchor() {
    std::normal_distribution<> positions(0, 0.25);

    auto anchorSide = occupyRandomSide();

    if(anchorSide == NONE)
        return;

    float posX, posY, posZ;

    switch(anchorSide){
        case(POS_X):
            posX = 1;
            posY = positions(*m_generator);
            posZ = positions(*m_generator);
            setSideAsOccupied(POS_X);
            setSideAsOccupied(NEG_X);
            break;
        case(POS_Y):
            posY = 1;
            posX = positions(*m_generator);
            posZ = positions(*m_generator);
            setSideAsOccupied(POS_Y);
            setSideAsOccupied(NEG_Y);
            break;
        case(POS_Z):
            posZ = 1;
            posX = positions(*m_generator);
            posY = positions(*m_generator);
            setSideAsOccupied(POS_Y);
            setSideAsOccupied(NEG_Z);
            break;
        case(NEG_X):
            posX = -1;
            posY = positions(*m_generator);
            posZ = positions(*m_generator);
            setSideAsOccupied(NEG_X);
            setSideAsOccupied(POS_X);
            break;
        case(NEG_Y):
            posY = -1;
            posX = positions(*m_generator);
            posZ = positions(*m_generator);
            setSideAsOccupied(NEG_Y);
            setSideAsOccupied(POS_Y);
            break;
        case(NEG_Z):
            posZ = -1;
            posX = positions(*m_generator);
            posY = positions(*m_generator);
            setSideAsOccupied(NEG_Z);
            setSideAsOccupied(POS_Z);
            break;
    }

    m_recursionAnchor = PxVec3(posX, posY, posZ);
}

ivc::IDHandler *ivc::RootMorphNode::getIDHandler() {
    return m_idHandler;
}

void ivc::RootMorphNode::init() {
    m_idHandler = new IDHandler();

    std::random_device rd;
    std::mt19937 generator(rd());
    m_generator = &generator;

    m_localNeurons = new NeuronCluster(m_generator, false, true, m_idHandler);
    m_brain = new NeuronCluster(m_generator, true, true, m_idHandler);

    m_recursionLimit = 0;   //TODO: randomize(?)

    if(m_recursionLimit > 0){
        setRecursionAnchor();

        std::normal_distribution<> limits(MEAN_JOINT_LIMIT, MEAN_JOINT_LIMIT * STANDARD_DEVIATION_FACTOR);
        float twist = limits(*m_generator);
        m_jointLimitX = {-twist, twist};
        m_jointLimitY = {limits(*m_generator), limits(*m_generator)};
    }

    std::uniform_real_distribution<> dis(0, 1);
/*
    for(int i = 0; i < MAX_CHILDREN; ++i){
        if(dis(*m_generator) < CHILD_CHANCE && !m_freeSides.empty()){
            MorphNode *newChild = new MorphNode();
            newChild->init(this, m_generator, 1);
            m_childNodeVector.emplace_back(newChild);
        }
    }
*/
    mutateBodyAndNeurons();

    m_isInitialized = true;
}

ivc::NeuronCluster *ivc::RootMorphNode::getBrain() {
    return m_brain;
}

void ivc::RootMorphNode::addNeuralConnections() {

    m_localNeurons->setPossibleInputs(getAllAdjacentOutputs());

    std::vector<unsigned long> brainInputs = m_localNeurons->getOutputGates();
    auto childOutputs = getAllChildOutputs();
    brainInputs.insert(brainInputs.end(), childOutputs.begin(), childOutputs.end());
    m_brain->setPossibleInputs(brainInputs);

    m_localNeurons->randomizeConnections();
    m_brain->randomizeConnections();

    for(auto child : m_childNodeVector){
        child->addNeuralConnections();
    }

}

void ivc::RootMorphNode::mutateBodyAndNeurons() {

    std::uniform_real_distribution<> dis(0, 1);

    BaseNode::mutateBodyAndNeurons();

    m_brain->mutateNeurons(m_idHandler);

    // TODO: change mean values relative to parent (?)
    if(dis(*m_generator) <= MUTATE_BODY_CONNECTION_CHANCE && !m_freeSides.empty()){
        MorphNode *newChild = new MorphNode();
        newChild->init(this, m_generator, INFINITY);
        m_childNodeVector.emplace_back(newChild);
    }

}

void ivc::RootMorphNode::mutateNeuralConnections() {
    BaseNode::mutateNeuralConnections();

    std::vector<unsigned long> brainInputs = m_localNeurons->getOutputGates();
    auto childOutputs = getAllChildOutputs();
    brainInputs.insert(brainInputs.end(), childOutputs.begin(), childOutputs.end());
    m_brain->setPossibleInputs(brainInputs);

    m_brain->mutateConnections();
}

ivc::BaseNode *ivc::RootMorphNode::copy() {
    auto copiedNode = new RootMorphNode(*this);

    copiedNode->m_idHandler = new IDHandler(*m_idHandler);

    std::vector<BaseNode*> copiedChildren;
    for(auto child : m_childNodeVector){
        auto newChild = child->copy();
        newChild->setParent(copiedNode);
        copiedChildren.push_back(newChild);
    }
    copiedNode->setChildren(copiedChildren);

    copiedNode->setLocalNeurons(m_localNeurons->copy());
    copiedNode->setBrain(m_brain->copy());

    return copiedNode;
}

void ivc::RootMorphNode::setBrain(ivc::NeuronCluster * brain) {
    m_brain = brain;
}

void ivc::RootMorphNode::setGenerator(std::mt19937 *gen) {
    BaseNode::setGenerator(gen);
    m_brain->setGenerator(gen);
}
