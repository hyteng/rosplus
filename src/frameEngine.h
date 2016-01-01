#ifndef frameEngine_h
#define frameEngine_h

#include <map>
#include "smBase.h"

class stateMachine;
class frameEngine :public smBase {

    public:
        frameEngine(const std::string& n);
        ~frameEngine();

        void setMachine(stateMachine* m);

    protected:
        virtual int InitializedLOAD(std::string& ret, void* para[]=NULL);
        virtual int LoadedUNLD(std::string& ret, void* para[]=NULL);
        
    private:
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
