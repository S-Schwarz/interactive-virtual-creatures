//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H
#define INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H

#include "../Gate.h"
#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;

namespace ivc{
    class JointEffector {
    private:
        PxD6Joint* m_joint = nullptr;
        Gate *input_0, *input_1, *input_2;
        unsigned long id_input_0, id_input_1, id_input_2;
    public:
        void step();
        void setJoint(PxD6Joint*);
        std::vector<unsigned long> getGateIDs();
        int bindGates(std::vector<Gate*>);
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H
