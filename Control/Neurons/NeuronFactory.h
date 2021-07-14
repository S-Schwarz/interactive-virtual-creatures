//
// Created by st on 7/14/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURONFACTORY_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURONFACTORY_H

#include "Neuron.h"
#include "TwoInputs/Neuron_Max.h"
#include "TwoInputs/Neuron_Min.h"
#include "TwoInputs/Neuron_Sum.h"
#include <random>

namespace ivc{
    enum NEURON_TYPE{
        SUM,
        MAX,
        MIN,
        COUNT
    };

    class NeuronFactory {
        private:

        public:
            static Neuron* createRandomNeuron();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONFACTORY_H
