//
// Created by st on 7/7/21.
//

#include "PhysicalCreature.h"

ivc::PhysicalCreature::PhysicalCreature(RootMorphNode rootNode, PxVec3 pos, PxPhysics* pxPhysics) {

    m_position = pos;
    m_physics = pxPhysics;
    m_material = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

    auto rootBody = createBox(rootNode.getDimensions()/2, m_position, PxVec3(0,0,0));

    //add root and brain neurons
    auto rootVec = rootNode.getLocalNeurons()->getCopyOfNeurons();
    auto brainVec = rootNode.getBrain()->getCopyOfNeurons();
    m_neuronVector.insert(m_neuronVector.end(), rootVec.begin(), rootVec.end());
    m_neuronVector.insert(m_neuronVector.end(), brainVec.begin(), brainVec.end());

    buildChildNodes(&rootNode, m_position, PxVec3(1,1,1), rootBody, 0);

    //create output gates for neurons
    for(auto neuron : m_neuronVector){
        auto newGate = new Gate();
        neuron->setOutput(newGate);
        m_gateMap[neuron->getOutputID()] = newGate;
    }
    //and for sensors
    for(auto sensor : m_sensorVector){
        auto outVec = sensor->getOutputIDs();
        std::vector<Gate*> newGates;
        for(auto id : outVec){
            auto newGate = new Gate();
            newGates.push_back(newGate);
            m_gateMap[id] = newGate;
        }
        sensor->setOutputGates(newGates);
    }

    //connect inputs for neurons
    for(auto neuron : m_neuronVector){
        auto inputs = neuron->getGateIDs();
        std::vector<Gate*> gateVec;
        for(auto id : inputs){
            gateVec.push_back(m_gateMap[id]);
        }
        neuron->bindGates(gateVec);
    }
    //and effectors
    for(auto effector : m_effectorVector){
        auto inVec = effector->getGateIDs();
        std::vector<Gate*> gateVec;
        for(auto id : inVec){
            gateVec.push_back(m_gateMap[id]);
        }
        effector->bindGates(gateVec);
    }
/*
    printf("NEURONS: %i\n", m_neuronVector.size());
    for(auto neuron : m_neuronVector){
        printf("IN: \n");
        auto inVec = neuron->getGateIDs();
        for(auto id : inVec){
            printf("%li\n", id);
        }
        printf("OUT: %li\n", neuron->getOutputID());
    }
    printf("SENSORS: %i\n", m_sensorVector.size());
    for(auto sensor : m_sensorVector){
        printf("OUT: \n");
        auto outVec = sensor->getOutputIDs();
        for(auto id : outVec){
            printf("%li\n", id);
        }
    }
    printf("EFFECTORS: %i\n", m_effectorVector.size());
    for(auto effector : m_effectorVector){
        printf("IN: \n");
        auto inVec = effector->getGateIDs();
        for(auto id : inVec){
            printf("%li\n", id);
        }
    }
    printf("GATES: %i\n", m_gateMap.size());
    for(auto const& pair : m_gateMap){
        printf("%li\n", pair.first);
    }
*/
}

