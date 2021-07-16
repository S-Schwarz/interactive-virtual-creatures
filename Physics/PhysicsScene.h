//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_PHYSICSSCENE_H
#define INTERACTIVE_VIRTUAL_CREATURES_PHYSICSSCENE_H

#include "PxPhysicsAPI.h"
#include "PhysicsBase.h"
#include "../Body/PhysicalCreature.h"
#include "../Body/RootMorphNode.h"
#include "../Constants.h"

using namespace physx;

namespace ivc{
    class PhysicsScene {
        private:
            PhysicsBase* m_base  = nullptr;
            PxScene* m_scene = nullptr;
            PhysicalCreature* m_creature = nullptr;

            PxRigidStatic* m_plane = nullptr;
            std::vector<PxRigidDynamic*> m_rigidDynamicsVector;
            bool isInitialized = false;
        public:
            int init(PhysicsBase*,RootMorphNode);
            void destroy();
            int simulate();

            int createPlane(PxVec3 normalVec, float distance, PxMaterial* material);
            std::vector<PxRigidDynamic*> getRigidDynamics();
            PxRigidStatic* getPlane();

    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_PHYSICSSCENE_H
