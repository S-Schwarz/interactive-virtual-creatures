//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_H

#include "../Gate.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include "../../Constants.h"
#include "../../Mutator.h"
#include <stdexcept>
#include <cmath>

namespace ivc{
    enum NEURON_TYPE{
        SUM,
        MAX,
        MIN,
        PRODUCT,
        DIVIDE,
        SUM_THRESHOLD,
        GREATER_THAN,
        SIGN,
        ABS,
        SIN,
        CONSTANT,
        COUNT
    };

    class Neuron {
        protected:
            NEURON_TYPE m_type;
            Gate* output;
            unsigned long m_outputID;
            float m_outputWeight = MEAN_NEURON_WEIGHT;
        public:
            virtual void setOutput(Gate*);
            virtual void swap();
            virtual void step();
            virtual Neuron* copy();
            virtual Gate* getOutputGate();
            virtual std::vector<unsigned long> getGateIDs();
            virtual int bindGates(std::vector<Gate*>);
            virtual unsigned long getOutputID();
            virtual void setID(unsigned long);
            virtual void chooseRandomInputs(std::vector<unsigned long>);
            virtual void mutate(std::mt19937*,bool);
            virtual void mutateConnections(std::mt19937*,std::vector<unsigned long>) = 0;
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_H
