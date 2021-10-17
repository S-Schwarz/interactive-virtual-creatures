//
// Created by st on 7/16/21.
//

#include "Evolver.h"

#include <memory>
#include "../Body/BaseNode.h"

bool contains(PxVec3 creaturePos, glm::vec3 targetPos, glm::vec3 targetSize){
    if(!(creaturePos.z < targetPos.z + (targetSize.z/2) && creaturePos.z > targetPos.z - (targetSize.z/2)))
        return false;
    if(!(creaturePos.x < targetPos.x + (targetSize.x/2) && creaturePos.x > targetPos.x - (targetSize.x/2)))
        return false;
    if(!(creaturePos.y < targetPos.y + (targetSize.y/2) && creaturePos.y > targetPos.y - (targetSize.y/2)))
        return false;
    return true;
}

std::pair<glm::vec3, glm::vec3> reachedTarget(PxVec3 currentPos, std::vector<std::pair<glm::vec3, glm::vec3>> targetVec, std::vector<std::pair<glm::vec3, glm::vec3>> reachedTargetsVec){
    for(auto pair : targetVec){
        if(std::find(reachedTargetsVec.begin(), reachedTargetsVec.end(), pair) != reachedTargetsVec.end()){
            continue;
        }
        if(contains(currentPos, pair.first, pair.second)){
            return pair;
        }
    }
    return {glm::vec3(INFINITY, INFINITY, INFINITY), glm::vec3(INFINITY,INFINITY,INFINITY)};
}

// adapted from https://stackoverflow.com/questions/5254838/calculating-distance-between-a-point-and-a-rectangular-box-nearest-point
float calcDistance(PxVec3 cPos, glm::vec3 tPos, glm::vec3 tSize){
    float dX = std::max((tPos.x - tSize.x/2) - cPos.x, std::max(0.f, cPos.x - (tPos.x + tSize.x/2)));
    float dZ = std::max((tPos.z - tSize.z/2) - cPos.z, std::max(0.f, cPos.z - (tPos.z + tSize.z/2)));
    return std::sqrt(dX*dX + dZ*dZ);
}

float distanceToClosestTarget(PxVec3 cPos, std::vector<std::pair<glm::vec3, glm::vec3>> targetVec){
    float minDist = INFINITY;
    for(const auto [pos, size] : targetVec){
        auto dist = calcDistance(cPos, pos, size);
        if(dist < minDist)
            minDist = dist;
    }
    return minDist;
}

