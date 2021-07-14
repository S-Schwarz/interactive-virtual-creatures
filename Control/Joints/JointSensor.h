//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H
#define INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H

#include "../Gate.h"
#include "PxPhysicsAPI.h"

using namespace physx;

namespace ivc{
    class JointSensor {
    private:
        Gate output_0, output_1, output_2;
    public:
        void step();
        void swap();
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H
