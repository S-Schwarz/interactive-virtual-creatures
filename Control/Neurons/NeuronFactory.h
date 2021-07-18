//
// Created by st on 7/14/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURONFACTORY_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURONFACTORY_H

#include "Neuron.h"
#include "Neuron_TwoInputs.h"
#include "Neuron_ZeroInputs.h"
#include <random>

namespace ivc{

    class NeuronFactory {
        private:

        public:
            static Neuron* createRandomNeuron(std::mt19937*);
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONFACTORY_H
