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

std::shared_ptr<ivc::BaseNode> ivc::BaseNode::getParentNode() {
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

PxVec3 ivc::BaseNode::getScale() {
    return m_scale;
}

ivc::IDHandler *ivc::BaseNode::getIDHandler() {
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

ivc::NeuronCluster *ivc::BaseNode::getLocalNeurons() {
    return m_localNeurons;
}

ivc::NeuronCluster *ivc::BaseNode::getBrain() {
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

void ivc::BaseNode::mutateBodyAndNeurons(bool onlyNeurons, EvoConfig* config) {

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

        //mutateBodyAndNeurons scale
        newX = m_scale.x;
        newY = m_scale.y;
        newZ = m_scale.z;

        if(dis(*m_generator) <= config->m_mutChance)
            newX = Mutator::mutateFloat(m_generator, m_scale.x, INFINITY, MIN_SCALE);
        if(dis(*m_generator) <= config->m_mutChance)
            newY = Mutator::mutateFloat(m_generator, m_scale.y, INFINITY, MIN_SCALE);
        if(dis(*m_generator) <= config->m_mutChance)
            newZ = Mutator::mutateFloat(m_generator, m_scale.z, INFINITY, MIN_SCALE);

        m_scale = PxVec3(newX,newY,newZ);

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

void ivc::BaseNode::mutateNeuralConnections(EvoConfig* config) {

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

void ivc::BaseNode::setParent(std::shared_ptr<BaseNode> parent) {
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
    if(m_brain)
        m_brain->setGenerator(gen);
    for(auto child : m_childNodeVector){
        child->setGenerator(gen);
    }
}

ivc::BaseNode::~BaseNode() {

    //std::cout << __FUNCTION__ << " at " << this << std::endl;

    delete m_localNeurons;
    delete m_brain;
    delete m_idHandler;

}

void ivc::BaseNode::mutateNewBodyAndNewNeurons(bool onlyNeurons, EvoConfig* config) {

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
            newChild->init(false, m_generator, shared_from_this(), config);
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

void ivc::BaseNode::setBrain(ivc::NeuronCluster* brain) {
    m_brain = brain;
}

std::shared_ptr<ivc::BaseNode> ivc::BaseNode::copy() {
    auto copiedNode = std::make_shared<BaseNode>(*this);

    if(m_idHandler)
        copiedNode->m_idHandler = new IDHandler(*m_idHandler);

    std::vector<std::shared_ptr<BaseNode>> copiedChildren;
    for(auto child : m_childNodeVector){
        auto newChild = child->copy();
        newChild->setParent(copiedNode);
        copiedChildren.push_back(newChild);
    }
    copiedNode->setChildren(copiedChildren);

    copiedNode->setLocalNeurons(m_localNeurons->copy());
    if(m_brain)
        copiedNode->setBrain(m_brain->copy());

    return copiedNode;
}

void ivc::BaseNode::init(bool root, std::mt19937* gen, std::shared_ptr<BaseNode> parent, EvoConfig* config) {
    m_parentNode = parent;
    m_isRoot = root;

    if(m_isRoot){
        std::random_device rd;
        std::mt19937 generator(rd());
        m_generator = &generator;

        m_idHandler = new IDHandler();
        m_brain = new NeuronCluster(m_generator, true, m_isRoot, getIDHandler(),config);
    }else{
        m_generator = gen;
    }

    m_localNeurons = new NeuronCluster(m_generator, false, m_isRoot, getIDHandler(),config);

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
                newChild->init(false, m_generator, shared_from_this(), config);
                m_childNodeVector.emplace_back(newChild);
            }
        }
    }else{
        std::uniform_int_distribution<> jointDis(0, 2);
        setJointType(static_cast<JOINT_TYPE>(jointDis(*m_generator)));
    }

    mutateBodyAndNeurons(false, config);
    mutateNewBodyAndNewNeurons(false, config);
    addNeuralConnections();
    mutateNeuralConnections(config);
}

PxVec3 ivc::BaseNode::getAnchorPosition(std::mt19937 *gen) {
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
