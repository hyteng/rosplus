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
#include <map>
#include <sstream>
#include <stdint.h>

class regData {
    public:
        virtual void setValueS(std::stringstream& ss) = 0;
        virtual void setValueP(const void* p) = 0;
        virtual void* getValueP() = 0;
        virtual std::string getValueS() = 0;
        virtual regData& ptr(const void* p) = 0;
};

class smBase;
typedef int(smBase::*pFunc)(void* argv[]);
typedef smBase* pBase;

class smBase {

    public:
        smBase(const std::string& n);
        virtual ~smBase();

    public:
        typedef enum {
            CMID_UNKNCM=-1, CMID_LOAD, CMID_UNLD, CMID_CONF, CMID_UNCF, CMID_PREP, CMID_STOP, CMID_SATR, CMID_SPTR, CMID_PAUS, CMID_RESU, CMID_STAT, CMID_CTRL, CMID_EXIT, MAX_CMD_AMOUNT
        } command;
        typedef enum {
            STID_Invaild=-1, STID_Waiting, STID_Initialized, STID_Loaded, STID_Configured, STID_Ready, STID_Running, STID_Paused, MAX_STATES_AMOUNT
        } status;
        //static int result[MAX_CMD_AMOUNT][MAX_STATES_AMOUNT];

        void init(stateMessager* msg, configSet* cfg, dataStream* data, const std::vector< std::pair<std::string, smBase*> > *other);
        virtual int doAction(command cmId, void* para[]=NULL);
        virtual smBase* getHelp() {return this;};
        virtual bool queryInterface(const string& funcName, void* ptr) = 0;

    protected:
        virtual int InitializedLOAD(void* argv[]=NULL) {return 2;};
        virtual int LoadedUNLD(void* argv[]=NULL) {return 1;};
        virtual int LoadedCONF(void* argv[]=NULL) {return 3;};
        virtual int ConfiguredUNCF(void* argv[]=NULL) {return 2;};
        virtual int ConfiguredPREP(void* argv[]=NULL) {return 4;};
        virtual int ReadySATR(void* argv[]=NULL) {return 5;};
        virtual int ReadySTOP(void* argv[]=NULL) {return 3;};
        virtual int RunningSPTR(void* argv[]=NULL) {return 4;};
        virtual int RunningPAUS(void* argv[]=NULL) {return 6;};
        virtual int PausedSPTR(void* argv[]=NULL) {return 4;};
        virtual int PausedRESU(void* argv[]=NULL) {return 5;};
        //virtual int PausedSATR(void* argv[]=NULL) {return 5;};
        virtual int SelfTrans(void* argv[]=NULL) {return (int)stId;};
        virtual int AnyIMPO(void* argv[]=NULL) {return (int)stId;};
        virtual int AnyEXIT(void* argv[]=NULL) {return 1;};
        virtual int OTFSTAT(void* argv[]=NULL) {return stId;};
        virtual int RunningSTAT(void* argv[]=NULL) {return OTFSTAT(argv);};
        virtual int PausedSTAT(void* argv[]=NULL) {return OTFSTAT(argv);};
        virtual int ReadySTAT(void* argv[]=NULL) {return OTFSTAT(argv);};
        virtual int OTFCTRL(void* argv[]=NULL); //{return stId;};
        virtual int RunningCTRL(void* argv[]=NULL) {return OTFCTRL(argv);};
        virtual int PausedCTRL(void* argv[]=NULL) {return OTFCTRL(argv);};
        virtual int ReadyCTRL(void* argv[]=NULL) {return OTFCTRL(argv);};
        virtual int accessReg(const int idx, const int rw, regData& data) {return 1;};
        virtual int maskRegData(regData& data, regData& mask) {return 1;};
        virtual int unmaskRegData(regData& data, regData& mask) {return 1;};

        pFunc actions[MAX_CMD_AMOUNT][MAX_STATES_AMOUNT];

    protected:
        stateMessager* stMsg;
        configSet* cfgInfo;
        dataStream* dataPool;
        const std::vector< std::pair<std::string, smBase*> > *helpList;
        status stId;
        std::string name;
        std::stringstream debugMsg;

        std::string otfMsg;
        std::map<std::string, std::vector<std::string> > *ctrl2conf;
        std::map<std::string, int> *conf2reg;
        std::map<std::string, uintptr_t> *conf2mask;
        std::vector<uintptr_t> *regAddr;
        std::vector<int> *regRWIdx;
        std::vector<int> regSet, confSet;

        regData *vd, *vm;
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
