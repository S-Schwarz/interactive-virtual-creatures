//
// Created by st on 7/1/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_BODYPART_H
#define INTERACTIVE_VIRTUAL_CREATURES_BODYPART_H

#include "BodyConnection.h"
#include <vector>
#include <foundation/PxVec3.h>

namespace ivc {
    class BodyPart {
        private:
            physx::PxVec3 m_dimensions;
            //TODO: joint type and DOF
            unsigned int m_recursiveLimit;
            std::vector<BodyConnection *> m_bodyConnectionVector;
            //TODO: local neurons
        public:

    };
}

#endif //INTERACTIVE_VIRTUAL_CREATURES_BODYPART_H
