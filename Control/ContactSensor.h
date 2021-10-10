//
// Created by st on 7/21/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_CONTACTSENSOR_H
#define INTERACTIVE_VIRTUAL_CREATURES_CONTACTSENSOR_H

#include "Gate.h"
#include <vector>
#include <random>
#include <memory>

namespace ivc{
    class ContactSensor {
        private:
            std::vector<std::shared_ptr<Gate>> m_outputGates;
            std::vector<unsigned long> m_outputIDs;
            std::vector<bool> m_hasContact;
        public:
            void initContactVec();
            void setOutputGates(std::vector<std::shared_ptr<Gate>>);
            void step();
            void swap();
            void setIDs(std::vector<unsigned long>);
            std::vector<unsigned long> getOutputIDs();

            bool contains(std::string);
            void setState(std::string, bool);
            std::vector<std::shared_ptr<Gate>> getOutputGates();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_CONTACTSENSOR_H
