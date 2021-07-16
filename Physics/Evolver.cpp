//
// Created by st on 7/16/21.
//

#include "Evolver.h"

int ivc::Evolver::init(ivc::PhysicsBase *base) {

    m_base = base;

    // create first generation
    for(int i = 0; i < CREATURES_PER_GENERATION; ++i){
        auto newRootNode = new RootMorphNode();
        newRootNode->init();
        newRootNode->addNeuralConnections();
        auto newScene = new PhysicsScene();
        newScene->init(m_base,*newRootNode);
        sceneMap[newScene] = {newRootNode, -INFINITY};
    }

    return 0;
}

void ivc::Evolver::evolveNextGeneration() {

    //fitness test all creatures
    for(auto const& pair : sceneMap){
        auto scene = pair.first;
        auto root = pair.second.first;

        auto startPos = scene->getCreaturePos();

        for(int i = 0; i < STEPS_PER_GENERATION; ++i){
            scene->simulate();
        }

        auto endPos = scene->getCreaturePos();

        float fitness = startPos.z - endPos.z;

        sceneMap[scene] = {root,fitness};
    }

    //find best creature
    RootMorphNode* bestCreature = nullptr;
    float bestScore = -INFINITY;
    for(auto const& pair : sceneMap){
        auto score = pair.second.second;
        if(score > bestScore){
            bestCreature = pair.second.first;
        }
    }
    currentBest = bestCreature;

    //create new generation
    std::map<PhysicsScene*, std::pair<RootMorphNode*, float>> nextGenMap;
    for(int i = 0; i < CREATURES_PER_GENERATION; ++i){
        auto newRoot = dynamic_cast<RootMorphNode*>(bestCreature->copy());
        std::random_device rd;
        std::mt19937 generator(rd());
        newRoot->setGenerator(&generator);
        newRoot->mutate();
        auto newScene = new PhysicsScene();
        newScene->init(m_base,*newRoot);
        nextGenMap[newScene] = {newRoot, -INFINITY};
    }

    //replace and delete old generation
    for(auto const& pair : sceneMap){
        delete pair.first;
        if(pair.second.first != currentBest)
            delete pair.second.first;
    }
    sceneMap = nextGenMap;

}

ivc::RootMorphNode* ivc::Evolver::evolveNewCreature() {

    for(int i = 0; i < NUMBER_OF_GENERATIONS; ++i){
        evolveNextGeneration();
    }

    return currentBest;

}
