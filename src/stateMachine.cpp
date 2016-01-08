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

    return 1;
}

int stateMachine::doAction(const smBase::command& cmId, string& ret) {
    string cmd, ret0;
    stringstream sCmd("");
    sCmd << (int)cmId;
    sCmd >> cmd;
    for(unsigned int i=0; i<moduleList.size(); i++) {
        int res = moduleList[i].second->doAction(cmId, ret0="");
        if(res == -1)
            return 0;
        ret += moduleList[i].first+"#"+cmd+"#"+ret0+"#"+"$";
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
    string ret;
    while(1) {
        smBase::command cmd = getCommand();
        if(cmd != smBase::CMID_UNKNCM) {
            ret = "";
            int res = doAction(cmd, ret);
            if(res != 1) {
                return res;
            }
            else {
                if(cmd == smBase::CMID_EXIT)
                    break;
            }
        }
    }
    return 1;
}

int stateMachine::dispatch2(const string& ctrl, string& ret) {

    string cmd, dev, msg;
    stringstream sCtrl(ctrl);
    getline(sCtrl, dev, '#');
    getline(sCtrl, cmd, '#');
    getline(sCtrl, msg, '#');
    cout << "dev: " << dev << ", cmd: " << cmd << ", msg: " << msg << endl;

    stringstream sCmd(cmd);
    int vCmd;
    sCmd >> vCmd;
    stringstream sMsg(msg);
    string ctrlName, rw, value="";
    getline(sMsg, ctrlName, ';');
    getline(sMsg, rw, ';');
    if(rw == "w")
        getline(sMsg, value, ';');
    cout << "ctrlName: " << ctrlName << ", rw: " << rw << ", value: " << value << endl;
    void* para[] = {&ctrlName, &rw, &value};

    int res = 0;
    if(vCmd >= (int)smBase::CMID_UNKNCM && vCmd < (int)smBase::MAX_CMD_AMOUNT) {
        smBase::command eCmd = smBase::command(vCmd);
        cout << "cmd: " << vCmd << ", id: " << eCmd << endl;
        if(dev == "all") {
            res = doAction(eCmd, ret);
        }
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
                res = pModule->doAction(eCmd, ret, para);
            }
            ret = dev+"#"+cmd+"#"+ret+"#"+"$";
        }
    }
    return res;
}
