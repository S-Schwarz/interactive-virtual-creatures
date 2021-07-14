//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H
#define INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H

#include <vector>
#include "PxPhysicsAPI.h"
#include "../Control/NeuronCluster.h"
#include <algorithm>
#include <random>

using namespace physx;

namespace ivc{

    enum NODE_SIDE{
        POS_X,
        POS_Y,
        POS_Z,
        NEG_X,
        NEG_Y,
        NEG_Z,
        NONE
    };

    class BaseNode {
        protected:
            //body
            PxVec3 m_dimension;
            PxVec3 m_recursionAnchor;
            PxVec3 m_scale;
            unsigned int m_recursionLimit;
            NeuronCluster* m_localNeurons = nullptr;
            BaseNode* m_parentNode = nullptr;
            std::pair<float,float> m_swingLimits, m_twistLimits;
            //children
            std::vector<BaseNode*> m_childNodeVector;
            std::vector<NODE_SIDE> m_freeSides = {POS_X,NEG_X,POS_Y,NEG_Y,POS_Z,NEG_Z};
        public:
            virtual unsigned int getNumberOfParts();
            virtual std::vector<BaseNode*> getChildren();
            virtual int getRecursionLimit();
            virtual PxVec3 getDimensions();
            virtual PxVec3 getHalfExtents();
            virtual PxVec3 getOrientation();
            virtual PxVec3 getParentAnchor();
            virtual BaseNode* getParentNode();
            virtual PxVec3 getScale();
            virtual std::pair<float,float> getSwingLimits();
            virtual std::pair<float,float> getTwistLimits();
            virtual NODE_SIDE occupyRandomSide();
            virtual int setSideAsOccupied(NODE_SIDE);
            virtual IDHandler* getIDHandler();
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H
