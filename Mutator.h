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
            static float mutateFloat(std::mt19937* gen, float old, float upperLimit, float lowerLimit){
                if(upperLimit <= lowerLimit)
                    return old;

                std::normal_distribution<> dis(old, old * STANDARD_DEVIATION_FACTOR);
                auto newValue = dis(*gen);

                if(newValue > upperLimit)
                    newValue = upperLimit;
                else if(newValue < lowerLimit)
                    newValue = lowerLimit;

                return newValue;
            }
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_MUTATOR_H
