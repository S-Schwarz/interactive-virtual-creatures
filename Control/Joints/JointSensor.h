//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H
#define INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H

#include "../Gate.h"
#include "PxPhysicsAPI.h"
#include <vector>
#include <random>
#include "../../Constants.h"
#include "../../Mutator.h"

using namespace physx;

namespace ivc{
    class JointSensor {
    private:
        PxD6Joint* m_joint = nullptr;
        Gate *output_0, *output_1, *output_2;
        unsigned long id_output_0, id_output_1, id_output_2;
        float weight_0 = 1.0f;
        float weight_1 = 1.0f;
        float weight_2 = 1.0f;
    public:
        void setOutputGates(std::vector<Gate*>);
        void randomize(std::mt19937*);
        void step();
        void swap();
        void setJoint(PxD6Joint*);
        void setIDs(unsigned long, unsigned long, unsigned long);
        std::vector<unsigned long> getOutputIDs();
        void mutate(std::mt19937*);
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H
