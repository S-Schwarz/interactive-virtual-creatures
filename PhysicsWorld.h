//
// Created by st on 6/25/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_PHYSICSWORLD_H
#define INTERACTIVE_VIRTUAL_CREATURES_PHYSICSWORLD_H

#include "PxPhysicsAPI.h"

#include <vector>

using namespace physx;

namespace ivc {

    static PxDefaultErrorCallback s_defaultErrorCallback;
    static PxDefaultAllocator s_defaultAllocatorCallback;

    class PhysicsWorld {
        private:
            PxFoundation* m_foundation = nullptr;
            PxPhysics* m_physics = nullptr;
            PxScene* m_scene = nullptr;

            PxRigidStatic* m_plane = nullptr;
            std::vector<PxRigidDynamic*> m_rigidDynamicsVector;
            bool isInitialized = false;
            float m_stepSize = 0.01f;
        public:
            int init();
            int simulate();
            int destroy();

            int createBox(PxVec3 halfextents, PxVec3 position, PxVec3 rotation, PxMaterial* material);
            int createPlane(PxVec3 normalVec, float distance, PxMaterial* material);

            std::vector<PxRigidDynamic*> getRigidDynamics();
            PxRigidStatic* getPlane();
            float getStepSize();
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_PHYSICSWORLD_H
