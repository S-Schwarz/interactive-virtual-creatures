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

namespace ivc{

    class Neuron {
        protected:
            Gate* output;
            unsigned long m_outputID;
            float m_outputWeight = 1.0f;
        public:
            virtual void setOutput(Gate*);
            virtual void randomize(std::mt19937*);
            virtual void swap();
            virtual void step();
            virtual Neuron* copy();
            virtual Gate* getOutputGate();
            virtual std::vector<unsigned long> getGateIDs();
            virtual int bindGates(std::vector<Gate*>);
            virtual unsigned long getOutputID();
            virtual void setID(unsigned long);
            virtual void chooseRandomInputs(std::vector<unsigned long>);
            virtual void mutate(std::mt19937*);
            virtual void mutateConnections(std::mt19937*,std::vector<unsigned long>) = 0;
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_H
