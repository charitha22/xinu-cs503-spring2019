/*	ldelete.c - ldelete 	*/
#include <xinu.h>

/* Lab 2: Complete this function */

syscall ldelete( 
		int32 ldes	/* lock descriptor */
	)
{
	// your implementation
    intmask mask;
    struct lockent* lockptr;
    pid32 reader, writer;

    mask = disable();
    if(isbadlock(ldes)){
        restore(mask);
        return SYSERR;
    }

    lockptr = &locktab[ldes];
    if(lockptr->lckstate == L_FREE){
        restore(mask);
        return SYSERR;
    }

    lockptr->lckstate = L_FREE;
    lockptr->lck_ctime = 0; // reset the creation time

    resched_cntl(DEFER_START);
    
    // free readers
    while(nonempty(lockptr->lck_rqueue)){
        reader = dequeue(lockptr->lck_rqueue);
        ready(reader);
    }

    // free writers
    while(nonempty(lockptr->lck_wqueue)){
        writer = dequeue(lockptr->lck_wqueue);
        ready(writer);

    }

    lockptr->lck_owner_state = UNLOCKED;
    
    resched_cntl(DEFER_STOP);
    restore(mask);
	
    return OK;
}
