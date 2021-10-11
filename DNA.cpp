//
// Created by st on 10/11/21.
//

#include "DNA.h"

ivc::DNA::DNA(ivc::BaseNode* rootPtr) {

    idHandler = *rootPtr->getIDHandler();

    int nextID = 0;

    std::map<BaseNode*, int> idMap;
    std::map<BaseNode*, NodeDNA> dnaMap;

    std::vector<BaseNode*> toCheck = {rootPtr};
    // collect all nodes and assign id
    while(!toCheck.empty()){
        auto currentNode = toCheck.back();
        toCheck.pop_back();

        auto dna = NodeDNA(currentNode);
        dna.ID = nextID;
        dnaMap.insert({currentNode, dna});
        idMap[currentNode] = nextID;
        nextID += 1;

        auto children = currentNode->getChildren();
        for(const auto& child : children){
            toCheck.push_back(child.get());
        }
    }

    for(auto [ptr, dna] : dnaMap){
        if(dna.isRoot){
            dna.parentID = -1;
        }else{
            dna.parentID = idMap[ptr->getParentNode()];
        }
        nodeVec.push_back(dna);
    }

}

std::shared_ptr<ivc::BaseNode> ivc::DNA::toRootNode() {

    std::map<int, std::pair<std::shared_ptr<BaseNode>, int>> newNodesMap;

    // create node representation
    for(auto nodeDNA : nodeVec){
        newNodesMap[nodeDNA.ID] = {nodeDNA.toNode(), nodeDNA.parentID};
    }

    std::shared_ptr<ivc::BaseNode> rootNode = nullptr;

    // link nodes
    for(auto [id, pair] : newNodesMap){
        auto nodePtr = pair.first;
        auto parentID = pair.second;
        // root node
        if(parentID == -1){
            rootNode = nodePtr;
            continue;
        }

        auto parent = newNodesMap[parentID].first.get();
        nodePtr->setParent(parent);
        parent->addChild(nodePtr);
    }

    std::random_device rd;
    rootNode->setGenerator(std::make_shared<std::mt19937>(rd()));
    rootNode->setIDHandler(std::make_shared<IDHandler>(idHandler));

    return rootNode;

}

ivc::NodeDNA::NodeDNA(ivc::BaseNode* nodePtr) {

    oriX = nodePtr->getOrientationInRadians().x;
    oriY = nodePtr->getOrientationInRadians().y;
    oriZ = nodePtr->getOrientationInRadians().z;

    dimX = nodePtr->getDimensions().x;
    dimY = nodePtr->getDimensions().y;
    dimZ = nodePtr->getDimensions().z;

    anchorX = nodePtr->getParentAnchor().x;
    anchorY = nodePtr->getParentAnchor().y;
    anchorZ = nodePtr->getParentAnchor().z;

    pSide = nodePtr->getParentSide();
    joint = nodePtr->getJointType();
    auto lims = nodePtr->getJointLimits();
    jointLimLow = lims.first;
    jointLimHigh = lims.second;

    for(auto side : nodePtr->getFreeSides()){
        free.push_back(side);
    }

    reflect = nodePtr->shouldBeReflected();
    isRoot = nodePtr->isRoot();

    for(auto neuron : nodePtr->getLocalNeurons()->getCopyOfNeurons()){
        local.push_back(*neuron);
    }
    contactSensor = *nodePtr->getLocalNeurons()->getCopyOfContactSensor();

    localOutputs = nodePtr->getLocalNeurons()->getOutputGates();
    localInputs = nodePtr->getLocalNeurons()->getPossibleInputs();

    if(isRoot){
        for(auto neuron : nodePtr->getBrain()->getCopyOfNeurons()){
            brain.push_back(*neuron);
        }
        brainOutputs = nodePtr->getBrain()->getOutputGates();
        brainInputs = nodePtr->getBrain()->getPossibleInputs();
    }else{
        jointSensor = *nodePtr->getLocalNeurons()->getCopiesOfJointNeurons().first;
        effector = *nodePtr->getLocalNeurons()->getCopiesOfJointNeurons().second;
    }

}

std::shared_ptr<ivc::BaseNode> ivc::NodeDNA::toNode() {

    auto newNode = std::make_shared<BaseNode>();

    newNode->setOrientation(PxVec3(oriX, oriY, oriZ));
    newNode->setDimension(PxVec3(dimX, dimY, dimZ));
    newNode->setParentAnchor(PxVec3(anchorX, anchorY, anchorZ));
    newNode->setParentSide((NODE_SIDE)pSide);
    newNode->setJointType((JOINT_TYPE)joint);
    newNode->setJointLimits(jointLimLow, jointLimHigh);
    newNode->emptyFreeSideVector();
    for(auto side : free){
        newNode->setSideAsFree((NODE_SIDE)side);
    }
    if(reflect)
        newNode->setReflectionFlag();

    auto neuronCluster = std::make_shared<NeuronCluster>();
    std::vector<std::shared_ptr<Neuron>> neuronVec;
    for(auto neuron : local){
        neuronVec.push_back(neuron.copy());
    }
    neuronCluster->setNeurons(neuronVec);
    neuronCluster->setContactSensor(std::make_shared<ContactSensor>(contactSensor));
    neuronCluster->setOutputGates(localOutputs);
    neuronCluster->setPossibleInputsDirectly(localInputs);

    if(isRoot){
        newNode->setAsRoot();

        auto brainCluster = std::make_shared<NeuronCluster>();
        std::vector<std::shared_ptr<Neuron>> brainVec;
        for(auto neuron : brain){
            brainVec.push_back(neuron.copy());
        }
        brainCluster->setNeurons(brainVec);
        brainCluster->setOutputGates(brainOutputs);
        brainCluster->setPossibleInputsDirectly(brainInputs);
        newNode->setBrain(brainCluster);
    }else{
        neuronCluster->setJointNeurons(std::make_shared<JointSensor>(jointSensor), std::make_shared<JointEffector>(effector));
    }
    newNode->setLocalNeurons(neuronCluster);

    return newNode;
}
