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
            unsigned int m_inactiveTime = 0;
        public:
            int init(PhysicsScene*);
            int simulate();
            void destroy();

            void setInactiveTime(unsigned int);
            void resetCreaturePosition();

            std::vector<PxArticulationLink*> getBodyParts();
            PxRigidStatic* getFloorPlane();
            void insertNewCreature(BaseNode* );

            PhysicalCreature* getCreature();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
