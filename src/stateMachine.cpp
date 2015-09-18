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
    cfg = new configSet();
    data = new dataStream();
    msg = new stateMessager();
    cfg->infoCreate();
    data->init();
    msg->init(this);
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
        stId = smBase::status(res);
        if(res == -1)
            return 0;
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
            moduleList.erase(moduleList.begin()+i);
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
    //CMID_UNKNCM=-1, CMID_LOAD, CMID_UNLD, CMID_CONF, CMID_UNCF, CMID_PREP, CMID_STOP, CMID_SATR, CMID_SPTR, CMID_PAUS, CMID_RESU, CMID_STAT, CMID_CTRL, CMID_EXIT, MAX_CMD_AMOUNT
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
    else if(s0=="stop")
        cmd = smBase::CMID_STOP;
    else if(s0=="satr")
        cmd = smBase::CMID_SATR;
    else if(s0=="sptr")
        cmd = smBase::CMID_SPTR;
    else if(s0=="paus")
        cmd = smBase::CMID_PAUS;
    else if(s0=="resu")
        cmd = smBase::CMID_RESU;
    else if(s0=="stat")
        cmd = smBase::CMID_STAT;
    //else if(s0=="ctrl")
        //cmd = smBase::CMID_CTRL;
    else if(s0=="exit")
        cmd = smBase::CMID_EXIT;
    else
        cmd = smBase::CMID_UNKNCM;

    stringstream cmdId;
    cmdId << (int)cmd;
    string result;
    cmdId >> result;
    return cmd;
}

int stateMachine::dispatch1() {

    while(1) {
        smBase::command cmd = getCommand();
        if(cmd != smBase::CMID_UNKNCM) {
            int res = doAction(cmd);
            if(res != 1) {
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

int stateMachine::dispatch2(string& ctrl) {

    string type, dev, msg;
    stringstream sCtrl(ctrl);
    getline(sCtrl, type, '#');
    getline(sCtrl, dev, '#');
    getline(sCtrl, msg, '#');
    stringstream sMsg(msg);
    cout << "type: " << type << ", dev: " << dev << ", msg: " << msg << endl;
    int res = 0;
    // for device command
    if(type == "cmd") {
        int v;
        sMsg >> v;
        if(v >= (int)smBase::CMID_UNKNCM && v < (int)smBase::MAX_CMD_AMOUNT && v != smBase::CMID_CTRL) {
            smBase::command cmd = smBase::command(v);
            cout << "cmd: " << v << ", id: " << cmd << endl;
            if(dev == "all")
                res = doAction(cmd);
            else {
                smBase* pModule = NULL;
                for(unsigned int i=0; i<moduleList.size();i++) {
                    std::pair<string, smBase*> pMode = moduleList[i];
                    if(pMode.first == dev) {
                        pModule = pMode.second;
                        break;
                    }
                }
                if(pModule != NULL) {
                    res = pModule->doAction(cmd);
                }
            }
            ctrl = dev+"#"+type+"#"+msg+"#";

            if(res != 1) {
                return 0;
            }
            else {
                if(cmd == smBase::CMID_EXIT)
                    return 1;
            }
        }

    }
    // for configure reg
    if(type == "ctrl") {
        smBase* pModule = NULL;
        for(unsigned int i=0; i<moduleList.size();i++) {
            std::pair<string, smBase*> pMode = moduleList[i];
            if(pMode.first == dev) {
                pModule = pMode.second;
                break;
            }
        }
        if(pModule != NULL) {
            string ctrlName, rw, value="";
            getline(sMsg, ctrlName, ';');
            getline(sMsg, rw, ';');
            if(rw == "w")
                getline(sMsg, value, ';');

            //configSet::infoValue* data*;
            void* para[] = {&ctrlName, &rw, &value};
            pModule->doAction(smBase::CMID_CTRL, para);
            // reverse the deve and type in string
            ctrl = dev+"#"+type+"#"+ctrlName+";"+rw+";"+value+";"+"#";
        }

    }

    return 1;
}
