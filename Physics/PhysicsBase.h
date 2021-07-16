//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_PHYSICSBASE_H
#define INTERACTIVE_VIRTUAL_CREATURES_PHYSICSBASE_H

#include "PxPhysicsAPI.h"

using namespace physx;

namespace ivc{

    static PxDefaultErrorCallback s_defaultErrorCallback;
    static PxDefaultAllocator s_defaultAllocatorCallback;

    class PhysicsBase {
        private:
            PxFoundation* m_foundation = nullptr;
            PxPhysics* m_physics = nullptr;
            PxMaterial* m_material = nullptr;
            bool isInitialized = false;
        public:
            int init();
            void destroy();
            PxPhysics* getPhysics();
            PxMaterial* getMaterial();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_PHYSICSBASE_H
