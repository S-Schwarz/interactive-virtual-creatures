//
// Created by st on 7/13/21.
//

#include "IDHandler.h"

unsigned long ivc::IDHandler::getNewID() {
    //if(m_unusedNumVector.empty()) TODO: reuse discarded IDs (?)
        return m_nextNum++;

    auto unused =m_unusedNumVector.back();
    m_unusedNumVector.pop_back();
    return unused;
}

void ivc::IDHandler::freeID(unsigned long id) {
    m_unusedNumVector.push_back(id);
}