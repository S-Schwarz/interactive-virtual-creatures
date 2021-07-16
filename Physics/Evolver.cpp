//
// Created by st on 7/16/21.
//

#include "Evolver.h"

int ivc::Evolver::init(ivc::PhysicsBase *base) {

    m_base = base;

    //find number of available threads
    m_numThreads = std::thread::hardware_concurrency();
    printf("AVAILABLE THREADS: %i\n", m_numThreads);

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

void testCreatures(std::vector<ivc::PhysicsScene*> sceneVec, std::map<ivc::PhysicsScene*, std::pair<ivc::RootMorphNode*, float>> *mapPtr){

    for(auto scene : sceneVec){
        //simulate and score
        auto startPos = scene->getCreaturePos();
        for(int i = 0; i < STEPS_PER_GENERATION; ++i){
            scene->simulate();
        }
        auto endPos = scene->getCreaturePos();
        float fitness = startPos.z - endPos.z;

        //write result
        (*mapPtr)[scene] = {(*mapPtr)[scene].first,fitness};
    }

}

void ivc::Evolver::evolveNextGeneration() {

    void (*testFuncPtr)(std::vector<ivc::PhysicsScene*>, std::map<ivc::PhysicsScene*,std::pair<ivc::RootMorphNode*, float>>*) = testCreatures;

    //fitness test all creatures
    //divide scenes among threads
    std::vector<std::unique_ptr<std::thread>> allThreads;
    std::vector<std::vector<PhysicsScene*>> allScenes;
    for(int i = 0; i < m_numThreads; ++i){
        allScenes.push_back({});
    }

    unsigned int iter = 0;
    for(auto const& pair : sceneMap){
        auto scene = pair.first;
        allScenes[iter].push_back(scene);
        if(iter == m_numThreads - 1){
            iter = 0;
        }else{
            ++iter;
        }
    }

    for(auto sceneVec : allScenes){
        allThreads.push_back(std::unique_ptr<std::thread>(new std::thread(testFuncPtr,sceneVec,&sceneMap)));
    }

    for (auto&& thread : allThreads) {
        thread->join();
    }

    allThreads.clear();

    //find best creature
    RootMorphNode* bestCreature = nullptr;
    float bestScore = -INFINITY;
    for(auto const& pair : sceneMap){
        auto score = pair.second.second;
        if(score > bestScore){
            bestCreature = pair.second.first;
            bestScore = score;
        }
    }
    currentBest = bestCreature;
    printf("BEST FITNESS SCORE: %f\n", bestScore);

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
        pair.first->destroy();
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
