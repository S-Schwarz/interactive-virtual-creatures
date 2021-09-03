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
        PxArticulationCache* m_cache = nullptr;
        PxArticulationLink* m_link = nullptr;
        Gate *m_output_0, *m_output_1, *m_output_2;
        unsigned long m_id_output_0, m_id_output_1, m_id_output_2;
        float m_weight_0 = 1.0f;
        float m_weight_1 = 1.0f;
        float m_weight_2 = 1.0f;
    public:
        void setOutputGates(std::vector<Gate*>);
        void randomize(std::mt19937*);
        void step();
        void swap();
        void setCache(PxArticulationCache*);
        void setLink(PxArticulationLink*);
        void setIDs(unsigned long, unsigned long, unsigned long);
        void setIDs(std::vector<unsigned long>);
        std::vector<unsigned long> getOutputIDs();
        std::vector<Gate*> getOutputGates();
        void mutate(std::mt19937*);
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTSENSOR_H
