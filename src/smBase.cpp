#include "smBase.h"

using std::string;

smBase::smBase(const string& n) {
    name = n;
    stId = STID_Waiting;
}

smBase::~smBase() {
}

void smBase::init(stateMessager* msg, configSet* cfg, dataStream* data) {

    stMsg = msg;
    cfgInfo = cfg;
    dataOut = data;
    
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
    actions[CMID_SATR][STID_Paused] = &smBase::PausedSATR;
    actions[CMID_SPTR][STID_Running] = &smBase::RunningSPTR;
    actions[CMID_SPTR][STID_Ready] = &smBase::SelfTrans;
    actions[CMID_SPTR][STID_Paused] = &smBase::SelfTrans;
    actions[CMID_STOP][STID_Configured] = &smBase::SelfTrans;
    actions[CMID_STOP][STID_Ready] = &smBase::ReadySTOP;
    actions[CMID_STOP][STID_Paused] = &smBase::PausedSTOP;
    actions[CMID_PAUS][STID_Paused] = &smBase::SelfTrans;
    actions[CMID_RESU][STID_Paused] = &smBase::PausedRESU;
    actions[CMID_RESU][STID_Running] = &smBase::SelfTrans;

    stId = STID_Initialized;
}

int smBase::doAction(command cmId) {
    int res, para = 0;
    stMsg->stateOut(2, name+" doAction");
    res = (this->*actions[cmId][stId])(para);
    if (res == -1) {
        //messager("action fail");
        stId = STID_Invaild;
        return 0;
    }
    stId = smBase::status(res);
    return res;
}
