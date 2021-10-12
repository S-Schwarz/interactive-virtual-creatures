//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_PHYSICSSCENE_H
#define INTERACTIVE_VIRTUAL_CREATURES_PHYSICSSCENE_H

#include "PxPhysicsAPI.h"
#include "PhysicsBase.h"
#include "../Body/PhysicalCreature.h"
#include "../Body/BaseNode.h"
#include "../Constants.h"
#include <memory>

using namespace physx;

namespace ivc{
    class PhysicsScene {
        private:
            std::shared_ptr<PhysicsBase> m_base  = nullptr;
            PxScene* m_scene = nullptr;
            std::shared_ptr<PhysicalCreature> m_creature = nullptr;
            std::shared_ptr<EvoConfig> m_config = nullptr;

            std::shared_ptr<BaseNode> m_rootNode = nullptr;

            PxRigidStatic* m_plane = nullptr;
            std::vector<PxRigidStatic*> m_objVec;
            bool m_isInitialized = false;
            PxDefaultCpuDispatcher* m_cpuDispatcher = nullptr;
        public:
            int init(std::shared_ptr<PhysicsBase>,std::shared_ptr<BaseNode>, std::shared_ptr<EvoConfig>);
            ~PhysicsScene(){destroy();};
            void destroy();
            int simulate(bool);

            int createPlane(PxVec3 normalVec, float distance, PxMaterial* material);
            PxRigidStatic* getPlane();
            PxVec3 getCreaturePos();
            std::vector<PxArticulationLink*> getBodyParts();

            void insertNewCreature(std::shared_ptr<BaseNode>);
            void alignChildren(std::vector<PxArticulationLink*>, std::vector<PxArticulationLink*>, PxArticulationCache*, PxArticulationCache*);

            void resetCreaturePosition();
            void rebuild();

            std::shared_ptr<BaseNode> getRootNode();

            std::shared_ptr<PhysicalCreature> getCreature();
            std::vector<PxRigidStatic*> getObjVec();

    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_PHYSICSSCENE_H
