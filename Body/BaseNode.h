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
#include <map>

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

    class BaseNode {
        protected:
            //body
            PxVec3 m_dimension = PxVec3(MEAN_PART_SIZE,MEAN_PART_SIZE,MEAN_PART_SIZE);
            PxVec3 m_recursionAnchor;
            PxVec3 m_parentAnchor;
            NODE_SIDE m_parentSide;
            PxVec3 m_scale = PxVec3(1,1,1);
            unsigned int m_recursionLimit;
            NeuronCluster* m_localNeurons = nullptr;
            BaseNode* m_parentNode = nullptr;
            std::pair<float,float> m_jointLimitX = {-MEAN_JOINT_LIMIT, MEAN_JOINT_LIMIT};
            std::pair<float,float> m_jointLimitY = {-MEAN_JOINT_LIMIT, MEAN_JOINT_LIMIT};
            std::pair<float,float> m_jointLimitZ = {-MEAN_JOINT_LIMIT, MEAN_JOINT_LIMIT};
            //children
            std::vector<BaseNode*> m_childNodeVector;
            std::vector<NODE_SIDE> m_freeSides = {POS_X,NEG_X,POS_Y,NEG_Y,POS_Z,NEG_Z};

            bool m_reflect = false;
            bool m_isInitialized = false;
            std::mt19937* m_generator = nullptr;
        public:
            virtual ~BaseNode();
            virtual void setParent(BaseNode*);
            virtual void setLocalNeurons(NeuronCluster*);
            virtual BaseNode* copy() = 0;
            virtual void mutateBodyAndNeurons();
            virtual void mutateNewBodyAndNewNeurons();
            virtual void mutateNeuralConnections();
            virtual unsigned int getNumberOfParts();
            virtual std::vector<BaseNode*> getChildren();
            virtual void setChildren(std::vector<BaseNode*>);
            virtual int getRecursionLimit();
            virtual PxVec3 getDimensions();
            virtual PxVec3 getHalfExtents();
            virtual PxVec3 getOrientation();
            virtual PxVec3 getParentAnchor();
            virtual BaseNode* getParentNode();
            virtual PxVec3 getScale();
            virtual std::pair<float,float> getSwingLimitsY();
            virtual std::pair<float,float> getSwingLimitsZ();
            virtual std::pair<float,float> getTwistLimits();
            virtual NODE_SIDE occupyRandomSide();
            virtual int setSideAsOccupied(NODE_SIDE);
            virtual void setSideAsFree(NODE_SIDE);
            virtual bool isFree(NODE_SIDE side);
            virtual IDHandler* getIDHandler();
            virtual NeuronCluster* getLocalNeurons();
            virtual NeuronCluster* getBrain();
            virtual std::vector<unsigned long> getAllAdjacentOutputs();
            virtual std::vector<unsigned long> getAllChildOutputs();
            virtual void addNeuralConnections();
            virtual std::mt19937* getGenerator();
            virtual void setGenerator(std::mt19937*);
            virtual BaseNode* reflect();
            virtual void chooseNewNeuronIDs(std::map<unsigned long,unsigned long>*);
            virtual void rewireInputs(std::map<unsigned long,unsigned long>*);
            virtual void setParentAnchor(PxVec3);
            virtual NODE_SIDE getParentSide();
            virtual void setParentSide(NODE_SIDE);
            virtual PxVec3 flipAnchor(PxVec3);
            virtual NODE_SIDE getOppositeSide(NODE_SIDE);
            virtual void reflectAlongAxis(NODE_SIDE);
            virtual bool shouldBeReflected();
            virtual bool setReflectionFlag();
            virtual std::string getParentSideAsString();
    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H
