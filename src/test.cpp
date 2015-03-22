#include <stateMachine.h>
#include <smBase.h>

int main(int argc, const char *argv[]) {

    stateMachine machine;
    machine.init();

    machine.dispatch1();
    return 0;
}
