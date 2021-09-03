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
#include "../Physics/ContactReporter.h"
#include "../Physics/PhysicsBase.h"

#include <set>

using namespace physx;

namespace ivc{

    class PhysicalCreature {
        private:
            PxVec3 m_position;
            PxPhysics* m_physics;
            PxMaterial* m_material;
            std::vector<PxArticulationLink*> m_bodyParts;
            PxArticulationReducedCoordinate* m_articulation = nullptr;

            PxArticulationLink* m_rootLink = nullptr;

            PxArticulationCache* m_cache = nullptr;

            ContactReporter* m_reporter;

            std::vector<Neuron*> m_neuronVector;
            std::vector<JointSensor*> m_sensorVector;
            std::vector<JointEffector*> m_effectorVector;
            std::vector<ContactSensor*> m_contactVector;
            std::map<unsigned long, Gate*> m_gateMap;

            std::vector<Neuron*> m_activeNeuronVector;
            std::vector<std::pair<JointSensor*,std::vector<unsigned long>>> m_activeSensorVector;
            std::vector<std::pair<ContactSensor*,std::vector<unsigned long>>> m_activeContactVector;

            PxArticulationLink* createLink(PxArticulationLink*, PxVec3 , PxVec3 , PxVec3);
            void addContactTriggers(PxArticulationLink*, PxVec3, ContactSensor*);
            void buildChildNodes(BaseNode*,PxVec3,PxVec3,PxVec3,PxArticulationLink*);
            void buildNode(BaseNode*, PxVec3, PxVec3, PxVec3, PxArticulationLink*, PxVec3);

            void checkNeuronsForActivity();
        public:
            ~PhysicalCreature();
            PhysicalCreature(RootMorphNode*, PxVec3, PhysicsBase*);
            std::vector<PxArticulationLink*> getBodies();
            void updateContactStates();
            void performBrainStep();
            PxVec3 getPosition();
            ContactReporter* getReporter();
            void initCache();
            void updateCache();
            PxArticulationReducedCoordinate* getArticulation();
            PxArticulationLink* getRootLink();
            PxArticulationCache* getCache();

            std::vector<Neuron*> getActiveNeurons();
            std::vector<std::pair<JointSensor*,std::vector<unsigned long>>> getActiveJointSensors();
            std::vector<std::pair<ContactSensor*,std::vector<unsigned long>>> getActiveContactSensors();
            std::vector<JointEffector*> getJointEffectors();

            std::vector<Neuron*> getNeurons();

    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_PHYSICALCREATURE_H
