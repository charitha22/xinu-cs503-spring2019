#include <xinu.h>

void iobound(int32 LOOP1, int32 LOOP2){
    int32 i, j;    
    for (i=0; i<LOOP1; i++) {
        for (j=0; j<LOOP2; j++) {
            // Insert code that performs memory copy (slow) and/or
            // ALU operations (fast).
            // Note: this loop consumes significant CPU cycles.
            sleepms(35);
        }
        // Using kprintf print the pid followed the outer loop count i,
        // the process's priority and remaining time slice (preempt).
        XTEST_KPRINTF("pid = %d, outer count = %d, priority = %d, preempt = %d\n",
                currpid, i, proctab[currpid].prprio, preempt);
    }
    XDEBUG_KPRINTF("DONE!\n");
}
