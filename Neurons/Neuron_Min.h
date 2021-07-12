//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_MIN_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_MIN_H

#include "Neuron.h"
#include "../Gate.h"

namespace ivc{
    class Neuron_Min : public Neuron{
    private:
        Gate *input_0, *input_1;
        float weight_0 = 1.0f;
        float weight_1 = 1.0f;

        void min();
    public:
        void step();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_MIN_H
