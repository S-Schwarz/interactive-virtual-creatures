//
// Created by st on 7/7/21.
//

#include "BaseNode.h"

unsigned int ivc::BaseNode::getNumberOfParts() {

    unsigned int number = 1;
    for(auto child : m_childNodeVector){
        number += child.getNumberOfParts();
    }

    return number;

}