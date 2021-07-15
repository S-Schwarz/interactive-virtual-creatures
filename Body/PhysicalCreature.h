//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_PHYSICALCREATURE_H
#define INTERACTIVE_VIRTUAL_CREATURES_PHYSICALCREATURE_H

#include "PxPhysicsAPI.h"
#include "RootMorphNode.h"
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Constants.h"

using namespace physx;

namespace ivc{

    class PhysicalCreature {
        private:
            PxVec3 m_position;
            PxPhysics* m_physics;
            PxMaterial* m_material;
            std::vector<PxRigidDynamic*> bodyParts;

            std::vector<Neuron*> m_neuronVector;
            std::vector<JointSensor*> m_sensorVector;
            std::vector<JointEffector*> m_effectorVector;
            std::map<unsigned long, Gate*> m_gateMap;

            PxRigidDynamic* createBox(PxVec3 , PxVec3 , PxVec3);
            void buildChildNodes(BaseNode*,PxVec3,PxVec3,PxRigidDynamic*,unsigned int);
        public:
            PhysicalCreature(RootMorphNode, PxVec3, PxPhysics*);
            std::vector<PxRigidDynamic*> getBodies();
            void performBrainStep();
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_PHYSICALCREATURE_H
