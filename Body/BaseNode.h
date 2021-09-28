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

    enum JOINT_TYPE{
        SWING1,
        SWING2,
        TWIST
    };

    class BaseNode {
        protected:
            //body
            PxVec3 m_orientation = PxVec3(0,0,0);
            PxVec3 m_dimension = PxVec3(MEAN_PART_SIZE,MEAN_PART_SIZE,MEAN_PART_SIZE);
            PxVec3 m_parentAnchor = PxVec3(0,0,0);
            NODE_SIDE m_parentSide = NONE;
            PxVec3 m_scale = PxVec3(1,1,1);
            NeuronCluster* m_localNeurons = nullptr;
            BaseNode* m_parentNode = nullptr;
            JOINT_TYPE m_jointType;
            std::pair<float,float> m_jointLimits= {-MEAN_JOINT_LIMIT, MEAN_JOINT_LIMIT};
            //children
            std::vector<BaseNode*> m_childNodeVector;
            std::vector<NODE_SIDE> m_freeSides = {POS_X,NEG_X,POS_Y,NEG_Y,POS_Z,NEG_Z};

            bool m_reflect = false;
            std::mt19937* m_generator = nullptr;

            NeuronCluster* m_brain = nullptr;
            IDHandler* m_idHandler = nullptr;

            bool m_isRoot = false;

            void setJointType(JOINT_TYPE);
            void chooseNewJointType();
        public:
            void init(bool, std::mt19937*, BaseNode*);
            BaseNode* copy();
            ~BaseNode();

            void setParent(BaseNode*);
            void setLocalNeurons(NeuronCluster*);
            void setChildren(std::vector<BaseNode*>);
            int setSideAsOccupied(NODE_SIDE);
            void setSideAsFree(NODE_SIDE);
            void setParentAnchor(PxVec3);
            void setGenerator(std::mt19937*);
            void setParentSide(NODE_SIDE);
            void setReflectionFlag();
            void setBrain(NeuronCluster*);

            unsigned int getNumberOfParts();
            std::vector<BaseNode*> getChildren();
            PxVec3 getDimensions();
            PxVec3 getHalfExtents();
            PxVec3 getOrientationInDegrees();
            PxVec3 getParentAnchor();
            BaseNode* getParentNode();
            PxVec3 getScale();
            std::pair<float,float> getJointLimits();
            JOINT_TYPE getJointType();
            IDHandler* getIDHandler();
            NeuronCluster* getLocalNeurons();
            NeuronCluster* getBrain();
            std::vector<unsigned long> getAllAdjacentOutputs();
            std::vector<unsigned long> getAllChildOutputs();
            std::mt19937* getGenerator();
            NODE_SIDE getParentSide();
            NODE_SIDE getOppositeSide(NODE_SIDE);
            std::string getParentSideAsString();
            PxVec3 getAnchorPosition(std::mt19937* gen);
            bool shouldBeReflected();

            void mutateBodyAndNeurons(bool);
            void mutateNewBodyAndNewNeurons(bool);
            void mutateNeuralConnections();
            void addNeuralConnections();

            NODE_SIDE occupyRandomSide();
            bool isFree(NODE_SIDE side);

            BaseNode* reflect();
            void chooseNewNeuronIDs(std::map<unsigned long,unsigned long>*);
            void rewireInputs(std::map<unsigned long,unsigned long>*);
            PxVec3 flipAnchor(PxVec3);
            void reflectAlongAxis(NODE_SIDE);

    };

}



#endif //INTERACTIVE_VIRTUAL_CREATURES_BASENODE_H
