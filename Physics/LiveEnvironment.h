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
        public:
            int init(PhysicsScene*);
            int simulate();
            void destroy();

            std::vector<PxArticulationLink*> getBodyParts();
            PxRigidStatic* getFloorPlane();
            void insertNewCreature(RootMorphNode* );
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
