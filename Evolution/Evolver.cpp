//
// Created by st on 7/16/21.
//

#include "Evolver.h"
#include "../Body/BaseNode.h"

int ivc::Evolver::init(ivc::PhysicsBase *base, EvoConfig* config) {

    m_base = base;
    m_config = config;

    //find number of available threads
    m_numThreads = std::thread::hardware_concurrency();
    printf("AVAILABLE THREADS: %i\n", m_numThreads);

    // create first generation
    createNewGeneration();

    return 0;
}

void testCreatures(std::vector<ivc::PhysicsScene*> sceneVec, std::map<ivc::PhysicsScene*, std::pair<ivc::BaseNode*, std::pair<PxVec3, PxVec3>>> *mapPtr, std::map<ivc::BaseNode*, std::vector<PxVec3>>* noveltyArchive, int stepsPG, int noveltyInterval){

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

        //create novelty vec
        std::vector<PxVec3> noveltyVec;

        //start moving
        for(int i = 0; i < stepsPG; ++i){
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

            if( i % noveltyInterval == 0){
                float dX = std::pow((currentPos.x - startPos.x), 2) * ( (currentPos.x - startPos.x) < 0 ? -1 : 1 );
                float dY = std::pow((currentPos.y - startPos.y), 2) * ( (currentPos.y - startPos.y) < 0 ? -1 : 1 );
                float dZ = std::pow((currentPos.z - startPos.z), 2) * ( (currentPos.z - startPos.z) < 0 ? -1 : 1 );

                noveltyVec.push_back(PxVec3(dX,dY,dZ));
            }


        }

        if(maxHeight > MAX_ALLOWED_HEIGHT)
            continue;

        auto endPos = scene->getCreaturePos();

        int goalLength = std::floor(stepsPG / noveltyInterval) + 1;
        auto lastDelta = noveltyVec.back();
        while(noveltyVec.size() < goalLength){
            noveltyVec.push_back(lastDelta);
        }

        //write result
        (*mapPtr)[scene] = {(*mapPtr)[scene].first,{startPos,endPos}};
        (*noveltyArchive)[(*mapPtr)[scene].first] = noveltyVec;
    }

}

void ivc::Evolver::evolveNextGeneration() {

    void (*testFuncPtr)(std::vector<ivc::PhysicsScene*>, std::map<ivc::PhysicsScene*,std::pair<ivc::BaseNode*, std::pair<PxVec3, PxVec3>>>*,std::map<BaseNode*, std::vector<PxVec3>>*, int, int) = testCreatures;

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

    int noveltyInterval = m_config->m_noveltyInterval;

    for(auto sceneVec : allScenes){
        allThreads.push_back(std::unique_ptr<std::thread>(new std::thread(testFuncPtr,sceneVec,&m_sceneMap, &m_currentGenNoveltyArchive, m_config->m_stepsPerGeneration, noveltyInterval)));
    }

    for (auto&& thread : allThreads) {
        thread->join();
    }

    allThreads.clear();

    calcFitness();
    createNextGeneration();

}

