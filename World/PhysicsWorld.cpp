//
// Created by st on 6/25/21.
//

#include "PhysicsWorld.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    createPlane(PxVec3(0,1,0), 0, mMaterial);
    createBox(PxVec3(0.5,0.5,0.5), PxVec3(2,3,0), PxVec3(45,0,0), mMaterial);
    createBox(PxVec3(0.5,0.5,0.5), PxVec3(0,3,0), PxVec3(0,45,0), mMaterial);
    createBox(PxVec3(0.5,0.5,0.5), PxVec3(-2,3,0), PxVec3(0,0,45), mMaterial);

    auto rootNode = RootMorphNode();
    rootNode.init();
    rootNode.addNeuralConnections();
    m_creature = new PhysicalCreature(rootNode, PxVec3(0,3,-5), m_physics);

    for(auto part : m_creature->getBodies()){
        m_scene->addActor(*part);
        m_rigidDynamicsVector.push_back(part);
    }

    isInitialized = true;

    return 0;

}

int ivc::PhysicsWorld::createBox(PxVec3 halfextents, PxVec3 position, PxVec3 rotation, PxMaterial* material) {
    if(m_scene == nullptr)
        return -1;

    PxShape* boxShape = m_physics->createShape(PxBoxGeometry(halfextents), *material);
    glm::quat glmQuat = glm::quat(glm::vec3(rotation.x,rotation.y,rotation.z));
    PxQuat physxQuat = PxQuat(glmQuat.x,glmQuat.y,glmQuat.z,glmQuat.w);
    PxTransform transform(position, physxQuat);
    PxRigidDynamic* body = m_physics->createRigidDynamic(transform);
    m_rigidDynamicsVector.push_back(body);
    body->attachShape(*boxShape);
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    m_scene->addActor(*body);
    boxShape->release();

    return 0;
}

int ivc::PhysicsWorld::createPlane(PxVec3 normalVec, float distance, PxMaterial* material) {
    if(m_scene == nullptr)
        return -1;

    PxVec3 normal = normalVec.getNormalized();
    PxRigidStatic* plane = PxCreatePlane(*m_physics, PxPlane(normal, distance), *material);
    m_plane = plane;
    m_scene->addActor(*plane);

    return 0;
}

int ivc::PhysicsWorld::simulate() {

    if(!isInitialized)
        return -1;

    m_scene->simulate(m_stepSize);
    m_scene->fetchResults(true);

    return 0;

}

int ivc::PhysicsWorld::destroy() {

    if(!isInitialized)
        return -1;

    for(auto rigidDynamic : m_rigidDynamicsVector){
        rigidDynamic->release();
    }
    m_rigidDynamicsVector.clear();

    if(m_plane)
        m_plane->release();

    m_scene->release();
    m_physics->release();
    m_foundation->release();

    return 0;

}

std::vector<PxRigidDynamic *> ivc::PhysicsWorld::getRigidDynamics() {

    return m_rigidDynamicsVector;

}

PxRigidStatic* ivc::PhysicsWorld::getPlane() {
    return m_plane;
}

float ivc::PhysicsWorld::getStepSize() {
    return m_stepSize;
}