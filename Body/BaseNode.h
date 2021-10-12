//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H
#define INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H

#include <vector>
#include "PxPhysicsAPI.h"
#include "../Control/NeuronCluster.h"
#include <algorithm>
#include <random>
#include "../Mutator.h"
#include "../Evolution/EvoConfig.h"
#include <map>
#include <memory>
#include <set>

using namespace physx;

namespace ivc{

    enum NODE_SIDE{
        POS_X,
        POS_Y,
        POS_Z,
        NEG_X,
        NEG_Y,
        NEG_Z,
        NONE
    };

    enum JOINT_TYPE{
        SWING1,
        SWING2,
        TWIST
    };

class BaseNode : public std::enable_shared_from_this<BaseNode> {
        protected:
            //body
            PxVec3 m_orientation = PxVec3(0,0,0);
            PxVec3 m_dimension = PxVec3(MEAN_PART_SIZE,MEAN_PART_SIZE,MEAN_PART_SIZE);
            PxVec3 m_parentAnchor = PxVec3(0,0,0);
            NODE_SIDE m_parentSide = NONE;
            std::shared_ptr<NeuronCluster> m_localNeurons = nullptr;
            BaseNode* m_parentNode;
            JOINT_TYPE m_jointType;
            std::pair<float,float> m_jointLimits= {-MEAN_JOINT_LIMIT, MEAN_JOINT_LIMIT};
            //children
            std::vector<std::shared_ptr<BaseNode>> m_childNodeVector;
            std::vector<NODE_SIDE> m_freeSides = {POS_X,NEG_X,POS_Y,NEG_Y,POS_Z,NEG_Z};

            bool m_reflect = false;
            std::shared_ptr<std::mt19937> m_generator = nullptr;

            std::shared_ptr<NeuronCluster> m_brain = nullptr;
            std::shared_ptr<IDHandler> m_idHandler = nullptr;

            bool m_isRoot = false;

            void chooseNewJointType();
            std::vector<std::shared_ptr<Neuron>> collectNeuronCopies();
            std::vector<std::shared_ptr<JointEffector>> collectEffectorCopies();
            std::vector<std::shared_ptr<JointSensor>> collectJointSensorCopies();
            std::vector<std::shared_ptr<ContactSensor>> collectContactSensorCopies();
        public:
            void init(bool, std::shared_ptr<std::mt19937>, BaseNode*, std::shared_ptr<EvoConfig>);
            std::shared_ptr<BaseNode> copy();

            void setParent(BaseNode*);
            void setLocalNeurons(std::shared_ptr<NeuronCluster>);
            void setChildren(std::vector<std::shared_ptr<BaseNode>>);
            int setSideAsOccupied(NODE_SIDE);
            void setSideAsFree(NODE_SIDE);
            void setParentAnchor(PxVec3);
            void setGenerator(std::shared_ptr<std::mt19937>);
            void setParentSide(NODE_SIDE);
            void setReflectionFlag();
            void setBrain(std::shared_ptr<NeuronCluster>);
            void addChild(std::shared_ptr<BaseNode>);
            void setOrientation(PxVec3);
            void setDimension(PxVec3);
            void setJointType(JOINT_TYPE);
            void setJointTypeDirectly(JOINT_TYPE);
            void setJointLimits(float,float);
            void setAsRoot();
            void emptyFreeSideVector();
            void setIDHandler(std::shared_ptr<IDHandler>);

            unsigned int getNumberOfParts();
            std::vector<unsigned int> getNeuronActivity();
            std::vector<std::shared_ptr<BaseNode>> getChildren();
            PxVec3 getDimensions();
            PxVec3 getHalfExtents();
            PxVec3 getOrientationInDegrees();
            PxVec3 getOrientationInRadians();
            PxVec3 getParentAnchor();
            BaseNode* getParentNode();
            std::pair<float,float> getJointLimits();
            JOINT_TYPE getJointType();
            std::shared_ptr<IDHandler> getIDHandler();
            std::shared_ptr<NeuronCluster> getLocalNeurons();
            std::shared_ptr<NeuronCluster> getBrain();
            std::vector<unsigned long> getAllAdjacentOutputs();
            std::vector<unsigned long> getAllChildOutputs();
            std::shared_ptr<std::mt19937> getGenerator();
            NODE_SIDE getParentSide();
            NODE_SIDE getOppositeSide(NODE_SIDE);
            std::string getParentSideAsString();
            PxVec3 getAnchorPosition(std::shared_ptr<std::mt19937> gen);
            std::vector<NODE_SIDE> getFreeSides();
            bool shouldBeReflected();
            bool isRoot();

            void mutateBodyAndNeurons(bool, std::shared_ptr<EvoConfig>);
            void mutateNewBodyAndNewNeurons(bool, std::shared_ptr<EvoConfig>);
            void mutateNeuralConnections(std::shared_ptr<EvoConfig>);
            void addNeuralConnections();

            NODE_SIDE occupyRandomSide();
            bool isFree(NODE_SIDE side);

            std::shared_ptr<BaseNode> reflect();
            void chooseNewNeuronIDs(std::map<unsigned long,unsigned long>*);
            void rewireInputs(std::map<unsigned long,unsigned long>*);
            PxVec3 flipAnchor(PxVec3);
            void reflectAlongAxis(NODE_SIDE);
            void chooseNewAnchorSide();
            void changeAnchorSideTo(NODE_SIDE);

            void replaceChild(std::shared_ptr<BaseNode>, std::shared_ptr<BaseNode>);
            std::shared_ptr<BaseNode> getRandomNode(std::shared_ptr<std::mt19937>);
            std::shared_ptr<BaseNode> getRandomChild(std::shared_ptr<std::mt19937>);
            NODE_SIDE getRandomFreeSide(std::shared_ptr<std::mt19937>);

    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H
