//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_PHYSICALCREATURE_H
#define INTERACTIVE_VIRTUAL_CREATURES_PHYSICALCREATURE_H

#include "PxPhysicsAPI.h"
#include "BaseNode.h"
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Constants.h"
#include "../Physics/ContactReporter.h"
#include "../Physics/PhysicsBase.h"

#include <set>
#include <memory>

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

            std::shared_ptr<ContactReporter> m_reporter;

            std::vector<std::shared_ptr<Neuron>> m_neuronVector;
            std::vector<std::shared_ptr<JointSensor>> m_sensorVector;
            std::vector<std::shared_ptr<JointEffector>> m_effectorVector;
            std::vector<std::shared_ptr<ContactSensor>> m_contactVector;
            std::map<unsigned long, std::shared_ptr<Gate>> m_gateMap;

            std::vector<std::shared_ptr<Neuron>> m_activeNeuronVector;
            std::vector<std::pair<std::shared_ptr<JointSensor>,unsigned long>> m_activeSensorVector;
            std::vector<std::pair<std::shared_ptr<ContactSensor>,std::vector<unsigned long>>> m_activeContactVector;

            PxArticulationLink* createLink(PxArticulationLink*, PxVec3 , PxVec3 , PxVec3);
            void addContactTriggers(PxArticulationLink*, PxVec3, std::shared_ptr<ContactSensor>);
            void buildChildNodes(std::shared_ptr<BaseNode>,PxVec3,PxVec3,PxVec3,PxArticulationLink*);
            void buildNode(std::shared_ptr<BaseNode>, PxVec3, PxVec3, PxVec3, PxArticulationLink*, PxVec3);

            void checkNeuronsForActivity();
        public:
            ~PhysicalCreature();
            PhysicalCreature(std::shared_ptr<BaseNode>, PxVec3, std::shared_ptr<PhysicsBase>, std::shared_ptr<EvoConfig>);
            std::vector<PxArticulationLink*> getBodies();
            void updateContactStates();
            void performBrainStep();
            PxVec3 getPosition();
            std::shared_ptr<ContactReporter> getReporter();
            void initCache();
            void updateCache();
            PxArticulationReducedCoordinate* getArticulation();
            PxArticulationLink* getRootLink();
            PxArticulationCache* getCache();

            std::vector<std::shared_ptr<Neuron>> getActiveNeurons();
            std::vector<std::pair<std::shared_ptr<JointSensor>,unsigned long>> getActiveJointSensors();
            std::vector<std::pair<std::shared_ptr<ContactSensor>,std::vector<unsigned long>>> getActiveContactSensors();
            std::vector<std::shared_ptr<JointEffector>> getJointEffectors();
            std::map<unsigned long, std::shared_ptr<Gate>> getGateMap();

            std::vector<std::shared_ptr<Neuron>> getNeurons();

    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_PHYSICALCREATURE_H
