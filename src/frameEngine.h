#ifndef frameEngine_h
#define frameEngine_h

#include <map>
#include <string>
#include "smBase.h"

class stateMachine;
class frameEngine :public smBase {

    public:
        frameEngine(const std::string& n);
        ~frameEngine();

        void setMachine(stateMachine* m);

    protected:
        virtual int InitializedLOAD(int para);
        virtual int LoadedUNLD(int para);

        virtual int LoadedCONF(int para) {return 3;};
        virtual int ConfiguredUNCF(int para) {return 2;};
        virtual int ConfiguredPREP(int para) {return 4;};
        virtual int ReadySATR(int para) {return 5;};
        virtual int RunningSPTR(int para) {return 4;};
        virtual int ReadySTOP(int para) {return 3;};
        virtual int RunningPAUS(int para) {return 6;};
        virtual int PausedSTOP(int para) {return 3;};
        virtual int PausedRESU(int para) {return stId;};
        virtual int PausedSATR(int para) {return 5;};
        virtual int SelfTrans(int para) {return 7;};
        virtual int AnyIMPO(int para) {return stId;};
        virtual int AnyEXIT(int para) {return 0;};

    private:
        //int readConfig();
        int moduleStringSplit(const std::string& modeList);
        int loadSharedModule(const std::string& modeName, const std::string& devName, const std::string& dir);
        int unloadSharedModule(const std::string& devName);

        /* data */
        stateMachine* theFrame;
        std::string libDir;
        std::map<std::string, std::pair<void*, int> > sharedLibHandle;
        std::map<std::string, std::string> dev2libMap;
};

#endif
