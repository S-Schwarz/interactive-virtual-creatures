//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H
#define INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H

#include "../Gate.h"
#include "PxPhysicsAPI.h"

using namespace physx;

namespace ivc{
    class JointEffector {
    private:
        Gate *input;
        PxD6Joint* output_joint;
    public:
        void step();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H
