//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_SUM_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_SUM_H

#include "../Neuron.h"
#include "../../Gate.h"
#include "Neuron_TwoInputs.h"

namespace ivc{
    class Neuron_Sum : public Neuron_TwoInputs {
        private:
            void sum();
        public:
            void step() override;

    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_SUM_H
