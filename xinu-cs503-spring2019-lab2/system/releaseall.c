/*	releaseall.c - releaseall	*/
#include <xinu.h>
#include <stdarg.h>

/* Lab 2: Complete this function */
local syscall release (int32 ldes);

syscall releaseall (int32 numlocks, ...) {

	//your implementation goes here
    va_list ap;
    intmask mask;
    int32   i;
    int32 ret;


    mask = disable();
    ret = OK;

    va_start(ap, numlocks); 
    for(i=0; i<numlocks; i++){
        if(release(va_arg(ap, int32)) == SYSERR){
            ret = SYSERR; // if at least one release is
                            // invalid return SYSERR
        }
    }
    va_end(ap);

    restore(mask);
	return ret;
}

static syscall release (int32 ldes){
    
    struct lockent * lckptr;
    pid32 reader, writer;
    // sanity checks
    if(isbadlock(ldes)){
        return SYSERR;
    }

    lckptr = &locktab[ldes];
    if(lckptr->lckstate == L_FREE){
        return SYSERR;
    }
    
    // if this lock is help by this process lockmap
    // must have the creation time of this lock
    if(lockmap[ldes][currpid] != lckptr->lck_ctime){
        return SYSERR;
    }
  
    // check the highest priority waiting process and
    // ready them
    if( nonempty(lckptr->lck_rqueue) && nonempty(lckptr->lck_wqueue)){
        // if the write list has the highest priority process
        if(firstkey(lckptr->lck_wqueue) >= firstkey(lckptr->lck_rqueue)){
            writer = dequeue(lckptr->lck_wqueue);
            // update lock tab
            lockmap[ldes][writer] = lckptr->lck_ctime;
            lckptr->lck_owner_state = WRITE;
            ready(writer);
        }
        // if read list has the high priority release all read processes
        // with priority not less than the highest write priority process
        else{
            // defer rescheduling TODO : verify
            resched_cntl(DEFER_START);
            
            lckptr->lck_owner_state = READ;
            while(firstkey(lckptr->lck_rqueue) >= firstkey(lckptr->lck_wqueue)){
                reader = dequeue(lckptr->lck_rqueue);
                lockmap[ldes][reader] = lckptr->lck_ctime;
                ready(reader);
            }
            resched_cntl(DEFER_STOP);
        }
        
    }
    // if only write processes are in wait queue release one
    else if(nonempty(lckptr->lck_wqueue)){
        writer = dequeue(lckptr->lck_wqueue);
        lockmap[ldes][writer] = lckptr->lck_ctime;
        
        lckptr->lck_owner_state = WRITE;
        ready(writer);
    }
    // if only read processes are in wait queue release all
    else if(nonempty(lckptr->lck_rqueue)){

        // defer rescheduling TODO : verify
        resched_cntl(DEFER_START);

        lckptr->lck_owner_state = READ;
        while(nonempty(lckptr->lck_rqueue)){
            reader = dequeue(lckptr->lck_rqueue);
            lockmap[ldes][reader] = lckptr->lck_ctime;
            ready(reader);
        }
        resched_cntl(DEFER_STOP);
    }

    // both lists empty
    else{
        // mark lock as UNLOCKED
        lckptr->lck_owner_state = UNLOCKED;
        lockmap[ldes][currpid] = 0;
    }



    return OK;
}
