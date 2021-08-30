//
// Created by st on 7/16/21.
//

#include "PhysicsScene.h"

int ivc::PhysicsScene::init(PhysicsBase* base, RootMorphNode* rootNode) {

    m_base = base;

    PxSceneDesc sceneDesc(m_base->getPhysics()->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(0);

    m_creature = new PhysicalCreature(rootNode,PxVec3(0,ROOT_STARTING_HEIGHT,0), m_base);
    sceneDesc.simulationEventCallback = m_creature->getReporter();

    m_scene = m_base->getPhysics()->createScene(sceneDesc);
    createPlane(PxVec3(0,1,0), 0, m_base->getMaterial());

    m_scene->addArticulation(*m_creature->getArticulation());
    m_creature->initCache();

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

    delete m_creature;

    if(m_plane)
        m_plane->release();

    m_scene->release();
}

int ivc::PhysicsScene::simulate(bool brainSteps) {
    if(!isInitialized)
        return -1;

    m_scene->simulate(SIMULATION_STEP_SIZE);
    m_scene->fetchResults(true);

    m_creature->updateCache();
    m_creature->updateContactStates();

    if(brainSteps){
        for(int i = 0; i < BRAINSTEPS_PER_SIMSTEP; ++i){
            m_creature->performBrainStep();
        }
    }


    return 0;
}

PxRigidStatic *ivc::PhysicsScene::getPlane() {
    return m_plane;
}

PxVec3 ivc::PhysicsScene::getCreaturePos() {
    return m_creature->getPosition();
}

std::vector<PxArticulationLink *> ivc::PhysicsScene::getBodyParts() {
    return m_creature->getBodies();
}

void ivc::PhysicsScene::insertNewCreature(ivc::RootMorphNode* newNode) {

    auto lastPos = m_creature->getPosition();
    lastPos += PxVec3(0,0.1,0);

    delete m_creature;
    m_plane->release();
    m_scene->release();

    PxSceneDesc sceneDesc(m_base->getPhysics()->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(0);

    m_creature = new PhysicalCreature(newNode,lastPos, m_base);
    sceneDesc.simulationEventCallback = m_creature->getReporter();

    m_scene = m_base->getPhysics()->createScene(sceneDesc);
    createPlane(PxVec3(0,1,0), 0, m_base->getMaterial());

    m_scene->addArticulation(*m_creature->getArticulation());
    m_creature->initCache();

}
