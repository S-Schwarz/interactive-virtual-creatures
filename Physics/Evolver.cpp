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
        auto swervingX = 0.5f * abs(startPos.x - endPos.x);
        fitness = (distanceTravelled - swervingX);

        //write result
        (*mapPtr)[scene] = {(*mapPtr)[scene].first,fitness};
    }

}

void ivc::Evolver::evolveNextGeneration() {

    void (*testFuncPtr)(std::vector<ivc::PhysicsScene*>, std::map<ivc::PhysicsScene*,std::pair<ivc::RootMorphNode*, float>>*) = testCreatures;

    printf("Size: %i\n", sceneMap.size());

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

    createNextGeneration();

}

ivc::RootMorphNode* ivc::Evolver::evolveNewCreature() {

    for(int i = 0; i < NUMBER_OF_GENERATIONS; ++i){
        printf("---------------------------\n");
        printf("GENERATION #%i:\n", i+1);
        evolveNextGeneration();
    }

    return currentBest;

}

void ivc::Evolver::createNextGeneration() {

    //find best creature
    RootMorphNode* bestCreature = nullptr;
    float bestScore = -INFINITY;
    float worstScore = INFINITY;
    for(auto const& pair : sceneMap){
        auto score = pair.second.second;
        if(score > bestScore){
            bestCreature = pair.second.first;
            bestScore = score;
        }
        if(score != -INFINITY && score < worstScore){
            worstScore = score;
        }
    }
    currentBest = bestCreature;
    printf("Best Score: %f\n", bestScore);

    if(bestScore == 0){
        //create completely new generation
        printf("Creating new generation!\n");
        deleteLastGeneration({});
        sceneMap = {};
        createNewGeneration();
    }else{
        //normalize scores
        for(auto pair : sceneMap){
            auto score = pair.second.second;
            if(bestScore == worstScore){
                sceneMap[pair.first] = {pair.second.first, 1.0f};
            }else{
                sceneMap[pair.first] = {pair.second.first, (score-worstScore)/(bestScore-worstScore)};
            }
        }
        //choose best creatures
        std::vector<std::pair<RootMorphNode*,float>> bestVec;
        for(auto pair : sceneMap){
            auto score = pair.second.second;
            if(score > EVOLUTION_MIN_SCORE){
                bestVec.push_back(pair.second);
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

        printf("Chose %zu parents for next gen\n", bestVec.size());

        //choose amount of children per root
        std::vector<std::pair<RootMorphNode*,unsigned int>> amountVec;
        float total = 0;
        for(auto pair : bestVec){
            total += pair.second;
        }
        float partSize = CREATURES_PER_GENERATION / total;

        for(auto pair : bestVec){
            auto amountChildren = floor(pair.second * partSize);
            amountVec.push_back({pair.first, amountChildren});
        }

        //create new generation
        auto nextGenMap = createNewGenerationFromParents(amountVec);

        //replace and delete old generation
        deleteLastGeneration(bestVec);
        sceneMap = nextGenMap;
    }



}

void ivc::Evolver::createNewGeneration() {
    for(int i = 0; i < CREATURES_PER_GENERATION; ++i){
        auto newRootNode = new RootMorphNode();
        newRootNode->init();
        newRootNode->addNeuralConnections();
        auto newScene = new PhysicsScene();
        newScene->init(m_base,newRootNode);
        sceneMap[newScene] = {newRootNode, -INFINITY};
    }
}

void ivc::Evolver::deleteLastGeneration(std::vector<std::pair<RootMorphNode*,float>> parents) {
    for(auto const& pair : sceneMap){
        pair.first->destroy();
        delete pair.first;

        bool keep = false;
        for(auto p : parents){
            if(pair.second.first == p.first){
                keep = true;
                break;
            }
        }
        if(!keep)
            delete pair.second.first;
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
