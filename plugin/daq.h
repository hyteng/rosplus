#ifndef daq_h
#define daq_h

#include <thread>
#include <fstream>
#include "../src/smBase.h"


class daq :public smBase {

    public:
        daq(const std::string& n);
        ~daq();
        
    protected:
        virtual int LoadedCONF(std::string& ret, void* para[]=NULL);
        virtual int ConfiguredPREP(std::string& ret, void* para[]=NULL);
        virtual int ReadySTOP(std::string& ret, void* para[]=NULL);
        virtual int ReadySATR(std::string& ret, void* para[]=NULL);
        virtual int RunningSPTR(std::string& ret, void* para[]=NULL);
        virtual int PausedSPTR(std::string& ret, void* para[]=NULL);

    private:
        int configDaq();
        int prepDaq();
        int finishDaq();
        int startDaq();
        int stopDaq();
        void runDaq();

    private:
        int runDaqCtrl;
        int daqStatus;
        int netMsgQue;
        unsigned int daqSize;
        streamMsg daqMsg;
        std::thread *t0, *ts;

        std::string fileName;
        std::fstream outFile;
};

#endif
