//
// Created by st on 7/14/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_TWOINPUTS_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_TWOINPUTS_H

#include "Neuron.h"
namespace ivc{
    class Neuron_TwoInputs : public Neuron {
        protected:
            Gate *input_0, *input_1;
            unsigned long id_input_0, id_input_1;
            float weight_0 = 1.0f;
            float weight_1 = 1.0f;

            float m_threshold = 0.0f;
        public:
            Neuron_TwoInputs(NEURON_TYPE);
            void randomize(std::mt19937*) override;
            std::vector<unsigned long> getGateIDs() override;
            int bindGates(std::vector<Gate*>) override;
            void chooseRandomInputs(std::vector<unsigned long>) override;
            void mutate(std::mt19937*) override;
            void mutateConnections(std::mt19937*,std::vector<unsigned long>) override;

            Neuron* copy() override;
            void step() override;

            void sum();
            void min();
            void max();
            void product();
            void divide();
            void sum_threshold();
            void greater_than();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_TWOINPUTS_H