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

                float stddev = old * STANDARD_DEVIATION_FACTOR;

                if(stddev < STANDARD_DEVIATION_FACTOR)
                    stddev = STANDARD_DEVIATION_FACTOR;

                std::normal_distribution<> dis(old, stddev);
                auto newValue = dis(*gen);

                std::uniform_real_distribution<> dis2(0, 1);
                if(dis2(*gen) <= MUTATE_SIGN_CHANCE)
                    newValue *= -1;

                if(newValue > upperLimit)
                    newValue = upperLimit;
                else if(newValue < lowerLimit)
                    newValue = lowerLimit;

                return newValue;
            }

            static float normalize(float old, float min, float max){
            return (old - min) / (max - min);
        }
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_MUTATOR_H
