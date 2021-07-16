//
// Created by st on 7/16/21.
//

#include "PhysicsScene.h"

int ivc::PhysicsScene::init(PhysicsBase* base, RootMorphNode rootNode) {

    m_base = base;

    PxSceneDesc sceneDesc(m_base->getPhysics()->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(0);

    m_scene = m_base->getPhysics()->createScene(sceneDesc);

    createPlane(PxVec3(0,1,0), 0, m_base->getMaterial());

    m_creature = new PhysicalCreature(rootNode,PxVec3(0,5,0), m_base->getPhysics());

    for(auto part : m_creature->getBodies()){
        m_scene->addActor(*part);
        m_rigidDynamicsVector.push_back(part);
    }

    isInitialized = true;

    return 0;
}

int ivc::PhysicsScene::createPlane(PxVec3 normalVec, float distance, PxMaterial *material) {
    if(m_scene == nullptr)
        return -1;

    PxVec3 normal = normalVec.getNormalized();
    PxRigidStatic* plane = PxCreatePlane(*m_base->getPhysics(), PxPlane(normal, distance), *material);
    m_plane = plane;
    m_scene->addActor(*plane);

    return 0;
}

void ivc::PhysicsScene::destroy() {
    if(!isInitialized)
        return;

    for(auto rigidDynamic : m_rigidDynamicsVector){
        rigidDynamic->release();
    }
    m_rigidDynamicsVector.clear();

    if(m_plane)
        m_plane->release();

    m_scene->release();
}

int ivc::PhysicsScene::simulate(bool brainSteps) {
    if(!isInitialized)
        return -1;

    m_scene->simulate(SIMULATION_STEP_SIZE);
    m_scene->fetchResults(true);

    if(brainSteps){
        for(int i = 0; i < BRAINSTEPS_PER_SIMSTEP; ++i){
            m_creature->performBrainStep();
        }
    }


    return 0;
}

std::vector<PxRigidDynamic *> ivc::PhysicsScene::getRigidDynamics() {
    return m_rigidDynamicsVector;
}

PxRigidStatic *ivc::PhysicsScene::getPlane() {
    return m_plane;
}

PxVec3 ivc::PhysicsScene::getCreaturePos() {
    return m_creature->getPosition();
}
