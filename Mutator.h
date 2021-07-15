//
// Created by st on 7/15/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_MUTATOR_H
#define INTERACTIVE_VIRTUAL_CREATURES_MUTATOR_H

#include <random>
#include "Constants.h"

namespace ivc{
    class Mutator{
        private:

        public:
            static float mutateFloat(std::mt19937* gen, float old){
                std::normal_distribution<> dis(old, old * STANDARD_DEVIATION_FACTOR);
                return dis(*gen);
            }
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_MUTATOR_H
