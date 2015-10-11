#include "smBase.h"

using std::string;
using std::stringstream;
using std::map;
using std::vector;

smBase::smBase(const string& n) {
    name = n;
    stId = STID_Waiting;
    regSet.clear();
    confSet.clear();
}

smBase::~smBase() {
}

void smBase::init(stateMessager* msg, configSet* cfg, dataStream* data, const std::vector< std::pair<std::string, smBase*> > *other) {

    stMsg = msg;
    cfgInfo = cfg;
    dataPool = data;
    helpList = other;

    int i, j;
    for(i = 0; i < MAX_CMD_AMOUNT; i++) {
       for(j = 0; j < MAX_STATES_AMOUNT; j++) {
           actions[i][j] = &smBase::AnyIMPO;
       }
    }

    /*ensure that EXIT is the last command before doing so*/
    for(j = 0; j < MAX_STATES_AMOUNT; j ++) {
        actions[CMID_EXIT][j] = &smBase::AnyEXIT;
    }

    actions[CMID_LOAD][STID_Initialized] = &smBase::InitializedLOAD;
    actions[CMID_LOAD][STID_Loaded] = &smBase::SelfTrans;
    actions[CMID_UNLD][STID_Initialized] = &smBase::SelfTrans;
    actions[CMID_UNLD][STID_Loaded] = &smBase::LoadedUNLD;
    actions[CMID_CONF][STID_Loaded] = &smBase::LoadedCONF;
    actions[CMID_CONF][STID_Configured] = &smBase::SelfTrans;
    actions[CMID_UNCF][STID_Loaded] = &smBase::SelfTrans;
    actions[CMID_UNCF][STID_Configured] = &smBase::ConfiguredUNCF;
    actions[CMID_PREP][STID_Configured] = &smBase::ConfiguredPREP;
    actions[CMID_PREP][STID_Ready] = &smBase::SelfTrans;
    actions[CMID_SATR][STID_Ready] = &smBase::ReadySATR;
    actions[CMID_SATR][STID_Running] = &smBase::SelfTrans;
    //actions[CMID_SATR][STID_Paused] = &smBase::PausedSATR;
    actions[CMID_SPTR][STID_Running] = &smBase::RunningSPTR;
    actions[CMID_SPTR][STID_Ready] = &smBase::SelfTrans;
    actions[CMID_SPTR][STID_Paused] = &smBase::PausedSPTR;
    actions[CMID_STOP][STID_Configured] = &smBase::SelfTrans;
    actions[CMID_STOP][STID_Ready] = &smBase::ReadySTOP;
    actions[CMID_PAUS][STID_Running] = &smBase::RunningPAUS;
    actions[CMID_PAUS][STID_Paused] = &smBase::SelfTrans;
    actions[CMID_RESU][STID_Paused] = &smBase::PausedRESU;
    actions[CMID_RESU][STID_Running] = &smBase::SelfTrans;
    actions[CMID_CTRL][STID_Running] = &smBase::RunningCTRL;
    actions[CMID_CTRL][STID_Paused] = &smBase::PausedCTRL;
    actions[CMID_CTRL][STID_Ready] = &smBase::ReadyCTRL;
    actions[CMID_STAT][STID_Running] = &smBase::RunningSTAT;
    actions[CMID_STAT][STID_Paused] = &smBase::PausedSTAT;
    actions[CMID_STAT][STID_Ready] = &smBase::ReadySTAT;


    stId = STID_Initialized;
}

int smBase::doAction(command cmId, void* para[]) {
    int res;
    debugMsg << name << "# doAction";
    stMsg->stateOut(debugMsg);
    res = (this->*actions[cmId][stId])(para);
    if (res == -1) {
        stId = STID_Invaild;
        return 0;
    }
    stId = smBase::status(res);
    return res;
}

int smBase::OTFCTRL(void* argv[]) {
    debugMsg << name << "# " << "OTFCONF";
    stMsg->stateOut(debugMsg);

    string ctrlName = *(string*)argv[0];
    string rw = *(string*)argv[1];
    string value = *(string*)argv[2];
    stringstream result("");
    int res;

    int io = -1;
    if(rw == "r")
        io = 0;
    if(rw == "w")
        io = 1;
    
    stringstream sValue(value);
    string v;
    std::vector<string> &confList = (*ctrl2conf)[ctrlName];
    for(unsigned int i=0; i<confList.size(); i++) {
        getline(sValue, v, ',');
        stringstream sv(v);
        (*vd).setValueS(sv);
        (*vm).ptr((void*)((*conf2mask)[confList[i]]));
        maskRegData(*vd, *vm);
        int regIdx = (*conf2reg)[confList[i]];
        if((res=accessReg(regIdx, io, *vd)) != 1)
            return -1;
        unmaskRegData(*vd, *vm);
        result << vd->getValueS() << ","; 
    }

    configSet::infoType type;
    string cfgIdx = "config."+name+"."+ctrlName;
    if((res=cfgInfo->infoGetType(cfgIdx, type)) == 1) {
        switch((int)type) {
            case 0: {
                uint32_t v0;
                sValue >> v0;
                if((res=cfgInfo->infoSetUint(cfgIdx, v0)) == 1) {
                    debugMsg << name << "# " << cfgIdx << " set " << v0;
                    stMsg->stateOut(debugMsg);
                }
                else {
                    debugMsg << name << "# " << cfgIdx << " not set. ";
                    stMsg->stateOut(debugMsg);
                    return -1;
                }
                break; }
            case 1: {
                uint64_t v1;
                sValue >> v1;
                if((res=cfgInfo->infoSetUlong(cfgIdx, v1)) == 1) {
                    debugMsg << name << "# " << cfgIdx << " set " << v1;
                    stMsg->stateOut(debugMsg);
                }
                else {
                    debugMsg << name << "# " << cfgIdx << " not set. ";
                    stMsg->stateOut(debugMsg);
                    return -1;
                }
                break; }
            case 2: {
                double v2;
                sValue >> v2;
                if((res=cfgInfo->infoSetDouble(cfgIdx, v2)) == 1) {
                    debugMsg << name << "# " << cfgIdx << " set " << v2;
                    stMsg->stateOut(debugMsg);
                }
                else {
                    debugMsg << name << "# " << cfgIdx << " not set. ";
                    stMsg->stateOut(debugMsg);
                    return -1;
                }
                break; }
            case 3: {
                string v3;
                sValue >> v3;
                if((res=cfgInfo->infoSetString(cfgIdx, v3)) == 1) {
                    debugMsg << name << "# " << cfgIdx << " set " << v3;
                    stMsg->stateOut(debugMsg);
                }
                else {
                    debugMsg << name << "# " << cfgIdx << " not set. ";
                    stMsg->stateOut(debugMsg);
                    return -1;
                }
                break; }
            default :
                return -1;
        }
    }

    *(string*)argv[2] = result.str();
    return (int)stId;
}
