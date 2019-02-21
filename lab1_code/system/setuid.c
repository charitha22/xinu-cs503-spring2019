#include <xinu.h>


syscall setuid(int newuid){
    intmask mask;
    
    mask = disable();
    // only a root process can change
    // its uid
    if(proctab[currpid].uid == 0){
        proctab[currpid].uid = newuid;
    }
    else{
        return SYSERR;
    }

    restore(mask);
    return OK;
}
