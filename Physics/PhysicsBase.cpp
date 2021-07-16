//
// Created by st on 7/16/21.
//

#include "PhysicsBase.h"

int ivc::PhysicsBase::init() {
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_defaultAllocatorCallback, s_defaultErrorCallback);
    if (!m_foundation) {
        printf("PxCreateFoundation failed!");
        return -1;
    }

    bool recordMemoryAllocations = true;

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), recordMemoryAllocations,nullptr);
    if (!m_physics) {
        printf("PxCreatePhysics failed!");
        return -1;
    }

    m_material = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

    isInitialized = true;

    return 0;

}

PxPhysics *ivc::PhysicsBase::getPhysics() {
    return m_physics;
}

PxMaterial *ivc::PhysicsBase::getMaterial() {
    return m_material;
}

void ivc::PhysicsBase::destroy() {
    if(!isInitialized)
        return;

    m_physics->release();
    m_foundation->release();
}
