//
// Created by st on 7/16/21.
//

#include "LiveEnvironment.h"

int ivc::LiveEnvironment::init(std::shared_ptr<PhysicsBase> base, std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>> nodeVec, std::shared_ptr<EvoConfig> config) {

    m_base = base;
    m_config = config;

    float bestDistance = -INFINITY;
    for(const auto& [node, pair] : nodeVec){
        auto liveScene = std::make_shared<PhysicsScene>();
        liveScene->init(m_base, node, m_config);
        m_sceneVec.push_back(liveScene);
        if(pair.first > bestDistance){
            m_currentBest = node;
            m_currentBestPath = pair.second;
            m_bestScene = liveScene;
            bestDistance = pair.first;
        }
    }

    std::vector<std::pair<std::shared_ptr<PhysicsScene>,int>> stableVec;
    for(auto scene : m_sceneVec){
        stableVec.push_back({scene,0});
    }

    //settle in to stable position
    for(int f = 0; f < FALL_DOWN_STEPS; ++f){
        for(int i = 0; i < stableVec.size(); ++i){
            if(stableVec[i].second == 5)
                continue;
            auto beforePos = stableVec[i].first->getCreaturePos();
            stableVec[i].first->simulate(false);
            auto afterPos = stableVec[i].first->getCreaturePos();
            if(beforePos == afterPos){
                stableVec[i] = {stableVec[i].first,stableVec[i].second+1};
            }else{
                stableVec[i] = {stableVec[i].first,0};
            }
        }
    }

    return 0;
}

int ivc::LiveEnvironment::simulate() {

    for(auto scene : m_sceneVec){
        scene->simulate(true);
    }

    return 0;
}

std::vector<std::pair<std::vector<PxArticulationLink*>, std::vector<PxVec3>>> ivc::LiveEnvironment::getBodyParts() {

    std::vector<std::pair<std::vector<PxArticulationLink*>, std::vector<PxVec3>>> returnVec;

    for(auto scene : m_sceneVec){
        std::vector<PxVec3> path = {};
        if(scene == m_bestScene)
            path = m_currentBestPath;
        returnVec.emplace_back(scene->getBodyParts(),path);
    }

    return returnVec;

}

PxRigidStatic *ivc::LiveEnvironment::getFloorPlane() {
    return m_bestScene->getPlane();
}

void ivc::LiveEnvironment::insertNewCreatures(std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>> nodeVec) {

    float bestDistance = -INFINITY;
    if(m_sceneVec.size() >= nodeVec.size()){
        for(int i = 0; i < m_sceneVec.size(); ++i){
            if(i >= nodeVec.size()){
                m_sceneVec[i]->destroy();
            }else{
                m_sceneVec[i]->insertNewCreature(nodeVec[i].first);
            }
            if(nodeVec[i].second.first > bestDistance){
                m_currentBest = nodeVec[i].first;
                m_currentBestPath = nodeVec[i].second.second;
                bestDistance = nodeVec[i].second.first;
            }
        }
        m_sceneVec.resize(nodeVec.size());
    }else{
        for(int i = 0; i < nodeVec.size(); ++i){
            auto liveScene = std::make_shared<PhysicsScene>();
            if(i >= m_sceneVec.size()){
                liveScene->init(m_base, nodeVec[i].first, m_config);
                m_sceneVec.push_back(liveScene);
            }else{
                m_sceneVec[i]->insertNewCreature(nodeVec[i].first);
            }
            if(nodeVec[i].second.first > bestDistance){
                m_currentBest = nodeVec[i].first;
                m_currentBestPath = nodeVec[i].second.second;
                m_bestScene = liveScene;
                bestDistance = nodeVec[i].second.first;
            }
        }
    }

    std::vector<std::pair<std::shared_ptr<PhysicsScene>,int>> stableVec;
    for(auto scene : m_sceneVec){
        stableVec.push_back({scene,0});
    }

    //settle in to stable position
    for(int f = 0; f < FALL_DOWN_STEPS; ++f){
        for(int i = 0; i < stableVec.size(); ++i){
            if(stableVec[i].second == 5)
                continue;
            auto beforePos = stableVec[i].first->getCreaturePos();
            stableVec[i].first->simulate(false);
            auto afterPos = stableVec[i].first->getCreaturePos();
            if(beforePos == afterPos){
                stableVec[i] = {stableVec[i].first,stableVec[i].second+1};
            }else{
                stableVec[i] = {stableVec[i].first,0};
            }
        }
    }

}

void ivc::LiveEnvironment::resetCreaturePosition() {

    std::vector<std::pair<std::shared_ptr<PhysicsScene>,int>> stableVec;
    for(auto scene : m_sceneVec){
        scene->resetCreaturePosition();
        stableVec.push_back({scene,0});
    }

    //settle in to stable position
    for(int f = 0; f < FALL_DOWN_STEPS; ++f){
        printf("F: %i\n", f);
        for(int i = 0; i < stableVec.size(); ++i){
            if(stableVec[i].second == 5)
                continue;
            auto beforePos = stableVec[i].first->getCreaturePos();
            stableVec[i].first->simulate(false);
            auto afterPos = stableVec[i].first->getCreaturePos();
            if(beforePos == afterPos){
                stableVec[i] = {stableVec[i].first,stableVec[i].second+1};
            }else{
                stableVec[i] = {stableVec[i].first,0};
            }
        }
        if(f == FALL_DOWN_STEPS-1)
            printf("TEST\n");
    }

}

std::shared_ptr<ivc::PhysicalCreature> ivc::LiveEnvironment::getBestCreature() {
    if(m_bestScene == nullptr)
        return nullptr;
    return m_bestScene->getCreature();
}

std::vector<PxRigidStatic *> ivc::LiveEnvironment::getObjVec() {
    return m_bestScene->getObjVec();
}

void ivc::LiveEnvironment::clean() {
    m_sceneVec = {};
    m_currentBest = nullptr;
    m_currentBestPath = {};
    m_bestScene = nullptr;
}
