#include <xinu.h>


syscall hello(void){

    XTEST_KPRINTF("Hello system call invoked\n");
    return 0;
}
