//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
#define INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H

#include "PhysicsScene.h"

namespace ivc{
    class LiveEnvironment {
        private:
            PhysicsScene* m_scene = nullptr;
            unsigned int inactiveTime = 0;
        public:
            int init(PhysicsScene*);
            int simulate();
            void destroy();

            void setInactiveTime(unsigned int);
            void resetCreaturePosition();

            std::vector<PxArticulationLink*> getBodyParts();
            PxRigidStatic* getFloorPlane();
            void insertNewCreature(RootMorphNode* );
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
