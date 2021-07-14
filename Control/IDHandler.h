//
// Created by st on 7/13/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_IDHANDLER_H
#define INTERACTIVE_VIRTUAL_CREATURES_IDHANDLER_H

#include <vector>

namespace ivc {
    class IDHandler {
        private:
            unsigned long m_nextNum = 0;
            std::vector<unsigned long> m_unusedNumVector;
        public:
            unsigned long getNewID();
            void freeID(unsigned long);
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_IDHANDLER_H
