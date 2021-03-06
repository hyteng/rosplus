#include "frameEngine.h"
#include "stateMachine.h"
#include <dlfcn.h>
#include <sstream>
//#include <fstream>
//#include <stdint.h>

using std::string;
using std::stringstream;
//using std::ifstream;

frameEngine::frameEngine(const string& n) : smBase(n) {
}

frameEngine::~frameEngine() {
}

void frameEngine::setMachine(stateMachine* m) {
    theFrame = m;
}

int frameEngine::InitializedLOAD(int para) {
    
    cfgInfo->infoClear();
    cfgInfo->readConfig("runSet.conf");

    string modeList;
    if(!cfgInfo->infoGetString("config.module", modeList)) {
        return -1;
    }                    

    if(!cfgInfo->infoGetString("config.lib_dir", libDir)) {
        return -1;       
    }                    
    
    sharedLibHandle.clear();
    dev2libMap.clear();
    moduleStringSplit(modeList);
    std::map<std::string, std::string>::const_iterator modeIter;
    for(modeIter=dev2libMap.begin(); modeIter!=dev2libMap.end(); modeIter++) {  
        if(loadSharedModule(modeIter->second, modeIter->first, libDir)) {
            stMsg->stateOut(2, modeIter->first);
        }
        else {
            return -1;
        }
    }
    return 2;
}

int frameEngine::LoadedUNLD(int para) {
    std::map<std::string, std::string>::const_iterator modeIter;
    for(modeIter=dev2libMap.begin(); modeIter!=dev2libMap.end(); modeIter++) {
        if(unloadSharedModule(modeIter->first)) {
            stMsg->stateOut(2, modeIter->first+" loaded");
        }
        else {
            return -1;
        }
    }
    return 1;
}

int frameEngine::moduleStringSplit(const string& modeList) {

    stringstream sList, sMode, sLib, sDev;
    string mode, lib, dev;
    sList << modeList;
    while(getline(sList, mode, ';')) {
        sMode << mode;
        getline(sMode, lib, '.');
        getline(sMode, dev, '.');
        dev2libMap[dev] = lib;
    }
    return dev2libMap.size();
}

int frameEngine::loadSharedModule(const string& modeName, const string& devName, const string& dir) {

    char* dlsym_error;
    string libPath = dir+"/lib"+modeName+".so";

    stMsg->stateOut(2, libPath);
    if(sharedLibHandle.find(modeName) == sharedLibHandle.end()) {
        void* libHandle = dlopen(libPath.c_str(), RTLD_LAZY);
        dlsym_error = dlerror();
        if(dlsym_error != NULL) {
            stMsg->stateOut(2, string(dlsym_error));
            //return 0;
        }

        if(libHandle == NULL) {
            stMsg->stateOut(2, "shared lib not opened");
            return -1;
        }
        else {
            sharedLibHandle[modeName].first = libHandle;
            sharedLibHandle[modeName].second = 1;
            stMsg->stateOut(2, "shared lib opened");
        }
    }
    else {
        sharedLibHandle[modeName].second++;
    }

    stMsg->stateOut(2, libPath);
    pCreate createModule = (pCreate) dlsym(sharedLibHandle[modeName].first, "create");
    dlsym_error = dlerror();
    if(dlsym_error != NULL) {
        sharedLibHandle[modeName].second--;
        if(sharedLibHandle[modeName].second == 0)
            sharedLibHandle.erase(modeName);
        return 0;
    }

    smBase* pModule = (*createModule)(devName);
    if(pModule == NULL)
        return 0;

    //dev2libMap[devName] = modeName;
    pModule->init(stMsg, cfgInfo, dataOut);
    theFrame->addMode(devName, pModule);

    return 1;
}

int frameEngine::unloadSharedModule(const string& devName) {

    char* dlsym_error;

    string modeName = dev2libMap[devName];
    string libPath = libDir+"/lib"+modeName+".so";
    pDestroy destroyModule = (pDestroy) dlsym(sharedLibHandle[modeName].first, "destroy");
    dlsym_error = dlerror();
    if(dlsym_error != NULL) {
        return 0;
    }

    smBase* pModule;
    pModule = theFrame->eraseMode(devName);
    if(pModule == NULL) {
        //msg
        return 0;
    }
    (*destroyModule)(pModule);

    dev2libMap.erase(devName);

    sharedLibHandle[modeName].second--;
    if(sharedLibHandle[modeName].second == 0) {
        dlclose(sharedLibHandle[modeName].first);
        sharedLibHandle.erase(modeName);
    }
    return 1;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new frameEngine(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
