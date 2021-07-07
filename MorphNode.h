//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H
#define INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H

#include "PxPhysicsAPI.h"
#include "NeuronCluster.h"
#include <vector>
#include <random>
#include "Constants.h"

using namespace physx;

namespace ivc {

    class MorphNode {
        private:
            //body
            PxVec3 m_dimension;
            unsigned int m_recursionLimit;
            NeuronCluster m_localNeurons;

            //connection
            PxVec3 m_parentAnchor;
            PxVec3 m_childAnchor;
            PxVec3 m_orientation;
            PxVec3 m_scale;
            //TODO: joint type and DOF
            //TODO: reflection
            bool m_terminalOnly;

            //children
            std::vector<MorphNode> m_childNodeVector;

        public:
            MorphNode(std::mt19937, unsigned int);

    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H