ivc::BaseNode* ivc::Evolver::evolveNewCreature() {

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
    newData->setLargestDistance(m_largestDistanceTravelled);
    m_largestDistanceTravelled = -INFINITY;
    auto fitnessScores = getAllFitnessScores();
    auto noveltyScores = getAllNoveltyScores();

    if(!fitnessScores.empty()){
        newData->calculateScoreData(fitnessScores, m_config->m_creaturesPerGeneration, m_config->m_forceDiversity, noveltyScores, m_config->m_useNoveltySearch);
        m_dataVec.push_back(newData);
        printf("Largest Distance: %f\n", newData->getLargestDistance());
        m_currentBest = newData->getBestCreature();
    }

    if(newData->getBestFitnessScore() == 0 || fitnessScores.empty()){
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
    for(int i = 0; i < m_config->m_creaturesPerGeneration; ++i){
        auto newRootNode = new BaseNode();
        newRootNode->init(true, nullptr, nullptr);
        newRootNode->addNeuralConnections();
        auto newScene = new PhysicsScene();
        newScene->init(m_base,newRootNode);
        m_sceneMap[newScene] = {newRootNode, {PxVec3(0,0,0),PxVec3(0,0,0)}};
    }
}

void ivc::Evolver::deleteLastGeneration(std::vector<BaseNode*> parents) {
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
    m_fitnessMap = {};
    m_noveltyMap = {};
}

std::map<ivc::PhysicsScene *, std::pair<ivc::BaseNode *, std::pair<PxVec3, PxVec3>>>
ivc::Evolver::createNewGenerationFromParents(std::vector<std::pair<BaseNode *, unsigned int>> amountPerParent) {

    std::map<PhysicsScene*, std::pair<BaseNode*, std::pair<PxVec3, PxVec3>>> nextGenMap;

    for(auto pair : amountPerParent){
        auto newScene = new PhysicsScene();
        newScene->init(m_base,pair.first);
        nextGenMap[newScene] = {pair.first, {PxVec3(0,0,0), PxVec3(0,0,0)}};
        for(int i = 0; i < pair.second; ++i){
            auto newRoot = dynamic_cast<BaseNode*>(pair.first->copy());
            std::random_device rd;
            std::mt19937 generator(rd());
            newRoot->setGenerator(&generator);
            newRoot->mutateBodyAndNeurons();
            newRoot->mutateNewBodyAndNewNeurons();
            newRoot->mutateNeuralConnections();
            newScene = new PhysicsScene();
            newScene->init(m_base,newRoot);
            nextGenMap[newScene] = {newRoot, {PxVec3(0,0,0), PxVec3(0,0,0)}};
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

ivc::BaseNode*ivc::Evolver::getCurrentBest() {
    return m_currentBest;
}

unsigned int ivc::Evolver::getNumberGenerations() {
    return m_numberGenerations;
}

std::vector<std::pair<ivc::BaseNode*, float>> ivc::Evolver::getAllFitnessScores() {
    std::vector<std::pair<BaseNode*, float>> scoreVec;
    for(auto const& [key, val] : m_fitnessMap){
        scoreVec.push_back({key, val});
    }
    return scoreVec;
}

std::vector<ivc::EvoData*> ivc::Evolver::getEvoDataVec() {
    return m_dataVec;
}

void ivc::Evolver::calcFitness() {

    // copy current gen into overall archive
    for(auto const& [baseNode, noveltyVec] : m_currentGenNoveltyArchive){
        m_noveltyArchive.push_back(noveltyVec);
    }

    // normal fitness function
    auto sideMP = m_config->m_useSidewaysMP ? m_config->m_sidewaysMultiplier : 0.0f;
    m_largestDistanceTravelled = -INFINITY;
    for(auto const& [key, val] : m_sceneMap){
        auto baseNode = val.first;
        auto startPos = val.second.first;
        auto endPos = val.second.second;

        auto distanceTravelled = startPos.z - endPos.z;
        if(distanceTravelled > m_largestDistanceTravelled)
            m_largestDistanceTravelled = distanceTravelled;
        auto swervingX = sideMP * abs(startPos.x - endPos.x);
        float fitness = (distanceTravelled - swervingX);

        m_fitnessMap[baseNode] = fitness;
    }

    //remove unfit creatures
    for(auto const& [key, val] : m_sceneMap) {
        auto baseNode = val.first;

        if (m_fitnessMap[baseNode] == INFINITY || m_fitnessMap[baseNode] == -INFINITY) {
            m_fitnessMap.erase(baseNode);
        }
    }
    // novelty fitness
    // assign novelty score to current gen
    for(auto const& [baseNode, noveltyVec] : m_currentGenNoveltyArchive){
        std::vector<float> nearestNeighborsDifferenceVec;
        // skip itself
        bool skipped = false;
        for(auto noveltyVecNeighbor : m_noveltyArchive){
            if(!skipped && noveltyVecNeighbor == noveltyVec){
                skipped = true;
                continue;
            }
            float diff = 0;

            if(m_config->m_onlyUseEndPos){
                float diff_back = 0;
                diff_back += std::sqrt(std::pow((noveltyVec.back().x - noveltyVecNeighbor.back().x),2));
                diff_back += std::sqrt(std::pow((noveltyVec.back().z - noveltyVecNeighbor.back().z),2));
                diff = std::sqrt(diff_back);
            }else{
                for(int i = 0; i < noveltyVec.size(); ++i){
                    auto posVec = noveltyVec[i];
                    auto posVecNeighbor = noveltyVecNeighbor[i];

                    float diff_i = 0;
                    diff_i += std::pow((posVec.x - posVecNeighbor.x),2);
                    diff_i += std::pow((posVec.z - posVecNeighbor.z),2);

                    diff += std::sqrt(diff_i);
                }
            }

            if(nearestNeighborsDifferenceVec.size() < m_config->m_noveltyNearestNeighbors){
                nearestNeighborsDifferenceVec.push_back(diff);
            }else{
                for(int k = 0; k < nearestNeighborsDifferenceVec.size(); ++k){
                    if(nearestNeighborsDifferenceVec[k] > diff){
                        nearestNeighborsDifferenceVec[k] = diff;
                        break;
                    }
                }
            }
        }

        // calc average distance
        float sumDiff = 0;
        for(auto diff : nearestNeighborsDifferenceVec){
            sumDiff += diff;
        }

        float averageDistance = sumDiff / nearestNeighborsDifferenceVec.size();

        m_noveltyMap[baseNode] = averageDistance;

    }

    m_currentGenNoveltyArchive = {};

}

std::vector<std::pair<ivc::BaseNode *, float>> ivc::Evolver::getAllNoveltyScores() {
    std::vector<std::pair<BaseNode*, float>> noveltyVec;
    for(auto const& [key, val] : m_noveltyMap){
        noveltyVec.push_back({key, val});
    }
    return noveltyVec;
}
