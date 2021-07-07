//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H
#define INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H

#include "PxPhysicsAPI.h"
#include "NeuronCluster.h"
#include <vector>

using namespace physx;

namespace ivc {

    class MorphNode {
        private:
            //body
            PxVec3 m_dimension;
            unsigned int m_recursionLimit;
            NeuronCluster m_localNeurons;

            //connection
            physx::PxVec3 m_position;
            physx::PxVec3 m_orientation;
            physx::PxVec3 m_scale;
            //TODO: joint type and DOF
            //TODO: reflection
            bool m_terminalOnly;

            //children
            std::vector<MorphNode> m_childNodeVector;

        public:
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H
