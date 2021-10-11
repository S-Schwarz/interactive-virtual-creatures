//
// Created by st on 7/16/21.
//

#include "Evolver.h"

#include <memory>
#include "../Body/BaseNode.h"

void testCreatures(std::vector<std::shared_ptr<ivc::PhysicsScene>> sceneVec, std::map<std::shared_ptr<ivc::BaseNode>, std::vector<PxVec3>>* posMapPtr, int stepsPG, int interval){

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
        std::vector<PxVec3> posRecordVec;

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

            if( i % interval == 0){
                posRecordVec.push_back(currentPos);
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
        (*posMapPtr)[scene->getRootNode()] = posRecordVec;
    }

}

int ivc::Evolver::init(std::shared_ptr<ivc::PhysicsBase> base, std::shared_ptr<EvoConfig> config) {

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
        m_testSceneVec = {};
        m_testPosMap = {};

        if(m_currentViableCreaturesVec.empty()){
            createFirstGeneration();
        }else{
            calcFitness();
            calcNovelty();
            printf("Largest distance: %f\n", m_currentLargestDistance);
            printf("NEURONS OVERALL: %u\n", m_currentBestVector[0].first->getNeuronActivity()[0] + m_currentBestVector[0].first->getNeuronActivity()[1]);
            printf("NEURONS BRAIN: %u\n", m_currentBestVector[0].first->getNeuronActivity()[1]);
            printf("JOINT SENSORS: %u\n", m_currentBestVector[0].first->getNeuronActivity()[2]);
            printf("CONTACT SENSORS: %u\n", m_currentBestVector[0].first->getNeuronActivity()[3]);
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
    }

}

void ivc::Evolver::createFirstGeneration() {
    for(int i = 0; i < m_config->m_creaturesPerGeneration; ++i){
        auto newRootNode = std::make_shared<BaseNode>();
        newRootNode->init(true, nullptr, nullptr,m_config);
        auto newScene = std::make_shared<PhysicsScene>();
        newScene->init(m_base, newRootNode,m_config);
        m_testSceneVec.push_back(newScene);
    }
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

            if(dis(*generator) < 0.5){
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
                while(otherParentVec.empty() || otherParentVec[0].first == node){
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

void ivc::Evolver::stopEvolution() {
    m_config->m_paused = true;
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

    void (*testFuncPtr)(std::vector<std::shared_ptr<PhysicsScene>>,std::map<std::shared_ptr<BaseNode>, std::vector<PxVec3>>*, int, int) = testCreatures;

    printf("Size: %lu\n", m_testSceneVec.size());

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
        allThreads.push_back(std::make_unique<std::thread>(testFuncPtr, sceneVec, &m_testPosMap, m_config->m_stepsPerGeneration, noveltyInterval));
    }

    for (auto&& thread : allThreads) {
        thread->join();
    }

    allThreads.clear();

    //choose viable creatures
    for(const auto &[node, vec] : m_testPosMap){
        if(!vec.empty()) {
            m_currentViableCreaturesVec.emplace_back(node,vec);
        }
    }

}

void ivc::Evolver::calcFitness() {

    m_currentLargestDistance = -INFINITY;
    m_currentBestFitnessScore = -INFINITY;
    m_currentWorstFitnessScore = INFINITY;

    if(m_clearBestVec){
        m_currentBestVector = {};
        m_clearBestVec = false;
    }

    // normal fitness function
    auto sideMP = m_config->m_useSidewaysMP ? m_config->m_sidewaysMultiplier : 0.0f;
    for(auto const& [baseNode, posVec] : m_currentViableCreaturesVec){
        auto startPos = posVec.front();
        auto endPos = posVec.back();

        // calc distance travelled
        auto distanceTravelled = startPos.z - endPos.z;
        if(distanceTravelled > m_currentLargestDistance){
            m_currentLargestDistance = distanceTravelled;
        }

        // is this creature better than the current best creatures?
        if(m_currentBestVector.size() < m_config->m_numberDisplayedCreatures){
                m_currentBestVector.push_back({baseNode, {distanceTravelled, posVec}});
        }else{
            int toReplaceIndex = -1;
            float worstDistance = INFINITY;
            for(int i = 0; i < m_currentBestVector.size(); ++i){
                float oldDistance = m_currentBestVector[i].second.first;
                if(distanceTravelled > oldDistance && worstDistance > oldDistance){
                    toReplaceIndex = i;
                    worstDistance = oldDistance;
                }
            }
            if(toReplaceIndex > -1){
                m_currentBestVector[toReplaceIndex] = {baseNode, {distanceTravelled, posVec}};
            }
        }

        // calculate and save fitness
        float fitness = 0;

        for(int i = 1; i < posVec.size(); ++i) {
            auto beforePos = posVec[i - 1];
            auto afterPos = posVec[i];

            auto swervingX = sideMP * abs(beforePos.x - afterPos.x);
            fitness += (beforePos.z - afterPos.z) - swervingX;
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

    // copy current gen into overall archive
    for(auto const& [baseNode, noveltyVec] : m_currentViableCreaturesVec){
        m_noveltyArchive.push_back(noveltyVec);
    }

    m_noveltyArchiveCopy = m_noveltyArchive;

    // novelty fitness
    // assign novelty score to current gen
    for(auto const& [baseNode, noveltyVec] : m_currentViableCreaturesVec){
        std::vector<float> nearestNeighborsDifferenceVec;
        // skip itself
        bool skipped = false;
        for(auto noveltyVecNeighbor : m_noveltyArchive){
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

            /*
            else{
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
            */
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

    }

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

}

void ivc::Evolver::createEvoData() {

    auto newData = std::make_shared<EvoData>();
    newData->setGeneration(m_numberGenerations);
    newData->setLargestDistance(m_currentLargestDistance);

    newData->setBestFitnessScore(m_currentBestFitnessScore);
    newData->setWorstFitnessScore(m_currentWorstFitnessScore);
    newData->setAverageFitnessScore(m_currentAverageFitnessScore);

    newData->setBestNoveltyScore(m_currentBestNoveltyScore);
    newData->setWorstNoveltyScore(m_currentWorstNoveltyScore);
    newData->setAverageNoveltyScore(m_currentAverageNoveltyScore);

    m_dataVec.push_back(newData);

}

void ivc::Evolver::clearBestVec() {
    m_clearBestVec = true;
}

void ivc::Evolver::graft(std::shared_ptr<BaseNode> base, std::shared_ptr<BaseNode> partner, std::shared_ptr<std::mt19937> gen) {

    // if nothing to graft from or to return
    if(base->getChildren().empty() || partner->getChildren().empty())
        return;

    auto toReplace = base->getRandomNode(gen);
    while(toReplace == nullptr)
        toReplace = base->getRandomNode(gen);
    auto replacement = partner->getRandomNode(gen);
    while(replacement == nullptr)
        replacement = partner->getRandomNode(gen);

    replacement->changeAnchorSideTo(toReplace->getParentSide());

    auto parentNode = toReplace->getParentNode();
    parentNode->replaceChild(toReplace, replacement);

}

std::vector<std::vector<PxVec3>> ivc::Evolver::getNoveltyArchive() {
    return m_noveltyArchiveCopy;
}
