//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H
#define INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H

#include "../Gate.h"
#include "PxPhysicsAPI.h"
#include <vector>
#include <random>
#include "../../Constants.h"
#include <chrono>
#include <algorithm>
#include "../../Mutator.h"

using namespace physx;

namespace ivc{
    class JointEffector {
    private:
        PxD6Joint* m_joint = nullptr;
        Gate *input_0, *input_1, *input_2;
        unsigned long id_input_0, id_input_1, id_input_2;
        float weight_0 = 1.0f;
        float weight_1 = 1.0f;
        float weight_2 = 1.0f;
    public:
        void randomize(std::mt19937*);
        void step();
        void setJoint(PxD6Joint*);
        std::vector<unsigned long> getGateIDs();
        int bindGates(std::vector<Gate*>);
        void chooseRandomInputs(std::vector<unsigned long>);
        void mutate(std::mt19937*);
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H
