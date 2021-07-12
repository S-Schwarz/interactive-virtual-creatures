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
#include "BaseNode.h"

using namespace physx;

namespace ivc {

    class MorphNode : public BaseNode {
        private:
            //connection
            PxVec3 m_parentAnchor;
            PxVec3 m_orientation;
            std::pair<float,float> m_swingLimits, m_twistLimits;
            //TODO: reflection
            bool m_terminalOnly;
        public:
            MorphNode() = default;
            MorphNode(BaseNode*, std::mt19937, unsigned int);
            PxVec3 getAnchorPosition(std::mt19937 gen);
            PxVec3 getOrientation() override;
            PxVec3 getParentAnchor() override;
            PxVec3 getScaledHalfExtents() override;
            PxVec3 getGlobalScale() override;
            std::pair<float,float> getSwingLimits() override;
            std::pair<float,float> getTwistLimits() override;


    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_MORPHNODE_H
