//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
#define INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H

#include "PhysicsScene.h"

namespace ivc{
    class LiveEnvironment {
        private:
            std::vector<PhysicsScene*> m_sceneVec;
            std::shared_ptr<BaseNode> m_currentBest = nullptr;
            std::vector<PxVec3> m_currentBestPath;
            PhysicsScene* m_bestScene = nullptr;
            PhysicsBase* m_base = nullptr;
            EvoConfig* m_config = nullptr;
        public:
            int init(PhysicsBase*, std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>>,EvoConfig*);
            int simulate();
            void destroy();

            void setInactiveTime(unsigned int);
            void resetCreaturePosition();

            std::vector<std::pair<std::vector<PxArticulationLink*>, std::vector<PxVec3>>> getBodyParts();
            std::vector<PxRigidStatic*> getObjVec();
            PxRigidStatic* getFloorPlane();
            void insertNewCreatures(std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>>);

            PhysicalCreature* getBestCreature();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
