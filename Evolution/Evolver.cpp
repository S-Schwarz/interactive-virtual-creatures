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
    createNewGeneration();

    return 0;
}

void testCreatures(std::vector<ivc::PhysicsScene*> sceneVec, std::map<ivc::PhysicsScene*, std::pair<ivc::RootMorphNode*, float>> *mapPtr){

    for(auto scene : sceneVec){
        //simulate and score
        //settle in to stable position
        int stableSteps = 0;
        bool resting = false;
        for(int i = 0; i < FALL_DOWN_STEPS; ++i){
            if(stableSteps == 5){
                resting = true;
                break;
            }
            auto beforePos = scene->getCreaturePos();
            scene->simulate(false);
            auto afterPos = scene->getCreaturePos();
            if(beforePos == afterPos){
                ++stableSteps;
            }else{
                stableSteps = 0;
            }

        }

        if(!resting)
            continue;

        auto startPos = scene->getCreaturePos();

        if(startPos.y < 0)
            continue;


        int stepCount = 0;

        //check if stopped moving
        auto deltaPos = startPos;

        //check for max height
        float maxHeight = 0;

        //take score after every simulation step
        float fitness = 0;

        //start moving
        for(int i = 0; i < STEPS_PER_GENERATION; ++i){
            scene->simulate(true);

            ++stepCount;

            auto currentPos = scene->getCreaturePos();
            if(currentPos.y > maxHeight)
                maxHeight = currentPos.y;

            if(stepCount == AMOUNT_STEPS_DELTA){
                if(deltaPos == currentPos)
                    break;

                stepCount = 0;
                deltaPos = currentPos;
            }


        }

        if(maxHeight > MAX_ALLOWED_HEIGHT)
            continue;

        auto endPos = scene->getCreaturePos();
        auto distanceTravelled = startPos.z - endPos.z;
        auto swervingX = 0.2f * abs(startPos.x - endPos.x);
        fitness = (distanceTravelled - swervingX);

        //write result
        (*mapPtr)[scene] = {(*mapPtr)[scene].first,fitness};
    }

}

void ivc::Evolver::evolveNextGeneration() {

    void (*testFuncPtr)(std::vector<ivc::PhysicsScene*>, std::map<ivc::PhysicsScene*,std::pair<ivc::RootMorphNode*, float>>*) = testCreatures;

    printf("Size: %lu\n", m_sceneMap.size());

    //fitness test all creatures
    //divide scenes among threads
    std::vector<std::unique_ptr<std::thread>> allThreads;
    std::vector<std::vector<PhysicsScene*>> allScenes;
    for(int i = 0; i < m_numThreads; ++i){
        allScenes.push_back({});
    }

    unsigned int iter = 0;
    for(auto const& pair : m_sceneMap){
        auto scene = pair.first;
        allScenes[iter].push_back(scene);
        if(iter == m_numThreads - 1){
            iter = 0;
        }else{
            ++iter;
        }
    }

    for(auto sceneVec : allScenes){
        allThreads.push_back(std::unique_ptr<std::thread>(new std::thread(testFuncPtr,sceneVec,&m_sceneMap)));
    }

    for (auto&& thread : allThreads) {
        thread->join();
    }

    allThreads.clear();

    createNextGeneration();

}

ivc::RootMorphNode* ivc::Evolver::evolveNewCreature() {

    for(int i = 0; i < NUMBER_OF_GENERATIONS; ++i){
        printf("---------------------------\n");
        printf("GENERATION #%i:\n", i+1);
        evolveNextGeneration();
    }

    return m_currentBest;

}

void ivc::Evolver::createNextGeneration() {

    auto newData = new EvoData();
    newData->setGeneration(m_numberGenerations);
    auto scores = getAllScores();

    if(!scores.empty()){
        newData->calculateScoreData(getAllScores());
        m_dataVec.push_back(newData);
        printf("Best Score: %f\n", newData->getBestScore());
        m_currentBest = newData->getBestCreature();
    }

    if(newData->getBestScore() == 0 || scores.empty()){
        //create completely new generation
        printf("Creating new generation!\n");
        deleteLastGeneration({});
        m_sceneMap = {};
        createNewGeneration();
    }else{
        //create new generation
        auto nextGenMap = createNewGenerationFromParents(newData->getParentVec());

        //replace and delete old generation
        deleteLastGeneration(newData->getParents());
        m_sceneMap = nextGenMap;
    }

}

void ivc::Evolver::createNewGeneration() {
    for(int i = 0; i < CREATURES_PER_GENERATION; ++i){
        auto newRootNode = new RootMorphNode();
        newRootNode->init();
        newRootNode->addNeuralConnections();
        auto newScene = new PhysicsScene();
        newScene->init(m_base,newRootNode);
        m_sceneMap[newScene] = {newRootNode, -INFINITY};
    }
}

void ivc::Evolver::deleteLastGeneration(std::vector<RootMorphNode*> parents) {
    for(auto const& pair : m_sceneMap){
        pair.first->destroy();
        delete pair.first;

        bool keep = false;
        for(auto p : parents){
            if(pair.second.first == p){
                keep = true;
                break;
            }
        }
        if(!keep){
            if(pair.second.first == m_currentBest)
                m_currentBest = nullptr;
            delete pair.second.first;
        }

    }
}

std::map<ivc::PhysicsScene *, std::pair<ivc::RootMorphNode *, float>>
ivc::Evolver::createNewGenerationFromParents(std::vector<std::pair<RootMorphNode *, unsigned int>> amountPerParent) {

    std::map<PhysicsScene*, std::pair<RootMorphNode*, float>> nextGenMap;

    for(auto pair : amountPerParent){
        auto newScene = new PhysicsScene();
        newScene->init(m_base,pair.first);
        nextGenMap[newScene] = {pair.first, -INFINITY};
        for(int i = 0; i < pair.second; ++i){
            auto newRoot = dynamic_cast<RootMorphNode*>(pair.first->copy());
            std::random_device rd;
            std::mt19937 generator(rd());
            newRoot->setGenerator(&generator);
            newRoot->mutateBodyAndNeurons();
            newRoot->mutateNewBodyAndNewNeurons();
            newRoot->mutateNeuralConnections();
            newScene = new PhysicsScene();
            newScene->init(m_base,newRoot);
            nextGenMap[newScene] = {newRoot, -INFINITY};
        }
    }

    return nextGenMap;

}

void ivc::Evolver::startContinuousEvolution() {

    int i = 0;
    while(!m_pauseEvolution){
        printf("---------------------------\n");
        printf("GENERATION #%i:\n", ++i);
        evolveNextGeneration();
        m_numberGenerations += 1;
    }

}

void ivc::Evolver::stopEvolution() {
    m_pauseEvolution = true;
}

ivc::RootMorphNode *ivc::Evolver::getCurrentBest() {
    return m_currentBest;
}

unsigned int ivc::Evolver::getNumberGenerations() {
    return m_numberGenerations;
}

std::vector<std::pair<ivc::RootMorphNode*, float>> ivc::Evolver::getAllScores() {
    std::vector<std::pair<RootMorphNode*, float>> scoreVec;
    for(auto const& pair : m_sceneMap){
        auto score = pair.second.second;
        if(score != -INFINITY && score != INFINITY){
            scoreVec.push_back(pair.second);
        }
    }
    return scoreVec;
}

std::vector<ivc::EvoData*> ivc::Evolver::getEvoDataVec() {
    return m_dataVec;
}
