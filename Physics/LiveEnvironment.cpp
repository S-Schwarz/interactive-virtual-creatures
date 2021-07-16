//
// Created by st on 7/16/21.
//

#include "LiveEnvironment.h"

int ivc::LiveEnvironment::init(ivc::PhysicsScene *scene) {

    m_scene = scene;

    return 0;
}

int ivc::LiveEnvironment::simulate() {
    m_scene->simulate(true);
    return 0;
}

std::vector<PxRigidDynamic *> ivc::LiveEnvironment::getBodyParts() {
    return m_scene->getRigidDynamics();
}

PxRigidStatic *ivc::LiveEnvironment::getFloorPlane() {
    return m_scene->getPlane();
}

void ivc::LiveEnvironment::destroy() {
    m_scene->destroy();
}
