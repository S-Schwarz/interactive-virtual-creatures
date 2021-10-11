//
// Created by st on 7/13/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_IDHANDLER_H
#define INTERACTIVE_VIRTUAL_CREATURES_IDHANDLER_H

#include <vector>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

namespace ivc {
    class IDHandler {
        private:
            unsigned long m_nextNum = 0;
            std::vector<unsigned long> m_unusedNumVector;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive &a, const unsigned version){
                a & m_nextNum & m_unusedNumVector;
            }
        public:
            unsigned long getNewID();
            void freeID(unsigned long);
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_IDHANDLER_H
