//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_H

#include "../Gate.h"

namespace ivc{

    class Neuron {
        protected:
            Gate output;
            float m_outputWeight = 1.0f;
        public:
            virtual void swap();
            virtual void step();
            virtual Gate* getGate();
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_H
