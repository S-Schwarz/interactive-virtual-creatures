//
// Created by st on 7/16/21.
//

#include "LiveEnvironment.h"

int ivc::LiveEnvironment::init(PhysicsBase* base, std::vector<std::pair<std::shared_ptr<BaseNode>,float>> nodeVec) {

    m_base = base;

    float bestDistance = -INFINITY;
    for(auto n : nodeVec){
        auto liveScene = new PhysicsScene();
        liveScene->init(m_base, n.first);
        m_sceneVec.push_back(liveScene);
        if(n.second > bestDistance){
            m_currentBest = n.first;
            m_bestScene = liveScene;
            bestDistance = n.second;
        }
    }

    std::vector<std::pair<PhysicsScene*,int>> stableVec;
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

std::vector<std::pair<std::vector<PxArticulationLink*>, bool>> ivc::LiveEnvironment::getBodyParts() {

    std::vector<std::pair<std::vector<PxArticulationLink*>, bool>> returnVec;

    for(auto scene : m_sceneVec){
        bool best = false;
        if(scene == m_bestScene)
            best = true;
        returnVec.push_back({scene->getBodyParts(),best});
    }

    return returnVec;

}

PxRigidStatic *ivc::LiveEnvironment::getFloorPlane() {
    return m_bestScene->getPlane();
}

void ivc::LiveEnvironment::destroy() {
    for(auto scene : m_sceneVec){
        scene->destroy();
    }
}

void ivc::LiveEnvironment::insertNewCreatures(std::vector<std::pair<std::shared_ptr<BaseNode>,float>> nodeVec) {

    float bestDistance = -INFINITY;
    if(m_sceneVec.size() >= nodeVec.size()){
        for(int i = 0; i < m_sceneVec.size(); ++i){
            if(i >= nodeVec.size()){
                m_sceneVec[i]->destroy();
            }else{
                m_sceneVec[i]->insertNewCreature(nodeVec[i].first);
            }
            if(nodeVec[i].second > bestDistance){
                m_currentBest = nodeVec[i].first;
                bestDistance = nodeVec[i].second;
            }
        }
        m_sceneVec.resize(nodeVec.size());
    }else{
        for(int i = 0; i < nodeVec.size(); ++i){
            if(i >= m_sceneVec.size()){
                auto liveScene = new PhysicsScene();
                liveScene->init(m_base, nodeVec[i].first);
                m_sceneVec.push_back(liveScene);
            }else{
                m_sceneVec[i]->insertNewCreature(nodeVec[i].first);
            }
            if(nodeVec[i].second > bestDistance){
                m_currentBest = nodeVec[i].first;
                bestDistance = nodeVec[i].second;
            }
        }
    }

    std::vector<std::pair<PhysicsScene*,int>> stableVec;
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

    std::vector<std::pair<PhysicsScene*,int>> stableVec;
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

ivc::PhysicalCreature *ivc::LiveEnvironment::getBestCreature() {
    return m_bestScene->getCreature();
}
