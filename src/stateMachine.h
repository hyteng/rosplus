#ifndef stateMachine_h
#define stateMachine_h

#include <vector>
#include <string>
#include <iostream>
#include "smBase.h"

class frameEngine;

class stateMachine {

    public:
        stateMachine();
        ~stateMachine();

        int init();
        void addMode(const std::string& name, smBase* mode);
        std::pair<std::string, smBase*> popMode();
        smBase* eraseMode(const std::string& name);
        int doAction(const smBase::command& cmId);
        smBase::command getCommand();

        int dispatch1();
        int dispatch2();

    private:
        std::vector< std::pair<std::string, smBase*> >moduleList;
        frameEngine* theEngine;
        smBase::status stId;

        configSet *cfg;
        dataStream *data;
        stateMessager *msg;
};

#endif
