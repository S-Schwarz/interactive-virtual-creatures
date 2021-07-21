//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_H

#include "Gate.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include "../Constants.h"
#include "../Mutator.h"
#include <stdexcept>
#include <cmath>

namespace ivc{
    enum NEURON_TYPE{
        //no input
        CONSTANT,
        //one input
        SIGN,
        ABS,
        SIN,
        SINE_OSCI,
        //two inputs
        SUM,
        MAX,
        MIN,
        PRODUCT,
        DIVIDE,
        SUM_THRESHOLD,
        GREATER_THAN,
        COUNT
    };

    class Neuron {
        private:
            NEURON_TYPE m_type;

            Gate* output;
            unsigned long m_outputID;
            float m_outputWeight = MEAN_NEURON_WEIGHT;

            unsigned int m_numberInputs;
            std::vector<unsigned long> m_inputIDs;
            std::vector<Gate*> m_inputGates;
            std::vector<float> m_inputWeights;

            //no inputs
                //constant neuron
                float m_constant = 1.0f;

            //one input
                //sine neuron
                float m_sin_amplitude = 1.0f;
                float m_sin_period = 1.0f;
                float m_sin_phase = 0.0f;
                float m_sin_vertical = 0.0f;

                //sine osci
                float m_osci_stepSize = 1.0f;
                float m_osci_offset = 0.0f;

            //two inputs
                //sum_threshold neuron
                float m_threshold = 0.0f;
        public:
            Neuron(std::mt19937*);
            void setOutput(Gate*);
            void swap();
            void step();
            Neuron* copy();
            Gate* getOutputGate();
            std::vector<unsigned long> getGateIDs();
            int bindGates(std::vector<Gate*>);
            unsigned long getOutputID();
            void setID(unsigned long);
            void chooseRandomInputs(std::vector<unsigned long>);
            void mutate(std::mt19937*,bool);
            void mutateConnections(std::mt19937*,std::vector<unsigned long>);

            //one input
            void sign();
            void abs();
            void sin();
            void sine_osci();
            //two inputs
            void sum();
            void min();
            void max();
            void product();
            void divide();
            void sum_threshold();
            void greater_than();
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_H
