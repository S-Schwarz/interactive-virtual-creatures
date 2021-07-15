//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_ROOTMORPHNODE_H
#define INTERACTIVE_VIRTUAL_CREATURES_ROOTMORPHNODE_H

#include "PxPhysicsAPI.h"
#include "../Control/NeuronCluster.h"
#include "MorphNode.h"
#include <vector>
#include <random>
#include "../Constants.h"
#include <iostream>
#include "BaseNode.h"
#include "../Control/IDHandler.h"

using namespace physx;

namespace ivc {

    class RootMorphNode : public BaseNode{
        private:
            //global brain
            NeuronCluster* m_brain = nullptr;
            IDHandler* m_idHandler = nullptr;
        public:
            void init();
            void addNeuralConnections() override;
            PxVec3 getParentAnchor() override;
            void setRecursionAnchor(std::mt19937 gen);
            IDHandler* getIDHandler() override;
            NeuronCluster* getBrain() override;
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_ROOTMORPHNODE_H
