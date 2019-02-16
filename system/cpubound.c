#include <xinu.h>

void cpubound(int32 LOOP1, int32 LOOP2){
    int32 a = 1;
    int32 b = 2;
    int32 c = 3;
    int32 d = 4;
    int32 e = 5;
    int32 i, j;
    byte storage[32];
    for (i=0; i<LOOP1; i++) {
        for (j=0; j<LOOP2; j++) {
            // Insert code that performs memory copy (slow) and/or
            // ALU operations (fast).
            // Note: this loop consumes significant CPU cycles.
            a += b;
            b *= c;
            d -= (a+b);
            e += (b/a);
            // wrap around 100000
            a = a%100000;
            b = b%100000;
            c = c%100000;
            d = d%100000;
            e = e%100000;
            
            // copy result to storage
            memcpy(storage, &a, 4);
            memcpy(storage+4, &b, 4);
            memcpy(storage+8, &c, 4);
            memcpy(storage+12, &d, 4);
            memcpy(storage+16, &e, 4);
        }
        // Using kprintf print the pid followed the outer loop count i,
        // the process's priority and remaining time slice (preempt).
        XTEST_KPRINTF("pid = %d, outer count = %d, priority = %d, preempt = %d\n",
                currpid, i, proctab[currpid].prprio, preempt);
    }
    XTEST_KPRINTF("DONE!\n");
}
