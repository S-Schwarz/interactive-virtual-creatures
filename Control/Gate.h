//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_GATE_H
#define INTERACTIVE_VIRTUAL_CREATURES_GATE_H

namespace ivc{
    class Gate {
    private:
        unsigned long m_id;
        float m_newValue = 0.0f;
        float m_oldValue = 0.0f;
    public:
        void setID(unsigned long);
        unsigned long getID();
        void setValue(float);
        float getValue();
        void swap();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_GATE_H
