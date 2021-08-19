//
// Created by st on 7/7/21.
//

#include "PhysicalCreature.h"

ivc::PhysicalCreature::PhysicalCreature(RootMorphNode* rootNode, PxVec3 pos, PhysicsBase* base) {

    m_position = pos;
    m_physics = base->getPhysics();
    m_material = base->getMaterial();
    reporter = new ContactReporter;

    m_articulation = m_physics->createArticulationReducedCoordinate();

    auto rootLink = createLink(nullptr,rootNode->getDimensions()/2, m_position, PxVec3(0,0,0));

    //add root and brain neurons
    auto rootVec = rootNode->getLocalNeurons()->getCopyOfNeurons();
    auto brainVec = rootNode->getBrain()->getCopyOfNeurons();
    m_neuronVector.insert(m_neuronVector.end(), rootVec.begin(), rootVec.end());
    m_neuronVector.insert(m_neuronVector.end(), brainVec.begin(), brainVec.end());

    //add root contact sensor
    auto contactSensor = rootNode->getLocalNeurons()->getCopyOfContactSensor();
    contactSensor->initContactVec();
    m_contactVector.push_back(contactSensor);
    addContactTriggers(rootLink,rootNode->getHalfExtents(),contactSensor);

    buildChildNodes(rootNode, m_position, PxVec3(1,1,1), PxVec3(0,0,0),rootLink);

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

    //and for contact Sensors
    for(auto contact : m_contactVector){
        auto outVec = contact->getOutputIDs();
        std::vector<Gate*> newGates;
        for(auto id : outVec){
            auto newGate = new Gate();
            newGates.push_back(newGate);
            m_gateMap[id] = newGate;
        }
        contact->setOutputGates(newGates);
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

void ivc::PhysicalCreature::buildChildNodes(BaseNode* parentNode, PxVec3 parentPos, PxVec3 parentScale, PxVec3 parentRotation, PxArticulationLink* parentLink) {

    auto parentHalfExtents = parentNode->getDimensions()/2;
    parentHalfExtents = PxVec3(parentHalfExtents.x * parentScale.x, parentHalfExtents.y * parentScale.y, parentHalfExtents.z * parentScale.z);

    auto childNodes = parentNode->getChildren();

    for(auto child : childNodes){

        PxVec3 scale = child->getScale();
        PxVec3 childScale = PxVec3(parentScale.x * scale.x, parentScale.y * scale.y, parentScale.z * scale.z);

        PxVec3 childSize = child->getHalfExtents();
        PxVec3 childHalfExtents = PxVec3(childSize.x * childScale.x, childSize.y * childScale.y, childSize.z * childScale.z);

        PxVec3 childRotation = child->getOrientation();
        childRotation += parentRotation;

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

        auto childLink = createLink(parentLink, childHalfExtents, childPos, childRotation);

        //add contactSensor
        auto contactSensor = child->getLocalNeurons()->getCopyOfContactSensor();
        contactSensor->initContactVec();
        m_contactVector.push_back(contactSensor);
        addContactTriggers(childLink,childHalfExtents,contactSensor);

        //add joint
        PxTransform parentTrans(parentVecModified);
        PxTransform childTrans(-1 * childVec);

        auto joint1 = static_cast<PxArticulationJointReducedCoordinate*>(childLink->getInboundJoint());

        joint1->setParentPose(parentTrans);
        joint1->setChildPose(childTrans);

        auto swingY = child->getSwingLimitsY();
        auto swingZ = child->getSwingLimitsZ();
        auto twist = child->getTwistLimits();

        joint1->setJointType(PxArticulationJointType::eSPHERICAL);

        joint1->setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eLIMITED);
        joint1->setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eLIMITED);
        joint1->setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eLIMITED);

        joint1->setLimit(PxArticulationAxis::eTWIST, twist.first, twist.second);
        joint1->setLimit(PxArticulationAxis::eSWING1, swingY.first, swingY.second);
        joint1->setLimit(PxArticulationAxis::eSWING2, swingZ.first, swingZ.second);

        auto dimA = parentHalfExtents*2;
        auto dimB = childHalfExtents*2;
        float maxStrength = 0;

        float volumeA = dimA.x * dimA.y * dimA.z;
        float volumeB = dimB.x * dimB.y * dimB.z;

        // TODO: choose smaller or larger ???
        if(volumeA > volumeB){
            maxStrength = volumeB * EFFECTOR_MAXIMUM_STRENGTH_FACTOR;
        }else{
            maxStrength = volumeA * EFFECTOR_MAXIMUM_STRENGTH_FACTOR;
        }

        joint1->setDrive(PxArticulationAxis::eTWIST, SPRING_STIFFNESS, SPRING_DAMPING, maxStrength, PxArticulationDriveType::eVELOCITY);
        joint1->setDrive(PxArticulationAxis::eSWING1, SPRING_STIFFNESS, SPRING_DAMPING, maxStrength, PxArticulationDriveType::eVELOCITY);
        joint1->setDrive(PxArticulationAxis::eSWING2, SPRING_STIFFNESS, SPRING_DAMPING, maxStrength, PxArticulationDriveType::eVELOCITY);

        //collect neurons from node
        auto neuronVec = child->getLocalNeurons()->getCopyOfNeurons();
        m_neuronVector.insert(m_neuronVector.end(), neuronVec.begin(), neuronVec.end());

        //associate joint with sensor/effector
        auto pair = child->getLocalNeurons()->getCopiesOfJointNeurons();
        pair.first->setLink(childLink);
        pair.second->setJoint(joint1);
        m_sensorVector.push_back(pair.first);
        m_effectorVector.push_back(pair.second);

        buildChildNodes(child,childPos,childScale,childRotation,childLink);
    }

}

