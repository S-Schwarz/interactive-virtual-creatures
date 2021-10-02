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
            PhysicsScene* m_bestScene = nullptr;
            PhysicsBase* m_base = nullptr;
        public:
            int init(PhysicsBase*, std::vector<std::pair<std::shared_ptr<BaseNode>,float>>);
            int simulate();
            void destroy();

            void setInactiveTime(unsigned int);
            void resetCreaturePosition();

            std::vector<std::pair<std::vector<PxArticulationLink*>, bool>> getBodyParts();
            PxRigidStatic* getFloorPlane();
            void insertNewCreatures(std::vector<std::pair<std::shared_ptr<BaseNode>,float>>);

            PhysicalCreature* getBestCreature();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_LIVEENVIRONMENT_H
