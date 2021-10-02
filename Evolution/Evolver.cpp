//
// Created by st on 7/16/21.
//

#include "Evolver.h"

#include <memory>
#include "../Body/BaseNode.h"

void testCreatures(std::vector<std::shared_ptr<ivc::PhysicsScene>> sceneVec, std::map<std::shared_ptr<ivc::PhysicsScene>, std::pair<std::shared_ptr<ivc::BaseNode>, std::pair<PxVec3, PxVec3>>> *mapPtr, std::map<std::shared_ptr<ivc::BaseNode>, std::vector<PxVec3>>* noveltyArchive,bool recordNovelty, int stepsPG, int noveltyInterval){

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

int ivc::Evolver::init(ivc::PhysicsBase *base, EvoConfig* config) {

    m_base = base;
    m_config = config;

    //find number of available threads
    m_numThreads = std::thread::hardware_concurrency();
    printf("AVAILABLE THREADS: %i\n", m_numThreads);

    createFirstGeneration();

    return 0;
}

void ivc::Evolver::startContinuousEvolution() {

    while(m_config->m_paused){

    }
    while(!m_config->m_paused){
        printf("---------------------------\n");
        printf("GENERATION #%i:\n", m_numberGenerations);

        testCurrentGeneration();
        m_testSceneMap.clear();

        if(m_currentViableCreaturesVec.empty()){
            createFirstGeneration();
        }else{
            calcFitness();
            printf("Largest distance: %f\n", m_currentLargestDistance);
            chooseParents();
            createNewGenerationFromParents();
            createEvoData();

            m_currentViableCreaturesVec.clear();
            m_currentFitnessMap.clear();
            m_currentNoveltyMap.clear();

            m_numberGenerations += 1;
        }
    }

}

void ivc::Evolver::createFirstGeneration() {
    for(int i = 0; i < m_config->m_creaturesPerGeneration; ++i){
        auto newRootNode = std::make_shared<BaseNode>();
        newRootNode->init(true, nullptr, nullptr,m_config);
        auto newScene = std::make_shared<PhysicsScene>();
        newScene->init(m_base, newRootNode);
        m_testSceneMap[newScene] = {newRootNode, {PxVec3(-INFINITY, -INFINITY, -INFINITY), PxVec3(-INFINITY, -INFINITY, -INFINITY)}};
    }
}


void ivc::Evolver::createNewGenerationFromParents() {

    printf("Choose %zu parents from this generation\n", m_nextParentVec.size());

    for(const auto&[node, amount] : m_nextParentVec){
        auto newScene = std::make_shared<PhysicsScene>();
        newScene->init(m_base,node);
        m_testSceneMap[newScene] = {node, {PxVec3(0,0,0), PxVec3(0,0,0)}};
        for(int i = 0; i < amount; ++i){
            auto newRoot = node->copy();
            std::random_device rd;
            std::mt19937 generator(rd());
            newRoot->setGenerator(&generator);
            newRoot->mutateBodyAndNeurons(m_config->m_useNoveltySearch || m_config->m_lockMorph, m_config);
            newRoot->mutateNewBodyAndNewNeurons(m_config->m_useNoveltySearch || m_config->m_lockMorph, m_config);
            newRoot->mutateNeuralConnections(m_config);
            newScene = std::make_shared<PhysicsScene>();
            newScene->init(m_base,newRoot);
            m_testSceneMap[newScene] = {newRoot, {PxVec3(0,0,0), PxVec3(0,0,0)}};
        }
    }

}

void ivc::Evolver::stopEvolution() {
    m_config->m_paused = true;
}

std::vector<std::pair<std::shared_ptr<ivc::BaseNode>,float>> ivc::Evolver::getCurrentBestVector() {
    return m_currentBestVector;
}

unsigned int ivc::Evolver::getNumberGenerations() {
    return m_numberGenerations;
}

std::vector<std::shared_ptr<ivc::EvoData>> ivc::Evolver::getEvoDataVec() {
    return m_dataVec;
}

void ivc::Evolver::testCurrentGeneration() {

    void (*testFuncPtr)(std::vector<std::shared_ptr<PhysicsScene>>, std::map<std::shared_ptr<PhysicsScene>,std::pair<std::shared_ptr<BaseNode>, std::pair<PxVec3, PxVec3>>>*,std::map<std::shared_ptr<BaseNode>, std::vector<PxVec3>>*,bool, int, int) = testCreatures;

    printf("Size: %lu\n", m_testSceneMap.size());

    //fitness test all creatures
    //divide scenes among threads
    std::vector<std::unique_ptr<std::thread>> allThreads;
    std::vector<std::vector<std::shared_ptr<PhysicsScene>>> allScenes;
    allScenes.reserve(m_numThreads);
    for(int i = 0; i < m_numThreads; ++i){
        allScenes.emplace_back();
    }

    unsigned int iter = 0;
    for(auto const& pair : m_testSceneMap){
        auto scene = pair.first;
        allScenes[iter].push_back(scene);
        if(iter == m_numThreads - 1){
            iter = 0;
        }else{
            ++iter;
        }
    }

    int noveltyInterval = m_config->m_noveltyInterval;

    allThreads.reserve(allScenes.size());
    for(const auto& sceneVec : allScenes){
        allThreads.push_back(std::make_unique<std::thread>(testFuncPtr, sceneVec, &m_testSceneMap, &m_currentGenNoveltyArchive,m_config->m_useNoveltySearch, m_config->m_stepsPerGeneration, noveltyInterval));
    }

    for (auto&& thread : allThreads) {
        thread->join();
    }

    allThreads.clear();

    //choose viable creatures
    for(const auto &[scene, pair] : m_testSceneMap){
        auto node = pair.first;
        auto posPair = pair.second;
        if(posPair.first.x != -INFINITY) {
            m_currentViableCreaturesVec.push_back(pair);
        }
    }

}

void ivc::Evolver::calcFitness() {

    m_currentLargestDistance = -INFINITY;
    m_currentBestFitnessScore = -INFINITY;
    m_currentWorstFitnessScore = INFINITY;
    // normal fitness function
    auto sideMP = m_config->m_useSidewaysMP ? m_config->m_sidewaysMultiplier : 0.0f;
    for(auto const& [baseNode, posPair] : m_currentViableCreaturesVec){
        auto startPos = posPair.first;
        auto endPos = posPair.second;

        // calc distance travelled
        auto distanceTravelled = startPos.z - endPos.z;
        if(distanceTravelled > m_currentLargestDistance){
            m_currentLargestDistance = distanceTravelled;
        }

        // is this creature better than the current best creatures?
        if(m_currentBestVector.size() < m_config->m_numberDisplayedCreatures){
                m_currentBestVector.emplace_back(baseNode, distanceTravelled);
        }else{
            int toReplaceIndex = -1;
            float worstDistance = INFINITY;
            for(int i = 0; i < m_currentBestVector.size(); ++i){
                float oldDistance = m_currentBestVector[i].second;
                if(distanceTravelled > oldDistance && worstDistance > oldDistance){
                    toReplaceIndex = i;
                    worstDistance = oldDistance;
                }
            }
            if(toReplaceIndex > -1){
                m_currentBestVector[toReplaceIndex] = {baseNode, distanceTravelled};
            }
        }

        // calculate and save fitness
        auto swervingX = sideMP * abs(startPos.x - endPos.x);
        float fitness = (distanceTravelled - swervingX);

        if(fitness > m_currentBestFitnessScore){
            m_currentBestFitnessScore = fitness;
        }
        if(fitness < m_currentWorstFitnessScore){
            m_currentWorstFitnessScore = fitness;
        }

        m_currentFitnessMap[baseNode] = fitness;
    }

}

void ivc::Evolver::calcNovelty() {

    // copy current gen into overall archive
    for(auto const& [baseNode, noveltyVec] : m_currentGenNoveltyArchive){
        m_noveltyArchive.push_back(noveltyVec);
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
                if(m_config->m_useXAxis)
                    diff_back += std::sqrt(std::pow((noveltyVec.back().x - noveltyVecNeighbor.back().x),2));
                if(m_config->m_useZAxis)
                    diff_back += std::sqrt(std::pow((noveltyVec.back().z - noveltyVecNeighbor.back().z),2));
                diff = std::sqrt(diff_back);
            }else{
                for(int i = 0; i < noveltyVec.size(); ++i){
                    auto posVec = noveltyVec[i];
                    auto posVecNeighbor = noveltyVecNeighbor[i];

                    float diff_i = 0;
                    if(m_config->m_useXAxis)
                        diff_i += std::pow((posVec.x - posVecNeighbor.x),2);
                    if(m_config->m_useZAxis)
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

        m_currentNoveltyMap[baseNode] = averageDistance;

    }

    m_currentGenNoveltyArchive.clear();

}

void ivc::Evolver::chooseParents() {

    // calculate average score
    float scoreSum = 0;
    for(const auto &[node, score] : m_currentFitnessMap){
        scoreSum += score;
    }
    m_currentAverageFitnessScore = scoreSum / m_currentFitnessMap.size();

    // normalize scores
    std::vector<std::pair<std::shared_ptr<BaseNode>, float>> normalizedScores;
    for(const auto &[node, score] : m_currentFitnessMap){
        if(m_currentBestFitnessScore == m_currentWorstFitnessScore){
            normalizedScores.emplace_back(node,1.0f);
        }else{
            normalizedScores.emplace_back(node, Mutator::normalize(score, m_currentWorstFitnessScore, m_currentBestFitnessScore));
        }
    }

    //choose best creatures
    std::vector<std::pair<std::shared_ptr<BaseNode>,float>> bestVec;
    for(const auto&[node, score] : normalizedScores){
        if(score > EVOLUTION_MIN_SCORE){
            bestVec.emplace_back(node,score);
        }
    }

    //sort by score
    std::sort(bestVec.begin(), bestVec.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });

    //cap number of parents
    if(bestVec.size() > EVOLUTION_MAX_PARENTS){
        bestVec.resize(EVOLUTION_MAX_PARENTS);
    }

    //choose amount of children per root
    std::vector<std::pair<std::shared_ptr<BaseNode>,unsigned int>> amountVec;
    float total = 0;
    for(const auto&[node, score] : bestVec){
        total += score;
    }
    float partSize = m_config->m_creaturesPerGeneration / total;

    for(const auto&[node, score] : bestVec){
        auto amountChildren = std::max(1.0f, floor(score * partSize));
        amountVec.emplace_back(node, amountChildren);
    }

    m_nextParentVec = amountVec;

}

void ivc::Evolver::createEvoData() {

    auto newData = std::make_shared<EvoData>();
    newData->setGeneration(m_numberGenerations);
    newData->setLargestDistance(m_currentLargestDistance);
    newData->setBestFitnessScore(m_currentBestFitnessScore);
    newData->setWorstFitnessScore(m_currentWorstFitnessScore);
    newData->setAverageFitnessScore(m_currentAverageFitnessScore);
    m_dataVec.push_back(newData);

}
