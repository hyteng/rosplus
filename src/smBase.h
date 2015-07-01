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
#include <vector>
#include <sstream>

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

        void init(stateMessager* msg, configSet* cfg, dataStream* data, const std::vector< std::pair<std::string, smBase*> > *other);
        virtual int doAction(command cmId);
        virtual void* getHelp() {return NULL;};

    protected:
        virtual int InitializedLOAD(int para) {return 2;};
        virtual int LoadedUNLD(int para) {return 1;};
        virtual int LoadedCONF(int para) {return 3;};
        virtual int ConfiguredUNCF(int para) {return 2;};
        virtual int ConfiguredPREP(int para) {return 4;};
        virtual int ReadySATR(int para) {return 5;};
        virtual int RunningSPTR(int para) {return 4;};
        virtual int ReadySTOP(int para) {return 3;};
        virtual int RunningPAUS(int para) {return 6;};
        virtual int PausedSPTR(int para) {return 4;};
        virtual int PausedRESU(int para) {return 5;};
        //virtual int PausedSATR(int para) {return 5;};
        virtual int SelfTrans(int para) {return stId;};
        virtual int AnyIMPO(int para) {return stId;};
        virtual int AnyEXIT(int para) {return 0;};
        virtual int OTFCONF(int para) {return stId;};
        virtual int RunningCONF(int para) {return stId;};
        virtual int PausedCONF(int para) {return stId;};
        virtual int ReadyCONF(int para) {return stId;};

        pFunc actions[MAX_CMD_AMOUNT][MAX_STATES_AMOUNT];

    protected:
        stateMessager* stMsg;
        configSet* cfgInfo;
        dataStream* dataPool;
        const std::vector< std::pair<std::string, smBase*> > *helpList;
        status stId;
        std::string name;
        std::stringstream debugMsg;
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