void ivc::PhysicalCreature::buildChildNodes(BaseNode* parentNode, PxVec3 parentPos, PxVec3 parentScale, PxRigidDynamic* parentBody, unsigned int recursionDepth) {

    auto parentHalfExtents = parentNode->getDimensions()/2;
    parentHalfExtents = PxVec3(parentHalfExtents.x * parentScale.x, parentHalfExtents.y * parentScale.y, parentHalfExtents.z * parentScale.z);

    auto childNodes = parentNode->getChildren();
    if(parentNode->getRecursionLimit() > recursionDepth)
        childNodes.push_back(parentNode);

    for(auto child : childNodes){

        PxVec3 scale = child->getScale();
        PxVec3 childScale = PxVec3(parentScale.x * scale.x, parentScale.y * scale.y, parentScale.z * scale.z);

        PxVec3 childSize = child->getHalfExtents();
        PxVec3 childHalfExtents = PxVec3(childSize.x * childScale.x, childSize.y * childScale.y, childSize.z * childScale.z);

        PxVec3 childRotation = child->getOrientation();

        PxVec3 parentVec = child->getParentAnchor();
        PxVec3 childVec;

        //TODO: ???
        if(parentVec.x == 1 || parentVec.x == -1){
            childVec = PxVec3(childHalfExtents.x * parentVec.x,0,0);
        }else if(parentVec.y == 1 || parentVec.y == -1){
            childVec = PxVec3(0,childHalfExtents.y * parentVec.y,0);
        }else{
            childVec = PxVec3(0,0,childHalfExtents.z * parentVec.z);
        }

        PxVec3 parentVecModified = PxVec3(parentVec.x * parentHalfExtents.x, parentVec.y * parentHalfExtents.y, parentVec.z * parentHalfExtents.z);

        PxVec3 childPos = parentPos + parentVecModified + childVec;

        auto childBody = createBox(childHalfExtents, childPos, childRotation);

        PxTransform parentTrans(parentVecModified);
        PxTransform childTrans(-1 * childVec);
        auto d6joint = PxD6JointCreate(*m_physics, parentBody, parentTrans, childBody, childTrans);

        d6joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
        d6joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
        d6joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

        PxSpring spring(SPRING_STIFFNESS, SPRING_DAMPING);     //TODO: changeable (?)
        auto swingY = child->getSwingLimitsY();
        auto swingZ = child->getSwingLimitsZ();
        auto twist = child->getTwistLimits();
        PxJointAngularLimitPair limits(twist.first, twist.second);
        PxJointLimitPyramid pyramid(swingY.first, swingY.second, swingZ.first, swingZ.second);
        d6joint->setPyramidSwingLimit(pyramid);
        d6joint->setTwistLimit(limits);
        PxD6JointDrive drive(SPRING_STIFFNESS, SPRING_DAMPING, FLT_MAX);
        d6joint->setDrive(PxD6Drive::eSLERP, drive);

        //collect neurons from node
        auto neuronVec = child->getLocalNeurons()->getCopyOfNeurons();
        m_neuronVector.insert(m_neuronVector.end(), neuronVec.begin(), neuronVec.end());

        //associate joint with sensor/effector
        auto pair = child->getLocalNeurons()->getCopiesOfJointNeurons();
        pair.first->setJoint(d6joint);
        pair.second->setJoint(d6joint);
        pair.second->calculateMaxStrength(parentHalfExtents*2,childHalfExtents*2);
        m_sensorVector.push_back(pair.first);
        m_effectorVector.push_back(pair.second);

        if(child == parentNode){
            buildChildNodes(child,childPos,childScale,childBody,recursionDepth+1);
        }else{
            buildChildNodes(child,childPos,childScale,childBody,0);
        }


    }

}

PxRigidDynamic* ivc::PhysicalCreature::createBox(PxVec3 halfextents, PxVec3 position, PxVec3 rotation) {

    PxShape* boxShape = m_physics->createShape(PxBoxGeometry(halfextents), *m_material);
    glm::quat glmQuat = glm::quat(glm::vec3(rotation.x,rotation.y,rotation.z));
    PxQuat physxQuat = PxQuat(glmQuat.x,glmQuat.y,glmQuat.z,glmQuat.w);
    PxTransform transform(position, physxQuat);
    PxRigidDynamic* body = m_physics->createRigidDynamic(transform);
    bodyParts.push_back(body);
    body->attachShape(*boxShape);
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    boxShape->release();

    return body;
}

std::vector<PxRigidDynamic *> ivc::PhysicalCreature::getBodies() {
    return bodyParts;
}

void ivc::PhysicalCreature::performBrainStep() {

    //calculate new values
    for(auto sensor : m_sensorVector){
        sensor->step();
    }

    for(auto neuron : m_neuronVector){
        neuron->step();
    }

    for(auto effector : m_effectorVector){
        effector->step();
    }

    //swap old and new values
    for(auto sensor : m_sensorVector){
        sensor->swap();
    }

    for(auto neuron : m_neuronVector){
        neuron->swap();
    }

}

PxVec3 ivc::PhysicalCreature::getPosition() {
    return bodyParts.front()->getGlobalPose().p;
}
