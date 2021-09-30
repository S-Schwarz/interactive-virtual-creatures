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
#include "../../Evolution/EvoConfig.h"

using namespace physx;

namespace ivc{
    class JointEffector {
    private:
        PxArticulationJointReducedCoordinate* m_joint = nullptr;
        Gate *m_input_0, *m_input_1, *m_input_2;
        unsigned long m_id_input_0, m_id_input_1, m_id_input_2;
        float m_weight_0 = 1.0f;
        float m_weight_1 = 1.0f;
        float m_weight_2 = 1.0f;
        std::vector<PxVec3> m_lastValues;
        unsigned int m_lastValuesIndex = 0;

        void addToHistory(PxVec3);
        PxVec3 getAverageValue();
    public:
        void step();
        void setJoint(PxArticulationJointReducedCoordinate*);
        std::vector<unsigned long> getGateIDs();
        int bindGates(std::vector<Gate*>);
        void chooseRandomInputs(std::vector<unsigned long>);
        void mutate(std::mt19937*,EvoConfig*);
        void mutateConnections(std::mt19937*,std::vector<unsigned long>,EvoConfig*);

        std::vector<unsigned long> getInputs();
        void setInputs(std::vector<unsigned long>);
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_JOINTEFFECTOR_H
