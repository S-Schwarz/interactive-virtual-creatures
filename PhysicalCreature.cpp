//
// Created by st on 7/7/21.
//

#include "PhysicalCreature.h"

ivc::PhysicalCreature::PhysicalCreature(RootMorphNode rootNode, PxVec3 pos, PxPhysics* pxPhysics) {

    m_position = pos;
    m_physics = pxPhysics;
    m_material = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

    auto rootBody = createBox(rootNode.getDimensions()/2, m_position, PxVec3(0,0,0));

    PxVec3 parentPos = m_position;
    PxVec3 parentHalfExtents = rootNode.getDimensions()/2;

    for(auto child : rootNode.getChildren()){
        PxVec3 childHalfExtents = child->getScaledHalfExtents();
        PxVec3 childRotation = child->getOrientation();

        PxVec3 parentVec = child->getParentAnchor();
        PxVec3 childVec;

        //TODO: ???
        if(parentVec.x == 1 || parentVec.x == -1){
            childVec = PxVec3(childHalfExtents.x * parentVec.x,0,0);
        }else if(parentVec.y == 1 || parentVec.y == -1){
            childVec = PxVec3(0,childHalfExtents.y * parentVec.y,0);
        }else{
            childVec = PxVec3(0,0,childHalfExtents.z * parentVec.z);
        }

        PxVec3 parentVecModified = PxVec3(parentVec.x * parentHalfExtents.x, parentVec.y * parentHalfExtents.y, parentVec.z * parentHalfExtents.z);

        PxVec3 childPos = parentPos + parentVecModified + childVec;

        auto childBody = createBox(childHalfExtents, childPos, childRotation);

        PxTransform parentTrans(parentVecModified);
        PxTransform childTrans(-1 * childVec);
        auto d6joint = PxD6JointCreate(*m_physics, rootBody, parentTrans, childBody, childTrans);

        d6joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
        d6joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
        d6joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

        PxSpring spring(SPRING_STIFFNESS, SPRING_DAMPING);     //TODO: changeable (?)
        auto swing = child->getSwingLimits();
        auto twist = child->getTwistLimits();
        PxJointAngularLimitPair limits(twist.first, twist.second, spring);
        PxJointLimitCone cone(swing.first, swing.second, spring);
        d6joint->setSwingLimit(cone);
        d6joint->setTwistLimit(limits);

    }


}

PxRigidDynamic* ivc::PhysicalCreature::createBox(PxVec3 halfextents, PxVec3 position, PxVec3 rotation) {

    PxShape* boxShape = m_physics->createShape(PxBoxGeometry(halfextents), *m_material);
    glm::quat glmQuat = glm::quat(glm::vec3(rotation.x,rotation.y,rotation.z));
    PxQuat physxQuat = PxQuat(glmQuat.x,glmQuat.y,glmQuat.z,glmQuat.w);
    PxTransform transform(position, physxQuat);
    PxRigidDynamic* body = m_physics->createRigidDynamic(transform);
    bodyParts.push_back(body);
    body->attachShape(*boxShape);
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    boxShape->release();

    return body;
}

std::vector<PxRigidDynamic *> ivc::PhysicalCreature::getBodies() {
    return bodyParts;
}