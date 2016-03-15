#include <stateMachine.h>
#include <smBase.h>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>

stateMachine machine;

int runDAQ();

int main(int argc, const char *argv[]) {

    int runNumber = 1;
    if(argc == 2)
        runNumber = atoi(argv[1]);
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
        std::cout << "pls set mechanical platform distination(fake): ";
        std::cin.sync();
        std::cin >> pos;

        runDAQ();
    }

    sleep(1);
    cmd = smBase::CMID_EXIT;
    machine.doAction(cmd, ret);

    return 0;
}

int runDAQ() {
    smBase::command cmd;
    std::string ret;
    cmd = smBase::CMID_PREP;
    machine.doAction(cmd, ret);
    sleep(1);
    cmd = smBase::CMID_SATR;
    machine.doAction(cmd, ret);
    sleep(12);
    cmd = smBase::CMID_SPTR;
    machine.doAction(cmd, ret);
    sleep(4);
    cmd = smBase::CMID_STOP;
    machine.doAction(cmd, ret);
    sleep(1);
}
