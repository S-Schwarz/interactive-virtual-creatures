//
// Created by st on 7/21/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_CONTACTREPORTER_H
#define INTERACTIVE_VIRTUAL_CREATURES_CONTACTREPORTER_H
class ContactReporter : public PxSimulationEventCallback {
    private:
        std::map<const char*, PxPairFlag::Enum> m_contactStateMap;
    public:
        void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {};
        void onWake(PxActor** actors, PxU32 count) override {};
        void onSleep(PxActor** actors, PxU32 count) override {};
        void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {};
        void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override {}
        void onTrigger(PxTriggerPair* pairs, PxU32 count) override {

            for (PxU32 i = 0; i < count; i++)
            {
                // ignore pairs when shapes have been deleted
                if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
                    continue;

                if(pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
                    m_contactStateMap[pairs[i].triggerShape->getName()] = PxPairFlag::eNOTIFY_TOUCH_FOUND;

                if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
                    m_contactStateMap[pairs[i].triggerShape->getName()] = PxPairFlag::eNOTIFY_TOUCH_LOST;

            }
        };

        std::map<const char*, PxPairFlag::Enum> retrieveAndResetMap(){
            auto tmp = m_contactStateMap;
            m_contactStateMap.clear();
            return tmp;
        }

};
#endif //INTERACTIVE_VIRTUAL_CREATURES_CONTACTREPORTER_H
