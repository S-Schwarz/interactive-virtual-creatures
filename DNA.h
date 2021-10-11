//
// Created by st on 10/11/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_DNA_H
#define INTERACTIVE_VIRTUAL_CREATURES_DNA_H

#include "Body/BaseNode.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

namespace ivc{
    class NodeDNA{
        private:
            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive &a, const unsigned version){
                a & ID & parentID & oriX & oriY & oriZ & dimX & dimY & dimZ & anchorX & anchorY & anchorZ & pSide &
                joint & jointLimLow & jointLimHigh & free & reflect & isRoot & local & brain & jointSensor & contactSensor & effector &
                localOutputs & localInputs & brainOutputs & brainInputs;
            }
        public:
            int ID;
            int parentID;

            float oriX, oriY, oriZ;
            float dimX, dimY, dimZ;
            float anchorX, anchorY, anchorZ;
            int pSide;
            int joint;
            float jointLimLow, jointLimHigh;
            std::vector<int> free;
            bool reflect;
            bool isRoot;

            std::vector<Neuron> local;
            std::vector<unsigned long> localOutputs;
            std::vector<unsigned long> localInputs;

            std::vector<Neuron> brain;
            std::vector<unsigned long> brainOutputs;
            std::vector<unsigned long> brainInputs;

            JointSensor jointSensor;
            ContactSensor contactSensor;
            JointEffector effector;

            NodeDNA()=default;
            NodeDNA(BaseNode*);
            std::shared_ptr<BaseNode> toNode();
    };

    class DNA {
        private:
            IDHandler idHandler;

            std::vector<NodeDNA> nodeVec;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive &a, const unsigned version){
                a & idHandler & nodeVec;
            }

        public:
            DNA()=default;
            DNA(BaseNode*);
            std::shared_ptr<BaseNode> toRootNode();

    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_DNA_H
