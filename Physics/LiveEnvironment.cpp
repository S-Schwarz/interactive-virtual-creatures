//
// Created by st on 7/16/21.
//

#include "LiveEnvironment.h"

int ivc::LiveEnvironment::init(ivc::PhysicsScene *scene) {

    m_scene = scene;

    //settle in to stable position
    int stableSteps = 0;
    for(int i = 0; i < FALL_DOWN_STEPS; ++i){
        if(stableSteps == 5)
            break;
        auto beforePos = scene->getCreaturePos();
        m_scene->simulate(false);
        auto afterPos = scene->getCreaturePos();
        if(beforePos == afterPos){
            ++stableSteps;
        }else{
            stableSteps = 0;
        }
    }

    return 0;
}

int ivc::LiveEnvironment::simulate() {
    if(inactiveTime > 0){
        m_scene->simulate(false);
        --inactiveTime;
    }else{
        m_scene->simulate(true);
    }

    return 0;
}

std::vector<PxArticulationLink *> ivc::LiveEnvironment::getBodyParts() {
    return m_scene->getBodyParts();
}

PxRigidStatic *ivc::LiveEnvironment::getFloorPlane() {
    return m_scene->getPlane();
}

void ivc::LiveEnvironment::destroy() {
    m_scene->destroy();
}

void ivc::LiveEnvironment::insertNewCreature(ivc::RootMorphNode* newNode) {
    m_scene->insertNewCreature(newNode);
}

void ivc::LiveEnvironment::resetCreaturePosition() {

    m_scene->resetCreaturePosition();
    setInactiveTime(250);

}

void ivc::LiveEnvironment::setInactiveTime(unsigned int time) {
    inactiveTime = time;
}
