#include "stateMachine.h"
#include "frameEngine.h"
#include <sstream>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::stringstream;

stateMachine::stateMachine() {
}

stateMachine::~stateMachine() {    
}

int stateMachine::init() {
    stId = smBase::STID_Waiting;
    msg = new stateMessager();
    cfg = new configSet();
    data = new dataStream();
    cfg->infoCreate();
    theEngine = new frameEngine("engine");
    theEngine->setMachine(this);
    theEngine->init(msg, cfg, data, &moduleList);
    moduleList.push_back(std::pair<string, smBase*>("engine", theEngine));
    stId = smBase::STID_Initialized;

    return 1;
}

int stateMachine::doAction(const smBase::command& cmId) {

    for(unsigned int i=0; i<moduleList.size(); i++) {
        int res = moduleList[i].second->doAction(cmId);
        //if(result == smBase::STID_Invaild || result == smBase::MAX_STATES_AMOUNT) {
        if(res == -1) {
            msg->stateOut(1, "doAction error");
            stId = smBase::STID_Invaild;
            return 0;
        }
        stId = smBase::status(res);
    }
    return 1;
}

void stateMachine::addMode(const string& name, smBase* mode) {
    moduleList.push_back(std::pair<string, smBase*>(name, mode));
    return;
}

const std::vector< std::pair<std::string, smBase*> >& stateMachine::getModuleList() {
    return moduleList;
}

std::pair<string, smBase*> stateMachine::popMode() {
    std::pair<string, smBase*> pMode = moduleList[moduleList.size()-1];
    moduleList.pop_back();
    return pMode;
}

smBase* stateMachine::eraseMode(const string& name) {
    smBase* pModule = NULL;
    for(unsigned int i=0; i<moduleList.size();i++) {
        std::pair<string, smBase*> pMode = moduleList[i];
        if(pMode.first == name) {
            pModule = pMode.second;
            break;
        }
    }
    return pModule;
}

smBase::command stateMachine::getCommand() {

    string s0;
    cout << "#Command: load, unld, conf, uncf, prep, satr, sptr, stop, paus, resu, exit, notr" << endl;
    cout << "#DAQ: ";
    cin >> s0;
    cout << "input " << s0 << endl;
    smBase::command cmd;
    //CMID_UNKNCM=-1, CMID_LOAD, CMID_UNLD, CMID_CONF, CMID_UNCF, CMID_PREP, CMID_SATR, CMID_SPTR, CMID_STOP, CMID_PAUS, CMID_RESU, CMID_EXIT,CMID_NONE_TRANS, MAX_CMD_AMOUNT
    if(s0=="load")
        cmd = smBase::CMID_LOAD;
    else if(s0=="unld")
        cmd = smBase::CMID_UNLD;
    else if(s0=="conf")
        cmd = smBase::CMID_CONF;
    else if(s0=="uncf")
        cmd = smBase::CMID_UNCF;
    else if(s0=="prep")
        cmd = smBase::CMID_PREP;
    else if(s0=="satr")
        cmd = smBase::CMID_SATR;
    else if(s0=="sptr")
        cmd = smBase::CMID_SPTR;
    else if(s0=="stop")
        cmd = smBase::CMID_STOP;
    else if(s0=="paus")
        cmd = smBase::CMID_PAUS;
    else if(s0=="resu")
        cmd = smBase::CMID_RESU;
    else if(s0=="exit")
        cmd = smBase::CMID_EXIT;
    else if(s0=="notr")
        cmd = smBase::CMID_NONE_TRANS;
    else
        cmd = smBase::CMID_UNKNCM;

    stringstream cmdId;
    cmdId << (int)cmd;
    string result;
    cmdId >> result;
    msg->stateOut(1, result);
    return cmd;
}

int stateMachine::dispatch1() {

    while(1) {
        smBase::command cmd = getCommand();
        if(cmd != smBase::CMID_UNKNCM && cmd != smBase::MAX_CMD_AMOUNT) {
            int res = doAction(cmd);
            if(res != 1) {
                msg->stateOut(1, "stateMachine::dispatch1 doAction error.");
                return 0;
            }
            else {
                if(cmd == smBase::CMID_EXIT)
                    break;
            }
        }
    }

    return 1;
}

int stateMachine::dispatch2() {

    return 1;
}
