#ifndef smBase_h
#define smBase_h

/*
#define MAX_CMD_AMOUNT 12
#define CMID_UNKN -1
#define CMID_LOAD 0
#define CMID_UNLD 1
#define CMID_CONF 2
#define CMID_UNCF 3      
#define CMID_PREP 4      
#define CMID_SATR 5
#define CMID_SPTR 6      
#define CMID_STOP 7
#define CMID_PAUS 8
#define CMID_RESU 9    
#define CMID_EXIT 10
#define CMID_NONE_TRANS 11

#define MAX_STATES_AMOUNT 7
#define STID_Waiting 0   
#define STID_Initialized 1
#define STID_Loaded 2    
#define STID_Configured 3
#define STID_Ready 4     
#define STID_Running 5   
#define STID_Paused 6 
*/

#include "stateMessager.h"
#include "configSet.h"
#include "dataStream.h"
#include <string>

class smBase;
typedef int(smBase::*pFunc)(int para);
typedef smBase* pBase;

class smBase {

    public:
        smBase(const std::string& n);
        virtual ~smBase();

    public:

    typedef enum {
        CMID_UNKNCM=-1, CMID_LOAD, CMID_UNLD, CMID_CONF, CMID_UNCF, CMID_PREP, CMID_SATR, CMID_SPTR, CMID_STOP, CMID_PAUS, CMID_RESU, CMID_EXIT, CMID_NONE_TRANS, MAX_CMD_AMOUNT
    } command;
    typedef enum {
        STID_Invaild=-1, STID_Waiting, STID_Initialized, STID_Loaded, STID_Configured, STID_Ready, STID_Running, STID_Paused, MAX_STATES_AMOUNT
    } status;
    //static int result[MAX_CMD_AMOUNT][MAX_STATES_AMOUNT];

    void init(stateMessager* msg, configSet* cfg, dataStream* data);
    virtual int doAction(command cmId);

    protected:
    virtual int InitializedLOAD(int para)=0;
    virtual int LoadedUNLD(int para)=0;
    virtual int LoadedCONF(int para)=0;
    virtual int ConfiguredUNCF(int para)=0;
    virtual int ConfiguredPREP(int para)=0;
    virtual int ReadySATR(int para)=0;
    virtual int RunningSPTR(int para)=0;
    virtual int ReadySTOP(int para)=0;
    virtual int RunningPAUS(int para)=0;
    virtual int PausedSTOP(int para)=0;
    virtual int PausedRESU(int para)=0;
    virtual int PausedSATR(int para)=0;
    virtual int SelfTrans(int para)=0;
    virtual int AnyIMPO(int para)=0;
    virtual int AnyEXIT(int para)=0;
    pFunc actions[MAX_CMD_AMOUNT][MAX_STATES_AMOUNT];

    protected:
    stateMessager* stMsg;
    configSet* cfgInfo;
    dataStream* dataOut;
    status stId;
    std::string name;
};

typedef pBase (*pCreate)(const std::string& name);
typedef void (*pDestroy)(pBase);

/*
int smBase::result[smBase::CMID_LOAD][smBase::STID_Initialized] = smBase::STID_Loaded;
int smBase::result[smBase::CMID_CONF][smBase::STID_Loaded] = smBase::STID_Configured;
int smBase::result[smBase::CMID_PREP][smBase::STID_Configured] = smBase::STID_Ready;
int smBase::result[smBase::CMID_SATR][smBase::STID_Ready] = smBase::STID_Running;
*/
#endif
