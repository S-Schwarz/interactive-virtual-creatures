//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H
#define INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H

#include "PxPhysicsAPI.h"
#include "../Control/NeuronCluster.h"
#include <vector>
#include <random>
#include "../Constants.h"
#include "BaseNode.h"

using namespace physx;

namespace ivc {

    class MorphNode : public BaseNode {
        private:
            //connection
            PxVec3 m_orientation = PxVec3(0,0,0);
            bool m_terminalOnly;
        public:
            ~MorphNode() override;
            BaseNode* copy() override;
            void mutateBodyAndNeurons() override;
            void mutateNewBodyAndNewNeurons() override;
            void init(BaseNode*, std::mt19937*, unsigned int);
            void addNeuralConnections() override;
            PxVec3 getAnchorPosition(std::mt19937* gen);
            PxVec3 getOrientation() override;
            PxVec3 getParentAnchor() override;
            IDHandler* getIDHandler() override;
            NeuronCluster* getBrain() override;


    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H
