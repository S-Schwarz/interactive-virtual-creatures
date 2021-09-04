//
// Created by st on 7/16/21.
//

#include "PhysicsScene.h"

int ivc::PhysicsScene::init(PhysicsBase* base, BaseNode* rootNode) {

    m_base = base;
    m_rootNode = rootNode;

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

    m_isInitialized = true;

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
    if(!m_isInitialized)
        return;

    delete m_creature;

    if(m_plane)
        m_plane->release();

    m_scene->release();
}

int ivc::PhysicsScene::simulate(bool brainSteps) {
    if(!m_isInitialized)
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

void ivc::PhysicsScene::insertNewCreature(ivc::BaseNode* newNode) {

    m_rootNode = newNode;
    //auto lastPos = m_creature->getPosition();
    //lastPos += PxVec3(0,0.5,0);

    /*auto oldCreature = m_creature;
    auto oldCache = m_creature->getCache();
    m_plane->release();
    m_scene->release();*/

    PxSceneDesc sceneDesc(m_base->getPhysics()->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(0);

    m_creature = new PhysicalCreature(newNode,PxVec3(0,ROOT_STARTING_HEIGHT,0), m_base);
    sceneDesc.simulationEventCallback = m_creature->getReporter();

    m_scene = m_base->getPhysics()->createScene(sceneDesc);
    createPlane(PxVec3(0,1,0), 0, m_base->getMaterial());

    m_scene->addArticulation(*m_creature->getArticulation());
    m_creature->initCache();

    // set position and rotation of new creature equal to old creature

    /*auto newRoot = m_creature->getRootLink();
    auto oldRoot = oldCreature->getRootLink();

    newRoot->setGlobalPose(oldRoot->getGlobalPose());

    PxArticulationLink* newChildrenArray[newRoot->getNbChildren()];
    newRoot->getChildren(newChildrenArray,newRoot->getNbChildren());
    std::vector<PxArticulationLink*> newChildren(newChildrenArray, newChildrenArray + newRoot->getNbChildren());

    PxArticulationLink* oldChildrenArray[oldRoot->getNbChildren()];
    newRoot->getChildren(oldChildrenArray,oldRoot->getNbChildren());
    std::vector<PxArticulationLink*> oldChildren(oldChildrenArray, oldChildrenArray + oldRoot->getNbChildren());

    alignChildren(oldChildren,newChildren, oldCache, m_creature->getCache());
*/
}

void ivc::PhysicsScene::alignChildren(std::vector<PxArticulationLink*> oldVec, std::vector<PxArticulationLink*> newVec, PxArticulationCache* oldCache, PxArticulationCache* newCache) {

    for(auto newLink : newVec){
        auto parentSide = newLink->getName();
        for(auto oldLink : oldVec){
            if(oldLink->getName() == parentSide){
                auto oldIndex = oldLink->getLinkIndex();
                PxVec3 pos = PxVec3(oldCache->jointPosition[oldIndex],oldCache->jointPosition[oldIndex+1],oldCache->jointPosition[oldIndex+2]);
                auto newIndex = newLink->getLinkIndex();

                newCache->jointPosition[newIndex] = pos.x;
                newCache->jointPosition[newIndex+1] = pos.y;
                newCache->jointPosition[newIndex+2] = pos.z;

                auto newArticulation = m_creature->getArticulation();
                newArticulation->applyCache(*newCache,PxArticulationCache::ePOSITION);

                PxArticulationLink* newChildrenArray[newLink->getNbChildren()];
                newLink->getChildren(newChildrenArray,newLink->getNbChildren());
                std::vector<PxArticulationLink*> newChildren(newChildrenArray, newChildrenArray + newLink->getNbChildren());

                PxArticulationLink* oldChildrenArray[oldLink->getNbChildren()];
                oldLink->getChildren(oldChildrenArray,oldLink->getNbChildren());
                std::vector<PxArticulationLink*> oldChildren(oldChildrenArray, oldChildrenArray + oldLink->getNbChildren());

                alignChildren(oldChildren,newChildren,oldCache,newCache);
                break;
            }
        }
    }

}

void ivc::PhysicsScene::resetCreaturePosition() {

    rebuild();

}

void ivc::PhysicsScene::rebuild() {

    if(m_rootNode == nullptr)
        return;

    destroy();

    PxSceneDesc sceneDesc(m_base->getPhysics()->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(0);

    m_creature = new PhysicalCreature(m_rootNode,PxVec3(0,ROOT_STARTING_HEIGHT,0), m_base);
    sceneDesc.simulationEventCallback = m_creature->getReporter();

    m_scene = m_base->getPhysics()->createScene(sceneDesc);
    createPlane(PxVec3(0,1,0), 0, m_base->getMaterial());

    m_scene->addArticulation(*m_creature->getArticulation());
    m_creature->initCache();

}

ivc::PhysicalCreature *ivc::PhysicsScene::getCreature() {
    return m_creature;
}
