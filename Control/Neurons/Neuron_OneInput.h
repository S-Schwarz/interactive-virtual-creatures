//
// Created by st on 7/18/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_ONEINPUT_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_ONEINPUT_H

#include "Neuron.h"

namespace ivc{
    class Neuron_OneInput : public Neuron {
    private:
        Gate *input_0;
        unsigned long id_input_0;
        float weight_0 = MEAN_NEURON_WEIGHT;

        //sin params
        float m_sin_amplitude = 1.0f;
        float m_sin_period = 1.0f;
        float m_sin_phase = 0.0f;
        float m_sin_vertical = 0.0f;
    public:
        Neuron_OneInput(NEURON_TYPE);
        void randomize(std::mt19937*) override;
        std::vector<unsigned long> getGateIDs() override;
        int bindGates(std::vector<Gate*>) override;
        void chooseRandomInputs(std::vector<unsigned long>) override;
        void mutate(std::mt19937*) override;
        void mutateConnections(std::mt19937*,std::vector<unsigned long>) override;

        Neuron* copy() override;
        void step() override;

        void sign();
        void abs();
        void sin();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_ONEINPUT_H