PxArticulationLink* ivc::PhysicalCreature::createLink(PxArticulationLink* parent, PxVec3 halfextents, PxVec3 position, PxVec3 rotation) {

    PxShape* boxShape = m_physics->createShape(PxBoxGeometry(halfextents), *m_material);
    boxShape->setName("draw");
    glm::quat glmQuat = glm::quat(glm::vec3(rotation.x,rotation.y,rotation.z));
    PxQuat physxQuat = PxQuat(glmQuat.x,glmQuat.y,glmQuat.z,glmQuat.w);
    PxTransform transform(position, physxQuat);
    PxArticulationLink* link = m_articulation->createLink(parent,transform);
    bodyParts.push_back(link);
    link->attachShape(*boxShape);
    PxRigidBodyExt::updateMassAndInertia(*link, CREATURE_DENSITY);
    boxShape->release();

    return link;
}

std::vector<PxArticulationLink*> ivc::PhysicalCreature::getBodies() {
    return bodyParts;
}

void ivc::PhysicalCreature::performBrainStep() {

    //calculate new values
    for(auto sensor : m_sensorVector){
        sensor->step();
    }

    for(auto contact : m_contactVector){
        contact->step();
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

    for(auto contact : m_contactVector){
        contact->swap();
    }

    for(auto neuron : m_neuronVector){
        neuron->swap();
    }

}

PxVec3 ivc::PhysicalCreature::getPosition() {
    return bodyParts.front()->getGlobalPose().p;
}

void ivc::PhysicalCreature::updateContactStates() {

    auto contactMap = reporter->retrieveAndResetMap();

    for(auto const& pair : contactMap){
        std::string id_string = pair.first;
        auto state = pair.second;

        //std::cout << id_string << "  -  " << state << "\n";

        for(auto contactSensor : m_contactVector){
            if(contactSensor->contains(id_string)){

                if(state == PxPairFlag::eNOTIFY_TOUCH_FOUND)
                    contactSensor->setState(id_string,true);
                else if(state == PxPairFlag::eNOTIFY_TOUCH_LOST)
                    contactSensor->setState(id_string,false);

                break;
            }
        }

    }

}

void ivc::PhysicalCreature::addContactTriggers(PxArticulationLink* link, PxVec3 halfExtents, ContactSensor* contactSensor) {

    std::vector<std::pair<PxVec3, PxTransform>> triggerVec = {
            {PxVec3(0.9f * halfExtents.x, 0.1f * halfExtents.y, 0.9f * halfExtents.z), PxTransform(PxVec3(0,halfExtents.y,0))},
            {PxVec3(0.9f * halfExtents.x, 0.1f * halfExtents.y, 0.9f * halfExtents.z), PxTransform(PxVec3(0,-halfExtents.y,0))},
            {PxVec3(0.1f * halfExtents.x, 0.9f * halfExtents.y, 0.9f * halfExtents.z), PxTransform(PxVec3(halfExtents.x,0,0))},
            {PxVec3(0.1f * halfExtents.x, 0.9f * halfExtents.y, 0.9f * halfExtents.z), PxTransform(PxVec3(-halfExtents.x,0,0))},
            {PxVec3(0.9f * halfExtents.x, 0.9f * halfExtents.y, 0.1f * halfExtents.z), PxTransform(PxVec3(0,0,halfExtents.z))},
            {PxVec3(0.9f * halfExtents.x, 0.9f * halfExtents.y, 0.1f * halfExtents.z), PxTransform(PxVec3(0,0,-halfExtents.z))}
    };

    auto ids = contactSensor->getOutputIDs();

    for (auto pair : triggerVec) {
        PxShape* triggerShape = m_physics->createShape(PxBoxGeometry(pair.first), *m_material);
        triggerShape->setLocalPose(pair.second);
        triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
        triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
        link->attachShape(*triggerShape);

        auto id_string = std::to_string(ids.back());

        //TODO: ???
        const std::string::size_type size = id_string.size();
        char *buffer = new char[size + 1];
        memcpy(buffer, id_string.c_str(), size + 1);

        triggerShape->setName(buffer);
        ids.pop_back();
    }

    if(!ids.empty())
        throw std::logic_error("SOMETHING WENT WRONG");

}

ContactReporter *ivc::PhysicalCreature::getReporter() {
    return reporter;
}

ivc::PhysicalCreature::~PhysicalCreature() {

    for(auto neuron : m_neuronVector){
        delete neuron;
    }

    for(auto sensor : m_sensorVector){
        delete sensor;
    }

    for(auto effector : m_effectorVector){
        delete effector;
    }

    for(auto contact : m_contactVector){
        delete contact;
    }

    for(auto pair : m_gateMap){
        delete pair.second;
    }

    delete m_cache;
    m_articulation->release();

    delete reporter;

}

void ivc::PhysicalCreature::initCache() {

    m_cache = m_articulation->createCache();

    for(auto sensor : m_sensorVector){
        sensor->setCache(m_cache);
    }

}

void ivc::PhysicalCreature::updateCache() {
    m_articulation->copyInternalStateToCache(*m_cache, PxArticulationCache::ePOSITION);
}

PxArticulationReducedCoordinate *ivc::PhysicalCreature::getArticulation() {
    return m_articulation;
}
