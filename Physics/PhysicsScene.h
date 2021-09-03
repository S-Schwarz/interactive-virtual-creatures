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

            RootMorphNode* m_rootNode = nullptr;

            PxRigidStatic* m_plane = nullptr;
            bool isInitialized = false;
        public:
            int init(PhysicsBase*,RootMorphNode*);
            void destroy();
            int simulate(bool);

            int createPlane(PxVec3 normalVec, float distance, PxMaterial* material);
            PxRigidStatic* getPlane();
            PxVec3 getCreaturePos();
            std::vector<PxArticulationLink*> getBodyParts();

            void insertNewCreature(RootMorphNode*);
            void alignChildren(std::vector<PxArticulationLink*>, std::vector<PxArticulationLink*>, PxArticulationCache*, PxArticulationCache*);

            void resetCreaturePosition();
            void rebuild();

            PhysicalCreature* getCreature();

    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_PHYSICSSCENE_H
