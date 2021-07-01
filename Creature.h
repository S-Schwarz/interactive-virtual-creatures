//
// Created by st on 7/1/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_CREATURE_H
#define INTERACTIVE_VIRTUAL_CREATURES_CREATURE_H

#include <foundation/PxVec3.h>
#include "DNA.h"

namespace ivc{
    class Creature {
        private:
            physx::PxVec3 m_rootNodePos;
        public:
            Creature(DNA dna);
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_CREATURE_H
