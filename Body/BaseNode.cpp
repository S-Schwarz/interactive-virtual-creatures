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

std::vector<std::shared_ptr<ivc::BaseNode>> ivc::BaseNode::getChildren() {
    return m_childNodeVector;
}

PxVec3 ivc::BaseNode::getDimensions() {
    return m_dimension;
}

PxVec3 ivc::BaseNode::getOrientationInDegrees() {
    return m_orientation * (180/M_PI);
}

PxVec3 ivc::BaseNode::getParentAnchor() {
    return m_parentAnchor;
}

ivc::BaseNode* ivc::BaseNode::getParentNode() {
    return m_parentNode;
}

PxVec3 ivc::BaseNode::getHalfExtents() {
    return m_dimension/2;
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

std::shared_ptr<ivc::IDHandler> ivc::BaseNode::getIDHandler() {
    if(m_isRoot)
        return m_idHandler;

    return m_parentNode->getIDHandler();
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

std::shared_ptr<ivc::NeuronCluster> ivc::BaseNode::getLocalNeurons() {
    return m_localNeurons;
}

std::shared_ptr<ivc::NeuronCluster> ivc::BaseNode::getBrain() {
    if(m_isRoot)
        return m_brain;

    return m_parentNode->getBrain();
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
    m_localNeurons->setPossibleInputs(getAllAdjacentOutputs());

    if(m_isRoot){
        std::vector<unsigned long> brainInputs = m_localNeurons->getOutputGates();
        auto childOutputs = getAllChildOutputs();
        brainInputs.insert(brainInputs.end(), childOutputs.begin(), childOutputs.end());
        m_brain->setPossibleInputs(brainInputs);
        m_brain->randomizeConnections();
    }

    m_localNeurons->randomizeConnections();
}

void ivc::BaseNode::mutateBodyAndNeurons(bool onlyNeurons, std::shared_ptr<EvoConfig> config) {

    std::uniform_real_distribution<> dis(0, 1);

    m_localNeurons->mutateNeurons(config);
    if(m_brain)
        m_brain->mutateNeurons(config);

    if(!onlyNeurons){
        //mutateBodyAndNeurons dimensions
        float newX = m_dimension.x;
        float newY = m_dimension.y;
        float newZ = m_dimension.z;

        if(dis(*m_generator) <= config->m_mutChance)
            newX = Mutator::mutateFloat(m_generator, m_dimension.x, INFINITY, MIN_PART_SIZE);
        if(dis(*m_generator) <= config->m_mutChance)
            newY = Mutator::mutateFloat(m_generator, m_dimension.y, INFINITY, MIN_PART_SIZE);
        if(dis(*m_generator) <= config->m_mutChance)
            newZ = Mutator::mutateFloat(m_generator, m_dimension.z, INFINITY, MIN_PART_SIZE);

        m_dimension = PxVec3(newX,newY,newZ);

        //mutate orientation
        newX = m_orientation.x;
        newY = m_orientation.y;
        newZ = m_orientation.z;

        if(dis(*m_generator) <= config->m_mutChance)
            newX = Mutator::mutateFloat(m_generator, m_orientation.x, MAX_ROTATION, -MAX_ROTATION);
        if(dis(*m_generator) <= config->m_mutChance)
            newY = Mutator::mutateFloat(m_generator, m_orientation.y, MAX_ROTATION, -MAX_ROTATION);
        if(dis(*m_generator) <= config->m_mutChance)
            newZ = Mutator::mutateFloat(m_generator, m_orientation.z, MAX_ROTATION, -MAX_ROTATION);

        m_orientation = PxVec3(newX,newY,newZ);

        if(!m_isRoot){
            //mutateBodyAndNeurons joint
            if(dis(*m_generator) <= config->m_mutChance){
                std::pair<float,float> newLimitX = {Mutator::mutateFloat(m_generator, m_jointLimits.first, JOINT_LIMIT, -JOINT_LIMIT), Mutator::mutateFloat(m_generator, m_jointLimits.second, JOINT_LIMIT, -JOINT_LIMIT)};
                m_jointLimits = newLimitX;
            }

            if(dis(*m_generator) <= config->m_mutChance){
                chooseNewJointType();
            }

            //mutateBodyAndNeurons parent anchor

            if(dis(*m_generator) <= config->m_mutChance){
                chooseNewAnchorSide();
            }

            if(m_parentSide == POS_X || m_parentSide == NEG_X){
                if(dis(*m_generator) <= config->m_mutChance)
                    m_parentAnchor.y = Mutator::mutateFloat(m_generator,m_parentAnchor.y,0.99,0.01);
                if(dis(*m_generator) <= config->m_mutChance)
                    m_parentAnchor.z = Mutator::mutateFloat(m_generator,m_parentAnchor.z,0.99,0.01);
            }else if(m_parentSide == POS_Y || m_parentSide == NEG_Y){
                if(dis(*m_generator) <= config->m_mutChance)
                    m_parentAnchor.x = Mutator::mutateFloat(m_generator,m_parentAnchor.x,0.99,0.01);
                if(dis(*m_generator) <= config->m_mutChance)
                    m_parentAnchor.z = Mutator::mutateFloat(m_generator,m_parentAnchor.z,0.99,0.01);
            }else{
                if(dis(*m_generator) <= config->m_mutChance)
                    m_parentAnchor.y = Mutator::mutateFloat(m_generator,m_parentAnchor.y,0.99,0.01);
                if(dis(*m_generator) <= config->m_mutChance)
                    m_parentAnchor.x = Mutator::mutateFloat(m_generator,m_parentAnchor.x,0.99,0.01);
            }
        }
    }

    for(auto child : m_childNodeVector){
        child->mutateBodyAndNeurons(onlyNeurons, config);
    }

}

void ivc::BaseNode::mutateNeuralConnections(std::shared_ptr<EvoConfig> config) {

    m_localNeurons->setPossibleInputs(getAllAdjacentOutputs());

    if(m_isRoot){
        std::vector<unsigned long> brainInputs = m_localNeurons->getOutputGates();
        auto childOutputs = getAllChildOutputs();
        brainInputs.insert(brainInputs.end(), childOutputs.begin(), childOutputs.end());
        m_brain->setPossibleInputs(brainInputs);

        m_brain->mutateConnections(config);
    }

    m_localNeurons->mutateConnections(config);

    for(auto child : m_childNodeVector){
        child->mutateNeuralConnections(config);
    }

}

void ivc::BaseNode::setChildren(std::vector<std::shared_ptr<BaseNode>> children) {
    m_childNodeVector = children;
}

void ivc::BaseNode::setParent(BaseNode* parent) {
    m_parentNode = parent;
}

void ivc::BaseNode::setLocalNeurons(std::shared_ptr<ivc::NeuronCluster> cluster) {
    m_localNeurons = cluster;
}

std::shared_ptr<std::mt19937> ivc::BaseNode::getGenerator() {
    return m_generator;
}

void ivc::BaseNode::setGenerator(std::shared_ptr<std::mt19937> gen) {
    m_generator = gen;
    m_localNeurons->setGenerator(gen);
    if(m_brain)
        m_brain->setGenerator(gen);
    for(auto child : m_childNodeVector){
        child->setGenerator(gen);
    }
}

void ivc::BaseNode::mutateNewBodyAndNewNeurons(bool onlyNeurons, std::shared_ptr<EvoConfig> config) {

    std::uniform_real_distribution<> dis(0, 1);
    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    if(!onlyNeurons){
        // remove child node
        if(!m_childNodeVector.empty() && dis(*m_generator) <= config->m_mutChance){
            std::uniform_int_distribution<> remDis(0, m_childNodeVector.size()-1);
            auto index = remDis(*m_generator);
            auto childNode = m_childNodeVector[index];
            auto anchor = childNode->getParentAnchor();

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

        //add child node
        if(dis(*m_generator) <= config->m_mutChance && !m_freeSides.empty()){
            auto newChild = std::make_shared<BaseNode>();
            newChild->init(false, m_generator, this, config);
            m_childNodeVector.emplace_back(newChild);
        }

        //reflect child node one time
        if(dis(*m_generator) <= config->m_reflChance){
            std::shuffle(std::begin(m_childNodeVector), std::end(m_childNodeVector), rng);
            for(auto child : m_childNodeVector){
                //check if opposite side is free
                NODE_SIDE oppositeSide = getOppositeSide(child->getParentSide());
                if(std::find(m_freeSides.begin(), m_freeSides.end(), oppositeSide) != m_freeSides.end()){
                    setSideAsOccupied(oppositeSide);
                    auto reflectedChild = child->reflect();
                    m_childNodeVector.push_back(reflectedChild);
                    break;
                }else{
                    continue;
                }

            }
        }

        //set reflection flag for child
        if(dis(*m_generator) <= config->m_reflChance){
            std::shuffle(std::begin(m_childNodeVector), std::end(m_childNodeVector), rng);
            for(auto child : m_childNodeVector){
                //check if opposite side is free and flag not already set
                NODE_SIDE oppositeSide = getOppositeSide(child->getParentSide());
                if(std::find(m_freeSides.begin(), m_freeSides.end(), oppositeSide) != m_freeSides.end() && !child->shouldBeReflected()){
                    setSideAsOccupied(oppositeSide);
                    child->setReflectionFlag();
                    break;
                }else{
                    continue;
                }

            }
        }
    }

    //add and remove neurons
    m_localNeurons->mutateNewNeurons(getIDHandler(),config);
    if(m_isRoot)
        m_brain->mutateNewNeurons(getIDHandler(),config);

    for(auto child : m_childNodeVector){
        child->mutateNewBodyAndNewNeurons(onlyNeurons, config);
    }

}

std::shared_ptr<ivc::BaseNode> ivc::BaseNode::reflect() {

    //copy this node recursively
    auto reflectedVersion = copy();

    //flip children recursively along axis
    reflectedVersion->reflectAlongAxis(getParentSide());

    //choose new outputs recursively
    std::map<unsigned long,unsigned long> newOutputIDs;
    reflectedVersion->chooseNewNeuronIDs(&newOutputIDs);

    //rewire inputs recursively
    reflectedVersion->rewireInputs(&newOutputIDs);

    return reflectedVersion;
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

bool ivc::BaseNode::shouldBeReflected() {
    return m_reflect;
}

void ivc::BaseNode::setReflectionFlag() {
    m_reflect = true;
}

std::string ivc::BaseNode::getParentSideAsString() {
        switch (m_parentSide) {
            case POS_X:
                return "POS_X";
            case NEG_X:
                return "NEG_X";
            case POS_Y:
                return "POS_Y";
            case NEG_Y:
                return "NEG_Y";
            case POS_Z:
                return "POS_Z";
            case NEG_Z:
                return "NEG_Z";
            case NONE:
                return "NONE";
        }
        return "";
}

void ivc::BaseNode::setBrain(std::shared_ptr<ivc::NeuronCluster> brain) {
    m_brain = brain;
}

std::shared_ptr<ivc::BaseNode> ivc::BaseNode::copy() {
    auto copiedNode = std::make_shared<BaseNode>(*this);

    if(m_idHandler)
        copiedNode->m_idHandler = std::make_shared<IDHandler>(*m_idHandler);

    std::vector<std::shared_ptr<BaseNode>> copiedChildren;
    for(auto child : m_childNodeVector){
        auto newChild = child->copy();
        newChild->setParent(copiedNode.get());
        copiedChildren.push_back(newChild);
    }
    copiedNode->setChildren(copiedChildren);

    copiedNode->setLocalNeurons(m_localNeurons->copy());
    if(m_brain)
        copiedNode->setBrain(m_brain->copy());

    return copiedNode;
}

void ivc::BaseNode::init(bool root, std::shared_ptr<std::mt19937> gen, BaseNode* parent, std::shared_ptr<EvoConfig> config) {
    m_parentNode = parent;
    m_isRoot = root;

    if(m_isRoot){
        std::random_device rd;
        m_generator = std::make_shared<std::mt19937>(rd());

        m_idHandler = std::make_shared<IDHandler>();
        m_brain = std::make_shared<NeuronCluster>(m_generator, true, m_isRoot, getIDHandler(),config);
    }else{
        m_generator = gen;
    }

    m_localNeurons = std::make_shared<NeuronCluster>(m_generator, false, m_isRoot, getIDHandler(),config);

    if(!m_isRoot){
        m_parentAnchor = getAnchorPosition(m_generator);
        m_orientation = PxVec3(0,0,0);
        m_dimension = m_parentNode->getHalfExtents() * 1.75f;
    }

    std::uniform_real_distribution<> dis(0, 1);

    if(m_isRoot){
        for(int i = 0; i < MAX_CHILDREN; ++i){
            if(dis(*m_generator) < CHILD_CHANCE && !m_freeSides.empty()){
                auto newChild = std::make_shared<BaseNode>();
                newChild->init(false, m_generator, this, config);
                m_childNodeVector.emplace_back(newChild);
            }
        }
    }else{
        std::uniform_int_distribution<> jointDis(0, 2);
        setJointType(static_cast<JOINT_TYPE>(jointDis(*m_generator)));
    }

    for(int i = 0; i < FIRST_MUTATION_NUM; ++i){
        mutateBodyAndNeurons(false, config);
        mutateNewBodyAndNewNeurons(false, config);
    }
    addNeuralConnections();
    mutateNeuralConnections(config);
}

PxVec3 ivc::BaseNode::getAnchorPosition(std::shared_ptr<std::mt19937> gen) {
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
            break;
        case(POS_Y):
            posY = 1;
            posX = positions(*gen);
            posZ = positions(*gen);
            setSideAsOccupied(NEG_Y);
            break;
        case(POS_Z):
            posZ = 1;
            posX = positions(*gen);
            posY = positions(*gen);
            setSideAsOccupied(NEG_Z);
            break;
        case(NEG_X):
            posX = -1;
            posY = positions(*gen);
            posZ = positions(*gen);
            setSideAsOccupied(POS_X);
            break;
        case(NEG_Y):
            posY = -1;
            posX = positions(*gen);
            posZ = positions(*gen);
            setSideAsOccupied(POS_Y);
            break;
        case(NEG_Z):
            posZ = -1;
            posX = positions(*gen);
            posY = positions(*gen);
            setSideAsOccupied(POS_Z);
            break;
    }

    return PxVec3(posX,posY,posZ);
}

std::pair<float, float> ivc::BaseNode::getJointLimits() {
    return m_jointLimits;
}

ivc::JOINT_TYPE ivc::BaseNode::getJointType() {
    return m_jointType;
}

void ivc::BaseNode::setJointType(ivc::JOINT_TYPE type) {
    if(type == SWING1){
        m_jointType = SWING1;
        if(m_parentSide == POS_Y || m_parentSide == NEG_Y){
            chooseNewJointType();
        }
    }else if(type == SWING2){
        m_jointType = SWING2;
        if(m_parentSide == POS_Z || m_parentSide == NEG_Z){
            chooseNewJointType();
        }
    }else if(type == TWIST){
        m_jointType = TWIST;
        if(m_parentSide == POS_X || m_parentSide == NEG_X){
            chooseNewJointType();
        }
    }
}

void ivc::BaseNode::chooseNewJointType() {
    std::uniform_real_distribution<> dis(0, 1);
    if(m_jointType == SWING1){
        if(dis(*m_generator) < 0.5){
            setJointType(SWING2);
        }else{
            setJointType(TWIST);
        }
    }else if(m_jointType == SWING2){
        if(dis(*m_generator) < 0.5){
            setJointType(SWING1);
        }else{
            setJointType(TWIST);
        }
    }else if(m_jointType == TWIST){
        if(dis(*m_generator) < 0.5){
            setJointType(SWING1);
        }else{
            setJointType(SWING2);
        }
    }

}

void ivc::BaseNode::chooseNewAnchorSide() {

    std::vector<NODE_SIDE> newSideVec;
    std::sample(m_parentNode->m_freeSides.begin(), m_parentNode->m_freeSides.end(), std::back_inserter(newSideVec), 1, *m_generator);

    if(newSideVec.empty()){
        return;
    }

    changeAnchorSideTo(newSideVec[0]);

}

void ivc::BaseNode::changeAnchorSideTo(ivc::NODE_SIDE newSide) {

    m_parentNode->setSideAsFree(m_parentSide);
    m_parentNode->setSideAsOccupied(newSide);
    m_parentSide = newSide;

    m_parentAnchor = PxVec3(std::clamp(m_parentAnchor.x,0.01f,0.99f), std::clamp(m_parentAnchor.y,0.01f,0.99f), std::clamp(m_parentAnchor.z,0.01f,0.99f));

    switch(m_parentSide){
        case(POS_X):
            m_parentAnchor = PxVec3(1, m_parentAnchor.y, m_parentAnchor.z);
            break;
        case(POS_Y):
            m_parentAnchor = PxVec3(m_parentAnchor.x, 1, m_parentAnchor.z);
            break;
        case(POS_Z):
            m_parentAnchor = PxVec3(m_parentAnchor.x, m_parentAnchor.y, 1);
            break;
        case(NEG_X):
            m_parentAnchor = PxVec3(-1, m_parentAnchor.y, m_parentAnchor.z);
            break;
        case(NEG_Y):
            m_parentAnchor = PxVec3(m_parentAnchor.x, -1, m_parentAnchor.z);
            break;
        case(NEG_Z):
            m_parentAnchor = PxVec3(m_parentAnchor.x, m_parentAnchor.y, -1);
            break;
        case NONE:
            throw std::invalid_argument("INVALID NODE SIDE");
    }

}

std::shared_ptr<ivc::BaseNode> ivc::BaseNode::getRandomNode(std::shared_ptr<std::mt19937> gen) {

    if(m_childNodeVector.empty())
        return nullptr;

    std::vector<std::shared_ptr<BaseNode>> randomChildVec;
    std::sample(m_childNodeVector.begin(), m_childNodeVector.end(), std::back_inserter(randomChildVec), 1, *m_generator);

    std::uniform_real_distribution<> dis(0, 1);

    if(dis(*gen) < 0.25){
        // return
        return randomChildVec[0];
    }else{
        //go deeper
        return randomChildVec[0]->getRandomNode(gen);
    }

}

void ivc::BaseNode::replaceChild(std::shared_ptr<BaseNode> toReplace, std::shared_ptr<BaseNode> replacement) {

    m_childNodeVector.erase(std::remove(m_childNodeVector.begin(), m_childNodeVector.end(), toReplace), m_childNodeVector.end());
    m_childNodeVector.push_back(replacement);

    replacement->setParent(this);

    //choose new outputs recursively
    std::map<unsigned long,unsigned long> newOutputIDs;
    replacement->chooseNewNeuronIDs(&newOutputIDs);

    //rewire inputs recursively
    replacement->rewireInputs(&newOutputIDs);

}

std::vector<unsigned int> ivc::BaseNode::getNeuronActivity() {

    std::vector<std::shared_ptr<Neuron>> activeNeuronsVec;
    std::vector<std::shared_ptr<Neuron>> activeBrainNeuronsVec;
    std::vector<std::pair<std::shared_ptr<JointSensor>,unsigned long>>  activeJointSensorsVec;
    std::vector<std::pair<std::shared_ptr<ContactSensor>,std::vector<unsigned long>>>  activeContactSensorsVec;

    if(m_isRoot){
        std::vector<std::shared_ptr<Neuron>> neuronVec = collectNeuronCopies();
        std::vector<std::shared_ptr<Neuron>> brainNeuronVec = m_brain->getCopyOfNeurons();
        std::vector<std::shared_ptr<JointSensor>> jointSensorVec = collectJointSensorCopies();
        std::vector<std::shared_ptr<ContactSensor>> contactSensorVec = collectContactSensorCopies();
        std::vector<std::shared_ptr<JointEffector>> effectorVec = collectEffectorCopies();

        // find all neurons, sensors that connect to effectors
        std::set<unsigned long> activeIDs;
        std::vector<unsigned long> toCheck;
        std::set<unsigned long> checkedIDs;

        for(auto effector : effectorVec){
            auto inVec = effector->getInputs();
            for(auto id : inVec){
                activeIDs.insert(id);
                toCheck.push_back(id);
            }
        }

        while(!toCheck.empty()){
            auto id = toCheck.back();
            toCheck.pop_back();
            checkedIDs.insert(id);

            std::shared_ptr<Neuron> neuron = nullptr;
            for(auto n : neuronVec){
                if(n->getOutputID() == id){
                    neuron = n;
                    break;
                }
            }

            for(auto n : brainNeuronVec){
                if(n->getOutputID() == id){
                    neuron = n;
                    break;
                }
            }

            if(neuron != nullptr){
                auto inVec = neuron->getInputs();
                for(auto inID : inVec){
                    activeIDs.insert(inID);
                    if(checkedIDs.find(inID) == checkedIDs.end())
                        toCheck.push_back(inID);
                }
            }

        }

        //fill vectors
        for(auto neuron : neuronVec){
            auto id = neuron->getOutputID();
            if(activeIDs.find(id) != activeIDs.end())
                activeNeuronsVec.push_back(neuron);
        }

        for(auto neuron : brainNeuronVec){
            auto id = neuron->getOutputID();
            if(activeIDs.find(id) != activeIDs.end())
                activeBrainNeuronsVec.push_back(neuron);
        }

        for(auto sensor : jointSensorVec){
            auto id = sensor->getOutputID();
            if(activeIDs.find(id) != activeIDs.end())
                activeJointSensorsVec.push_back({sensor,id});
        }

        for(auto contact : contactSensorVec){
            auto idVec = contact->getOutputIDs();
            std::vector<unsigned long> activeOuts;
            for(auto id : idVec){
                if(activeIDs.find(id) != activeIDs.end())
                    activeOuts.push_back(id);
            }
            if(activeOuts.size() > 0)
                activeContactSensorsVec.push_back({contact,activeOuts});
        }

        unsigned int numContact = 0;

        for(auto pair : activeContactSensorsVec){
            numContact += pair.second.size();
        }

        return {(unsigned int)activeNeuronsVec.size(), (unsigned int)activeBrainNeuronsVec.size(), (unsigned int)activeJointSensorsVec.size(), numContact};
    }

    return {0,0,0,0};

}

std::vector<std::shared_ptr<ivc::Neuron>> ivc::BaseNode::collectNeuronCopies() {

    std::vector<std::shared_ptr<ivc::Neuron>> retVec = m_localNeurons->getCopyOfNeurons();

    for(auto child : m_childNodeVector){
        auto childVec = child->collectNeuronCopies();
        retVec.insert(retVec.end(), childVec.begin(), childVec.end());
    }

    return retVec;
}

std::vector<std::shared_ptr<ivc::JointEffector>> ivc::BaseNode::collectEffectorCopies() {
    std::vector<std::shared_ptr<ivc::JointEffector>> retVec;

    if(!m_isRoot)
        retVec.push_back(m_localNeurons->getCopiesOfJointNeurons().second);

    for(auto child : m_childNodeVector){
        auto childVec = child->collectEffectorCopies();
        retVec.insert(retVec.end(), childVec.begin(), childVec.end());
    }

    return retVec;
}

std::vector<std::shared_ptr<ivc::JointSensor>> ivc::BaseNode::collectJointSensorCopies() {
    std::vector<std::shared_ptr<ivc::JointSensor>> retVec;

    if(!m_isRoot)
        retVec.push_back(m_localNeurons->getCopiesOfJointNeurons().first);

    for(auto child : m_childNodeVector){
        auto childVec = child->collectJointSensorCopies();
        retVec.insert(retVec.end(), childVec.begin(), childVec.end());
    }

    return retVec;
}

std::vector<std::shared_ptr<ivc::ContactSensor>> ivc::BaseNode::collectContactSensorCopies() {
    std::vector<std::shared_ptr<ivc::ContactSensor>> retVec = {m_localNeurons->getCopyOfContactSensor()};

    for(auto child : m_childNodeVector){
        auto childVec = child->collectContactSensorCopies();
        retVec.insert(retVec.end(), childVec.begin(), childVec.end());
    }

    return retVec;
}

PxVec3 ivc::BaseNode::getOrientationInRadians() {
    return m_orientation;
}

std::vector<ivc::NODE_SIDE> ivc::BaseNode::getFreeSides() {
    return m_freeSides;
}

bool ivc::BaseNode::isRoot() {
    return m_isRoot;
}

void ivc::BaseNode::addChild(std::shared_ptr<BaseNode> newChild) {
    m_childNodeVector.push_back(newChild);
}

void ivc::BaseNode::setOrientation(PxVec3 vec) {
    m_orientation = vec;
}

void ivc::BaseNode::setDimension(PxVec3 vec) {
    m_dimension = vec;
}

void ivc::BaseNode::setJointLimits(float low, float high) {
    m_jointLimits = {low, high};
}

void ivc::BaseNode::emptyFreeSideVector() {
    m_freeSides = {};
}

void ivc::BaseNode::setAsRoot() {
    m_isRoot = true;
}

void ivc::BaseNode::setIDHandler(std::shared_ptr<ivc::IDHandler> handler) {
    m_idHandler = handler;
}
