//
// Created by st on 7/16/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
#define INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H

#include "PhysicsScene.h"

namespace ivc{
    class LiveEnvironment {
        private:
            std::vector<std::shared_ptr<PhysicsScene>> m_sceneVec;
            std::shared_ptr<BaseNode> m_currentBest = nullptr;
            std::vector<PxVec3> m_currentBestPath;
            std::shared_ptr<PhysicsScene> m_bestScene = nullptr;
            std::shared_ptr<PhysicsBase> m_base = nullptr;
            std::shared_ptr<EvoConfig> m_config = nullptr;
        public:
            int init(std::shared_ptr<PhysicsBase>, std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>>,std::shared_ptr<EvoConfig>);
            int simulate();

            void setInactiveTime(unsigned int);
            void resetCreaturePosition();
            void clean();

            std::vector<std::pair<std::vector<PxArticulationLink*>, std::vector<PxVec3>>> getBodyParts();
            std::vector<PxRigidStatic*> getObjVec();
            PxRigidStatic* getFloorPlane();
            void insertNewCreatures(std::vector<std::pair<std::shared_ptr<BaseNode>,std::pair<float, std::vector<PxVec3>>>>);

            std::shared_ptr<PhysicalCreature> getBestCreature();
            std::shared_ptr<BaseNode> getBestNode();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
