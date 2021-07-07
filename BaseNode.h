//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H
#define INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H

#include <vector>
#include "PxPhysicsAPI.h"
#include "NeuronCluster.h"

using namespace physx;

namespace ivc{

    class BaseNode {
        protected:
            //body
            PxVec3 m_dimension;
            unsigned int m_recursionLimit;
            NeuronCluster m_localNeurons;

            //children
            std::vector<BaseNode> m_childNodeVector;
        public:
            virtual unsigned int getNumberOfParts();
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H