void testCreatures(std::vector<std::shared_ptr<ivc::PhysicsScene>> sceneVec, std::map<std::shared_ptr<ivc::BaseNode>, std::pair<std::vector<PxVec3>, int>>* posMapPtr, std::shared_ptr<ivc::EvoConfig> config){

    auto stepsPG = config->m_stepsPerGeneration;
    auto interval = config->m_noveltyInterval;
    auto useTarget = config->useTargetForEvo();
    auto targetVec = config->m_foodVec;
    auto useRewards = config->useRewardsForEvo();

    auto nullTarget = std::pair<glm::vec3, glm::vec3>(glm::vec3(INFINITY, INFINITY, INFINITY), glm::vec3(INFINITY,INFINITY,INFINITY));

    for(auto scene : sceneVec){

        unsigned int stepsToTarget = 0;
        bool hasReachedTarget = false;
        std::vector<std::pair<glm::vec3, glm::vec3>> reachedTargetsVec;
        //simulate and score
        //settle in to stable position
        int stableSteps = 0;
        bool resting = false;
        for(int i = 0; i < FALL_DOWN_STEPS; ++i){
            if(stableSteps == 20){
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

        auto currentTransform = scene->getCreature()->getRootLink()->getGlobalPose();
        PxTransform newTrans(PxVec3(0,currentTransform.p.y,0), currentTransform.q);
        scene->getCreature()->getArticulation()->teleportRootLink(newTrans, true);

        auto startPos = scene->getCreaturePos();

        if(startPos.y < 0)
            continue;

        int stepCount = 0;

        //check if stopped moving
        auto deltaPos = startPos;

        //check for max height
        float maxHeight = 0;

        //create novelty vec
        std::vector<PxVec3> posRecordVec;
        posRecordVec.push_back(startPos);
        //start moving
        for(int i = 0; i < stepsPG; ++i){
            scene->simulate(true);

            ++stepCount;
            ++stepsToTarget;

            PxVec3 currentPos = scene->getCreaturePos();

            if( i % interval == 0){
                posRecordVec.push_back(currentPos);
            }

            if(useTarget){
                auto reached = reachedTarget(currentPos, targetVec, reachedTargetsVec);
                if(reached != nullTarget){
                    hasReachedTarget = true;
                    break;
                }
            }else if(useRewards){
                auto reached = reachedTarget(currentPos, targetVec, reachedTargetsVec);
                if(reached != nullTarget) {
                    reachedTargetsVec.push_back(reached);
                }
            }

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

        int goalLength = std::floor(stepsPG / interval) + 1;
        auto lastPos = posRecordVec.back();
        while(posRecordVec.size() < goalLength){
            posRecordVec.push_back(lastPos);
        }

        //write result
        if(useTarget){
            //printf("SPG: %i STT: %u TARGET FITNESS: %i\n", stepsPG, stepsToTarget, stepsPG - stepsToTarget);
            if(hasReachedTarget){
                (*posMapPtr)[scene->getRootNode()] = {posRecordVec, stepsPG - stepsToTarget};
            }
            else{
                auto cPos = posRecordVec.back();
                float distance = distanceToClosestTarget(cPos, targetVec);
                (*posMapPtr)[scene->getRootNode()] = {posRecordVec, - (int)distance};
            }

        }else{
            (*posMapPtr)[scene->getRootNode()] = {posRecordVec, reachedTargetsVec.size()};
        }

    }

}

int ivc::Evolver::init(std::shared_ptr<ivc::PhysicsBase> base, std::shared_ptr<EvoConfig> config) {

    m_base = base;
    m_config = config;

    //find number of available threads
    m_numThreads = std::thread::hardware_concurrency();
    printf("AVAILABLE THREADS: %i\n", m_numThreads);

    return 0;
}

void ivc::Evolver::startContinuousEvolution() {

    while(!m_config->m_shouldEnd){
        testCurrentGeneration();
        calcDistanceTravelled();
        calcStats();

        if(m_currentViableCreaturesVec.empty() || m_currentLargestDistance == 0){
            m_testSceneVec = {};
            createFirstGeneration();
        }else{
            calcFitness();
            calcNovelty();
            m_testPosMap.clear();
            printf("---------------------------\n");
            printf("GENERATION #%i:\n", m_numberGenerations);
            printf("Size: %lu\n", m_testSceneVec.size());
            m_testSceneVec = {};
            printf("Largest distance: %f\n", m_currentLargestDistance);
            //printf("NEURONS OVERALL: %u\n", m_currentBestVector[0].first->getNeuronActivity()[0]);
            //printf("NEURONS BRAIN: %u\n", m_currentBestVector[0].first->getNeuronActivity()[1]);
            //printf("JOINT SENSORS: %u\n", m_currentBestVector[0].first->getNeuronActivity()[2]);
            //printf("CONTACT SENSORS: %u\n", m_currentBestVector[0].first->getNeuronActivity()[3]);
            chooseParents();
            createNewGenerationFromParents();
            createEvoData();

            m_currentViableCreaturesVec = {};
            m_currentFitnessMap = {};
            m_currentNoveltyMap = {};

            if(!m_config->m_useNoveltySearch)
                m_noveltyArchive = {};

            m_numberGenerations += 1;
        }

        if(m_numberGenerations-1 == 103){
            m_config->m_shouldSave = true;
            m_config->m_shouldEnd = true;
            m_config->numberOfRuns += 1;
            m_config->m_fileName = "run" + std::to_string(m_config->numberOfRuns);
        }
    }

}

void ivc::Evolver::createFirstGeneration() {
    printf("Generating new Population\n");
    m_config->m_mutChance *= 3;
    for(int i = 0; i < m_config->m_creaturesPerGeneration; ++i){
        auto newRootNode = std::make_shared<BaseNode>();
        newRootNode->init(true, nullptr, nullptr,m_config);
        auto newScene = std::make_shared<PhysicsScene>();
        newScene->init(m_base, newRootNode,m_config);
        m_testSceneVec.push_back(newScene);
    }
    m_config->m_mutChance /= 3;
}


void ivc::Evolver::createNewGenerationFromParents() {

    printf("Choose %zu parents from this generation\n", m_nextParentVec.size());

    for(const auto&[node, amount] : m_nextParentVec){
        // add the parent unchanged
        auto newScene = std::make_shared<PhysicsScene>();
        newScene->init(m_base,node, m_config);
        m_testSceneVec.push_back(newScene);
        for(int i = 0; i < amount; ++i){
            // add mutated children
            std::random_device rd;
            auto generator = std::make_shared<std::mt19937>(rd());

            std::uniform_real_distribution<> dis(0, 1);

            if(dis(*generator) < 0.5f || m_nextParentVec.size() == 1){
                // asexual reproduction

                auto newRoot = node->copy();
                newRoot->setGenerator(generator);
                newRoot->mutateBodyAndNeurons(m_config->m_lockMorph, m_config);
                newRoot->mutateNewBodyAndNewNeurons(m_config->m_lockMorph, m_config);
                newRoot->mutateNeuralConnections(m_config);
                newScene = std::make_shared<PhysicsScene>();
                newScene->init(m_base,newRoot, m_config);
                m_testSceneVec.push_back(newScene);

            }else{
                // sexual reproduction
                std::vector<std::pair<std::shared_ptr<BaseNode>, unsigned int>> otherParentVec;
                while(otherParentVec.empty() || otherParentVec[0].first == node || otherParentVec[0].first->getChildren().empty()){
                    otherParentVec.clear();
                    std::sample(m_nextParentVec.begin(), m_nextParentVec.end(), std::back_inserter(otherParentVec), 1, *generator);
                }

                auto newRoot = node->copy(); // first parent
                auto partnerRoot = otherParentVec[0].first->copy(); // second parent
                graft(newRoot, partnerRoot, generator);

                newRoot->setGenerator(generator);
                // mutate connections to integrate new body part
                newRoot->mutateNeuralConnections(m_config);
                newScene = std::make_shared<PhysicsScene>();
                newScene->init(m_base,newRoot, m_config);
                m_testSceneVec.push_back(newScene);

            }
        }
    }

}

std::vector<std::pair<std::shared_ptr<ivc::BaseNode>,std::pair<float, std::vector<PxVec3>>>> ivc::Evolver::getCurrentBestVector() {
    return m_currentBestVector;
}

unsigned int ivc::Evolver::getNumberGenerations() {
    return m_numberGenerations;
}

std::vector<std::shared_ptr<ivc::EvoData>> ivc::Evolver::getEvoDataVec() {
    return m_dataVec;
}

void ivc::Evolver::testCurrentGeneration() {

    void (*testFuncPtr)(std::vector<std::shared_ptr<PhysicsScene>>,std::map<std::shared_ptr<BaseNode>, std::pair<std::vector<PxVec3>, int>>*, std::shared_ptr<ivc::EvoConfig>) = testCreatures;

    //fitness test all creatures
    //divide scenes among threads
    std::vector<std::unique_ptr<std::thread>> allThreads;
    std::vector<std::vector<std::shared_ptr<PhysicsScene>>> allScenes;
    allScenes.reserve(m_numThreads);
    for(int i = 0; i < m_numThreads; ++i){
        allScenes.emplace_back();
    }

    unsigned int iter = 0;
    for(auto const& scene : m_testSceneVec){
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
        allThreads.push_back(std::make_unique<std::thread>(testFuncPtr, sceneVec, &m_testPosMap, m_config));
    }

    for (auto&& thread : allThreads) {
        thread->join();
    }

    allThreads.clear();

    //choose viable creatures
    for(const auto &[node, pair] : m_testPosMap){
        if(!pair.first.empty()) {
            m_currentViableCreaturesVec.emplace_back(node,pair.first);
        }
    }

}

void ivc::Evolver::calcFitness() {

    m_currentBestFitnessScore = -INFINITY;
    m_currentWorstFitnessScore = INFINITY;
    m_currentAverageFitnessScore = 0;

    // normal fitness function
    auto sideMP = m_config->m_useSidewaysMP ? m_config->m_sidewaysMultiplier : 0.0f;
    for(auto const& [baseNode, posVec] : m_currentViableCreaturesVec){
        auto startPos = posVec.front();
        auto endPos = posVec.back();

        // calculate and save fitness
        float fitness = 0;

        for(int i = 1; i < posVec.size(); ++i) {
            auto beforePos = posVec[i - 1];
            auto afterPos = posVec[i];

            auto swervingX = sideMP * abs(beforePos.x - afterPos.x);
            fitness += (beforePos.z - afterPos.z) - swervingX;
        }

        if(m_config->useRewardsForEvo()){
            fitness += m_config->m_rewardBonus * (float)m_testPosMap[baseNode].second;
        }else if(m_config->useTargetForEvo()){
            fitness = (float)m_testPosMap[baseNode].second;
        }

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

    m_currentBestNoveltyScore = -INFINITY;
    m_currentWorstNoveltyScore = INFINITY;
    m_currentAverageNoveltyScore = 0;

    // novelty fitness
    // assign novelty score to current gen
    for(auto const& [baseNode, noveltyVec] : m_currentViableCreaturesVec){
        std::vector<float> nearestNeighborsDifferenceVec;
        // skip itself

        for(auto noveltyVecNeighbor : m_noveltyArchive){
            float diff = 0;

            if(noveltyVec.back().z <= 0 && noveltyVec.back().x > -(float(m_config->m_noveltyWidth)/2) && noveltyVec.back().x < float(m_config->m_noveltyWidth)/2){
                diff += std::pow((noveltyVec.back().x - noveltyVecNeighbor.back().x),2);
                diff += std::pow((noveltyVec.back().z - noveltyVecNeighbor.back().z),2);
                diff = std::sqrt(diff);
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
        bool skipped = false;
        for(auto const& [baseNode, noveltyVecNeighbor] : m_currentViableCreaturesVec){
            if(!skipped && noveltyVecNeighbor == noveltyVec){
                skipped = true;
                continue;
            }
            float diff = 0;
            if(noveltyVec.back().z <= 0 && noveltyVec.back().x > -(float(m_config->m_noveltyWidth)/2) && noveltyVec.back().x < float(m_config->m_noveltyWidth)/2){
                diff += std::pow((noveltyVec.back().x - noveltyVecNeighbor.back().x),2);
                diff += std::pow((noveltyVec.back().z - noveltyVecNeighbor.back().z),2);
                diff = std::sqrt(diff);
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

        if(averageDistance > m_currentBestNoveltyScore)
            m_currentBestNoveltyScore = averageDistance;
        if(averageDistance < m_currentWorstNoveltyScore)
            m_currentWorstNoveltyScore = averageDistance;

        m_currentNoveltyMap[baseNode] = averageDistance;

        if(averageDistance > std::max(MIN_NOVELTY_VAL * m_noveltyMinFactor, 0.05f)){
            m_noveltyArchive.push_back(noveltyVec);
            m_evalsSinceLastAddition = 0;
        }else{
            m_evalsSinceLastAddition += 1;
        }

        if(m_evalsSinceLastAddition >= 1000){
            m_noveltyMinFactor -= 0.05f;
            m_evalsSinceLastAddition = 0;
        }

    }

    m_noveltyArchiveCopy = m_noveltyArchive;

}

void ivc::Evolver::chooseParents() {

    // calculate average score
    float scoreSum = 0;
    for(const auto &[node, score] : m_currentFitnessMap){
        scoreSum += score;
    }
    m_currentAverageFitnessScore = scoreSum / m_currentFitnessMap.size();

    // calculate average novelty
    float noveltySum = 0;
    for(const auto &[node, score] : m_currentNoveltyMap){
        noveltySum += score;
    }
    m_currentAverageNoveltyScore = noveltySum / m_currentNoveltyMap.size();

    if(!m_config->m_userSelection){

        // normalize scores
        std::vector<std::pair<std::shared_ptr<BaseNode>, float>> normalizedScores;
        if(m_config->m_useNoveltySearch){
            for(const auto &[node, score] : m_currentNoveltyMap){
                if(m_currentBestNoveltyScore == m_currentWorstNoveltyScore){
                    normalizedScores.emplace_back(node,1.0f);
                }else{
                    normalizedScores.emplace_back(node, Mutator::normalize(score, m_currentWorstNoveltyScore, m_currentBestNoveltyScore));
                }
            }
        }else{
            for(const auto &[node, score] : m_currentFitnessMap){
                if(m_currentBestFitnessScore == m_currentWorstFitnessScore){
                    normalizedScores.emplace_back(node,1.0f);
                }else{
                    normalizedScores.emplace_back(node, Mutator::normalize(score, m_currentWorstFitnessScore, m_currentBestFitnessScore));
                }
            }
        }

        //sort by score
        std::sort(normalizedScores.begin(), normalizedScores.end(), [](auto &left, auto &right) {
            return left.second > right.second;
        });

        //cap number of parents
        auto maxParents = floor(m_config->m_creaturesPerGeneration * 0.2);
        if(normalizedScores.size() > maxParents){
            normalizedScores.resize(maxParents);
        }

        //choose amount of children per root
        std::vector<std::pair<std::shared_ptr<BaseNode>,unsigned int>> amountVec;
        float total = 0;
        for(const auto&[node, score] : normalizedScores){
            total += score;
        }
        float partSize = m_config->m_creaturesPerGeneration / total;

        for(const auto&[node, score] : normalizedScores){
            auto amountChildren = std::max(1.0f, floor(score * partSize));
            amountVec.emplace_back(node, amountChildren);
        }

        m_nextParentVec = amountVec;

    }else{

        if(!m_currentBestVector.empty())
            throw std::logic_error("AUA");

        auto fitAmount = std::floor(m_config->m_numberDisplayedCreatures/2.f);
        auto novelAmount = std::ceil(m_config->m_numberDisplayedCreatures/2.f);

        std::vector<std::pair<std::shared_ptr<BaseNode>, std::pair<float, std::vector<PxVec3>>>> fitVec;
        std::vector<std::pair<std::shared_ptr<BaseNode>, std::pair<float, std::vector<PxVec3>>>> novelVec;

        for(auto [node, path] : m_currentViableCreaturesVec){
            if(fitVec.size() < fitAmount) {
                fitVec.push_back({node, {m_currentFitnessMap[node], path}});
                continue;
            } else{
                int toReplaceIndex = -1;
                float worstScore = INFINITY;
                for (int i = 0; i < fitVec.size(); ++i) {
                    float oldScore = m_currentFitnessMap[fitVec[i].first];
                    if (m_currentFitnessMap[node] > oldScore && worstScore > oldScore) {
                        toReplaceIndex = i;
                        worstScore = oldScore;
                    }
                }
                if (toReplaceIndex > -1) {
                    fitVec[toReplaceIndex] = {node, {m_currentFitnessMap[node], path}};
                    continue;
                }
            }

            if(novelVec.size() < novelAmount) {
                novelVec.push_back({node, {m_currentNoveltyMap[node], path}});
                continue;
            } else{
                int toReplaceIndex = -1;
                float worstScore = INFINITY;
                for (int i = 0; i < novelVec.size(); ++i) {
                    float oldScore = m_currentNoveltyMap[novelVec[i].first];
                    if (m_currentNoveltyMap[node] > oldScore && worstScore > oldScore) {
                        toReplaceIndex = i;
                        worstScore = oldScore;
                    }
                }
                if (toReplaceIndex > -1) {
                    novelVec[toReplaceIndex] = {node, {m_currentNoveltyMap[node], path}};
                    continue;
                }
            }
        }

        m_currentBestVector.insert(m_currentBestVector.end(), fitVec.begin(), fitVec.end());
        m_currentBestVector.insert(m_currentBestVector.end(), novelVec.begin(), novelVec.end());

        while(m_selectedNode == nullptr){
            // wait
        }

        m_nextParentVec = {{m_selectedNode, m_config->m_creaturesPerGeneration}};
        m_selectedNode = nullptr;

    }

}

void ivc::Evolver::createEvoData() {

    auto newData = std::make_shared<EvoData>();
    newData->setGeneration(m_numberGenerations);
    newData->m_largestDistanceTravelled = m_currentLargestDistance;
    newData->m_worstDistanceTravelled = m_currentWorstDistance;
    newData->m_averageDistanceTravelled = m_currentAverageDistance;

    newData->m_bestFitnessScore = m_currentBestFitnessScore;
    newData->m_worstFitnessScore = m_currentWorstFitnessScore;
    newData->m_averageFitnessScore = m_currentAverageFitnessScore;

    newData->m_bestNoveltyScore = m_currentBestNoveltyScore;
    newData->m_worstNoveltyScore = m_currentWorstNoveltyScore;
    newData->m_averageNoveltyScore = m_currentAverageNoveltyScore;

    newData->m_highestComplexity = m_currentHighestComplexity;
    newData->m_lowestComplexity = m_currentLowestComplexity;
    newData->m_averageComplexity = m_currentAverageComplexity;

    newData->m_mostActiveNeurons = m_currentMostActiveNeurons;
    newData->m_leastActiveNeurons = m_currentLeastActiveNeurons;
    newData->m_averageActiveNeurons = m_currentAverageActiveNeurons;

    newData->m_mostActiveBrainNeurons = m_currentMostActiveBrainNeurons;
    newData->m_leastActiveBrainNeurons = m_currentLeastActiveBrainNeurons;
    newData->m_averageActiveBrainNeurons = m_currentAverageActiveBrainNeurons;

    newData->m_mostActiveJointSensors = m_currentMostActiveJointSensors;
    newData->m_leastActiveJointSensors = m_currentLeastActiveJointSensors;
    newData->m_averageActiveJointSensors = m_currentAverageActiveJointSensors;

    newData->m_mostActiveContactSensors = m_currentMostActiveContactSensors;
    newData->m_leastActiveContactSensors = m_currentLeastActiveContactSensors;
    newData->m_averageActiveContactSensors = m_currentAverageActiveContactSensors;

    newData->m_complexityOfCurrentBest = m_complexityOfCurrentBest;
    newData->m_neuronsOfCurrentBest = m_neuronsOfCurrentBest;
    newData->m_brainOfCurrentBest = m_brainOfCurrentBest;
    newData->m_jointsOfCurrentBest = m_jointsOfCurrentBest;
    newData->m_contactsOfCurrentBest = m_contactsOfCurrentBest;

    m_dataVec.push_back(newData);

}

void ivc::Evolver::graft(std::shared_ptr<BaseNode> base, std::shared_ptr<BaseNode> partner, std::shared_ptr<std::mt19937> gen) {

    std::uniform_real_distribution<> dis(0, 1);

    auto nodeInBase = base->getRandomNode(gen);

    auto nodeInPartner = partner->getRandomNode(gen);
    // cant attach root
    while(nodeInPartner == partner)
        nodeInPartner = partner->getRandomNode(gen);

    //decide to attach or replace
    if(dis(*gen) < 1.f/(float)(nodeInBase->getChildren().size()+1)){
        //attach
        auto attachSide = nodeInBase->getRandomFreeSide(gen);

        nodeInPartner->setParent(nodeInBase.get());
        nodeInPartner->changeAnchorSideTo(attachSide);

        nodeInBase->setSideAsOccupied(attachSide);
        nodeInBase->addChild(nodeInPartner);

        //choose new outputs recursively
        std::map<unsigned long,unsigned long> newOutputIDs;
        nodeInPartner->chooseNewNeuronIDs(&newOutputIDs);

        //rewire inputs recursively
        nodeInPartner->rewireInputs(&newOutputIDs);
    }else{
        //replace
        auto toReplace = nodeInBase->getRandomChild(gen);
        nodeInPartner->changeAnchorSideTo(toReplace->getParentSide());

        auto parentNode = toReplace->getParentNode();
        parentNode->replaceChild(toReplace, nodeInPartner);

        //choose new outputs recursively
        std::map<unsigned long,unsigned long> newOutputIDs;
        nodeInPartner->chooseNewNeuronIDs(&newOutputIDs);

        //rewire inputs recursively
        nodeInPartner->rewireInputs(&newOutputIDs);
    }

}

std::vector<std::vector<PxVec3>> ivc::Evolver::getNoveltyArchive() {
    return m_noveltyArchiveCopy;
}

void ivc::Evolver::calcDistanceTravelled() {

    m_currentLargestDistance = -INFINITY;
    m_currentWorstDistance = INFINITY;
    m_currentAverageDistance = 0;

    bool shouldRefresh = false;
    if(m_config->m_clearBestVec || m_config->m_userSelection){
        m_currentBestVector = {};
        m_config->m_clearBestVec = false;
        shouldRefresh = true;
    }

    for(auto const& [baseNode, posVec] : m_currentViableCreaturesVec) {
        auto startPos = posVec.front();
        auto endPos = posVec.back();

        // calc distance travelled
        auto distanceTravelled = startPos.z - endPos.z;
        if (distanceTravelled > m_currentLargestDistance) {
            m_currentLargestDistance = distanceTravelled;
        }
        if(distanceTravelled < m_currentWorstDistance)
            m_currentWorstDistance = distanceTravelled;
        m_currentAverageDistance += distanceTravelled;

        // is this creature better than the current best creatures?
        if(!m_config->m_userSelection){
            if(m_currentBestVector.size() > m_config->m_numberDisplayedCreatures){
                m_currentBestVector.resize(m_config->m_numberDisplayedCreatures);
            } else if(m_currentBestVector.size() < m_config->m_numberDisplayedCreatures) {
                m_currentBestVector.push_back({baseNode, {distanceTravelled, posVec}});
            } else {
                int toReplaceIndex = -1;
                float worstDistance = INFINITY;
                auto modifiedDistance = distanceTravelled;
                for (int i = 0; i < m_currentBestVector.size(); ++i) {
                    float oldDistance = m_currentBestVector[i].second.first;

                    //if we use targets or rewards
                    if(m_config->useRewardsForEvo()){
                        modifiedDistance += m_config->m_rewardBonus * (float)m_testPosMap[baseNode].second;
                    }else if(m_config->useTargetForEvo()){
                        modifiedDistance = (float)m_testPosMap[baseNode].second;
                    }

                    if (modifiedDistance > oldDistance && worstDistance > oldDistance) {
                        toReplaceIndex = i;
                        worstDistance = oldDistance;
                    }
                }
                if (toReplaceIndex > -1) {
                    m_currentBestVector[toReplaceIndex] = {baseNode, {modifiedDistance, posVec}};
                }
            }
        }

    }
    m_currentAverageDistance /= m_currentViableCreaturesVec.size();
    if(shouldRefresh)
        m_config->m_refreshLiveEnvironment = true;
}

void ivc::Evolver::clean() {
    m_numberGenerations = 1;
    m_currentBestVector = {};
    m_dataVec = {};

    m_testSceneVec = {};
    m_testPosMap = {};
    m_noveltyArchive = {};
    m_noveltyArchiveCopy = {};
    m_currentViableCreaturesVec = {};
    m_currentFitnessMap = {};
    m_currentNoveltyMap = {};
    m_nextParentVec = {};

    m_evalsSinceLastAddition = 0;
    m_noveltyMinFactor = 1.0f;
}

void ivc::Evolver::loadStartNode(std::shared_ptr<BaseNode> node) {
    auto newScene = std::make_shared<PhysicsScene>();
    newScene->init(m_base,node, m_config);
    m_testSceneVec.push_back(newScene);
    for(int i = 0; i < m_config->m_creaturesPerGeneration; ++i){
        std::random_device rd;
        auto generator = std::make_shared<std::mt19937>(rd());
        auto newRoot = node->copy();
        newRoot->setGenerator(generator);
        newRoot->mutateBodyAndNeurons(m_config->m_lockMorph, m_config);
        newRoot->mutateNewBodyAndNewNeurons(m_config->m_lockMorph, m_config);
        newRoot->mutateNeuralConnections(m_config);
        newScene = std::make_shared<PhysicsScene>();
        newScene->init(m_base,newRoot, m_config);
        m_testSceneVec.push_back(newScene);
    }


}

void ivc::Evolver::calcStats() {

    m_currentHighestComplexity = -INFINITY;
    m_currentLowestComplexity = INFINITY;
    m_currentAverageComplexity = 0;

    m_currentMostActiveNeurons = -INFINITY;
    m_currentLeastActiveNeurons= INFINITY;
    m_currentAverageActiveNeurons = 0;

    m_currentMostActiveBrainNeurons = -INFINITY;
    m_currentLeastActiveBrainNeurons= INFINITY;
    m_currentAverageActiveBrainNeurons = 0;

    m_currentMostActiveJointSensors = -INFINITY;
    m_currentLeastActiveJointSensors= INFINITY;
    m_currentAverageActiveJointSensors = 0;

    m_currentMostActiveContactSensors = -INFINITY;
    m_currentLeastActiveContactSensors= INFINITY;
    m_currentAverageActiveContactSensors = 0;

    for(auto const& [baseNode, posVec] : m_currentViableCreaturesVec) {

        auto complexity = baseNode->getNumberOfParts();
        auto neuronStats = baseNode->getNeuronActivity();
        auto neurons = neuronStats[0];
        auto brainNeurons = neuronStats[1];
        auto jointSensors = neuronStats[2];
        auto contactSensors = neuronStats[3];

        if(complexity > m_currentHighestComplexity)
            m_currentHighestComplexity = complexity;
        if(complexity < m_currentLowestComplexity)
            m_currentLowestComplexity = complexity;
        m_currentAverageComplexity += complexity;

        if(neurons > m_currentMostActiveNeurons)
            m_currentMostActiveNeurons = neurons;
        if(neurons < m_currentLeastActiveNeurons)
            m_currentLeastActiveNeurons = neurons;
        m_currentAverageActiveNeurons += neurons;

        if(brainNeurons > m_currentMostActiveBrainNeurons)
            m_currentMostActiveBrainNeurons = brainNeurons;
        if(brainNeurons < m_currentLeastActiveBrainNeurons)
            m_currentLeastActiveBrainNeurons = brainNeurons;
        m_currentAverageActiveBrainNeurons += brainNeurons;

        if(jointSensors > m_currentMostActiveJointSensors)
            m_currentMostActiveJointSensors = jointSensors;
        if(jointSensors < m_currentLeastActiveJointSensors)
            m_currentLeastActiveJointSensors = jointSensors;
        m_currentAverageActiveJointSensors += jointSensors;

        if(contactSensors > m_currentMostActiveContactSensors)
            m_currentMostActiveContactSensors = contactSensors;
        if(contactSensors < m_currentLeastActiveContactSensors)
            m_currentLeastActiveContactSensors = contactSensors;
        m_currentAverageActiveContactSensors += contactSensors;

        auto startPos = posVec.front();
        auto endPos = posVec.back();
        // calc distance travelled
        auto distanceTravelled = startPos.z - endPos.z;

        if(distanceTravelled == m_currentLargestDistance){
            m_complexityOfCurrentBest = complexity;
            m_neuronsOfCurrentBest = neurons;
            m_brainOfCurrentBest = brainNeurons;
            m_jointsOfCurrentBest = jointSensors;
            m_contactsOfCurrentBest = contactSensors;
        }

    }

    m_currentAverageComplexity /= (float)m_currentViableCreaturesVec.size();
    m_currentAverageActiveNeurons /= (float)m_currentViableCreaturesVec.size();
    m_currentAverageActiveBrainNeurons /= (float)m_currentViableCreaturesVec.size();
    m_currentAverageActiveJointSensors /= (float)m_currentViableCreaturesVec.size();
    m_currentAverageActiveContactSensors /= (float)m_currentViableCreaturesVec.size();

}

void ivc::Evolver::setUserSelection(std::shared_ptr<BaseNode> node) {
    m_selectedNode = node;
}
