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
#include "../Evolution/EvoConfig.h"
#include <stdexcept>
#include <cmath>
#include <memory>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

namespace ivc{
    enum NEURON_TYPE{
        //no input
        CONSTANT,
        SINE_OSCI,
        SAW_OSCI,
        //one input
        SIGN,
        ABS,
        SIN,
        SAW,
        DELAY,
        //two inputs
        SUM,
        MAX,
        MIN,
        PRODUCT,
        DIVIDE,
        SUM_THRESHOLD,
        GREATER_THAN,
        //three inputs
        IF_THEN_ELSE,
        COUNT
    };

    class Neuron {
        private:
            NEURON_TYPE m_type;

            std::shared_ptr<Gate> m_output;
            unsigned long m_outputID;

            unsigned int m_numberInputs;
            std::vector<unsigned long> m_inputIDs;
            std::vector<std::shared_ptr<Gate>> m_inputGates;
            std::vector<float> m_inputWeights;

            //no inputs
                //constant neuron
                float m_constant = 1.0f;

            //one input
                //sine/saw neuron
                float m_amplitude = 1.0f;
                float m_period = 1.0f;
                float m_phase = 0.0f;
                float m_vertical = 0.0f;

                //osci neuron
                float m_osci_stepSize = 1.0f;
                float m_osci_offset = 0.0f;

                //delay memory
                float m_delayed_input = 0.0f;
            //two inputs
                //sum_threshold neuron
                float m_threshold = 0.0f;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive &a, const unsigned version){
                a & m_type & m_outputID & m_numberInputs & m_inputIDs & m_inputWeights &
                m_constant & m_amplitude & m_period & m_phase & m_vertical & m_osci_stepSize & m_osci_offset & m_delayed_input & m_threshold;
            }
        public:
            Neuron()=default;
            Neuron(std::shared_ptr<std::mt19937>, std::shared_ptr<EvoConfig>);
            void setOutput(std::shared_ptr<Gate>);
            void swap();
            void step();
            std::shared_ptr<Neuron> copy();
            std::shared_ptr<Gate> getOutputGate();
            std::vector<unsigned long> getGateIDs();
            int bindGates(std::vector<std::shared_ptr<Gate>>);
            unsigned long getOutputID();
            void setID(unsigned long);
            void chooseRandomInputs(std::vector<unsigned long>);
            void mutate(std::shared_ptr<std::mt19937>,bool, std::shared_ptr<EvoConfig>);
            void mutateConnections(std::shared_ptr<std::mt19937>,std::vector<unsigned long>,std::shared_ptr<EvoConfig>);

            std::vector<unsigned long> getInputs();
            void setInputs(std::vector<unsigned long>);

            NEURON_TYPE getType();

            //no inputs
            void sine_osci();
            void saw_osci();
            //one input
            void sign();
            void abs();
            void sin();
            void saw();
            void delay();
            //two inputs
            void sum();
            void min();
            void max();
            void product();
            void divide();
            void sum_threshold();
            void greater_than();
            //three inputs
            void if_then_else();
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_H
