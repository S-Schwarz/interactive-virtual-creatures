//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H
#define INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H

#include "../Gate.h"
#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;

namespace ivc{
    class JointSensor {
    private:
        PxD6Joint* m_joint = nullptr;
        Gate output_0, output_1, output_2;
        unsigned long id_output_0, id_output_1, id_output_2;
    public:
        void step();
        void swap();
        void setJoint(PxD6Joint*);
        void setIDs(unsigned long, unsigned long, unsigned long);
        std::vector<unsigned long> getOutputIDs();
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H
