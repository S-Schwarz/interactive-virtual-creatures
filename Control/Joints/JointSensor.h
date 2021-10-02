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
#include "../../Evolution/EvoConfig.h"

using namespace physx;

namespace ivc{
    class JointSensor {
    private:
        PxArticulationCache* m_cache = nullptr;
        PxArticulationLink* m_link = nullptr;
        Gate *m_output;
        unsigned long m_id_output;
    public:
        void setOutputGate(Gate*);
        void step();
        void swap();
        void setCache(PxArticulationCache*);
        void setLink(PxArticulationLink*);
        void setID(unsigned long);
        unsigned long getOutputID();
        Gate* getOutputGate();
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H
