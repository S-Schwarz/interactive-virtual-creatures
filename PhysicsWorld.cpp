//
// Created by st on 6/25/21.
//

#include "PhysicsWorld.h"

int ivc::PhysicsWorld::init() {

    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_defaultAllocatorCallback, s_defaultErrorCallback);
    if (!m_foundation) {
        printf("PxCreateFoundation failed!");
        return -1;
    }

    bool recordMemoryAllocations = true;

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), recordMemoryAllocations,
                                          nullptr);
    if (!m_physics) {
        printf("PxCreatePhysics failed!");
        return -1;
    }

    PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(0);

    m_scene = m_physics->createScene(sceneDesc);

    PxMaterial* mMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

    PxVec3 normalVec(0, 1, 0);
    PxVec3 normal = normalVec.getNormalized();
    PxRigidStatic* plane = PxCreatePlane(*m_physics, PxPlane(normal, 0), *mMaterial);
    m_scene->addActor(*plane);

    PxShape* boxShape = m_physics->createShape(PxBoxGeometry(0.5, 0.5, 0.5), *mMaterial);
    PxTransform transform(PxVec3(0, 10, 0));
    PxRigidDynamic* body = m_physics->createRigidDynamic(transform);
    m_rigidBodiesVector.push_back(body);
    body->attachShape(*boxShape);
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    m_scene->addActor(*body);
    boxShape->release();

    isInitialized = true;

    return 0;

}

int ivc::PhysicsWorld::simulate() {

    if(!isInitialized)
        return -1;

    for (int i = 0; i < 10000; ++i) {
        m_scene->simulate(0.01f);
        m_scene->fetchResults(true);
    }

    return 0;

}

int ivc::PhysicsWorld::destroy() {

    if(!isInitialized)
        return -1;

    for(auto body : m_rigidBodiesVector){
        body->release();
    }
    m_rigidBodiesVector.clear();

    m_scene->release();
    m_physics->release();
    m_foundation->release();

    return 0;

}