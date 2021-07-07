//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_ROOTMORPHNODE_H
#define INTERACTIVE_VIRTUAL_CREATURES_ROOTMORPHNODE_H

#include "PxPhysicsAPI.h"
#include "NeuronCluster.h"
#include "MorphNode.h"
#include <vector>

using namespace physx;

namespace ivc {

    class RootMorphNode {
        private:
            //body
            PxVec3 m_dimension;
            unsigned int m_recursionLimit;
            NeuronCluster m_localNeurons;

            //global brain
            NeuronCluster m_brain;

            //children
            std::vector<MorphNode> m_childNodeVector;

        public:

    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_ROOTMORPHNODE_H
