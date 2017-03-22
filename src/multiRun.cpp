#include <stateMachine.h>
#include <smBase.h>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

stateMachine machine;

int runDAQ(int time);

int main(int argc, const char *argv[]) {

    int runNumber=1, runTime=2;
    char *pr="-r";
    char *pt="-t";
    if(argc > 1) {
        for(int i=1; i<argc; i++) {
            if(strcmp(argv[i], pr) == 0) {
                i++;
                runNumber = atoi(argv[i]);
                continue;
            }
            if(strcmp(argv[i], pt) == 0) {
                i++;
                runTime = atoi(argv[i]);
                continue;
            }
        }
    }
    std::cout << "runNumber: " << runNumber << std::endl;
    machine.init();
    std::string ret;

    smBase::command cmd;
    cmd = smBase::CMID_LOAD;
    machine.doAction(cmd, ret);
    sleep(1);   
    cmd = smBase::CMID_CONF;
    machine.doAction(cmd, ret);
    sleep(1);

    std::string pos;
    for(int i=0; i<runNumber; i++) {
        std::cout << "running " << i << "th: ";
        std::cin.sync();
        std::cin >> std::noskipws >> pos;

        runDAQ(runTime);

        std::cout << "done" << std::endl;
        //std::cin.sync();
        //std::cin >> std::noskipws >> pos;

        //runDAQ(runTime);
    }

    sleep(1);
    cmd = smBase::CMID_EXIT;
    machine.doAction(cmd, ret);

    return 0;
}

int runDAQ(int time) {
    smBase::command cmd;
    std::string ret;
    cmd = smBase::CMID_PREP;
    machine.doAction(cmd, ret);
    sleep(1);
    cmd = smBase::CMID_SATR;
    machine.doAction(cmd, ret);
    sleep(time);
    cmd = smBase::CMID_SPTR;
    machine.doAction(cmd, ret);
    sleep(2);
    cmd = smBase::CMID_STOP;
    machine.doAction(cmd, ret);
    sleep(1);
}
