//
// Created by st on 7/1/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_BODYCONNECTION_H
#define INTERACTIVE_VIRTUAL_CREATURES_BODYCONNECTION_H

#include <foundation/PxVec3.h>
#include <vector>

class BodyConnection {
    private:
        physx::PxVec3 m_position;
        physx::PxVec3 m_orientation;
        physx::PxVec3 m_scale;
        //TODO: reflection
        bool m_terminalOnly;
    public:

};


#endif //INTERACTIVE_VIRTUAL_CREATURES_BODYCONNECTION_H
