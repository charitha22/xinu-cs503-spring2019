/*	lock.c - lock */
#include <xinu.h>

/* Lab 2: Complete this function */

syscall lock(int32 ldes, int32 type, int32 lpriority) {

        //your implementation goes here
	//most of the logic can be implemented here
	//consider different cases as mentioned in the handout
    intmask mask;
    struct  lockent* lckptr;
    int32 largest_write_lprio;
    struct procent* prptr;

    mask = disable();
    if(isbadlock(ldes)){
        restore(mask);
        return SYSERR;
    }

    lckptr = &locktab[ldes];
    if(lckptr->lckstate == L_FREE){
        restore(mask);
        return SYSERR;
    }

    /*kprintf("lock owner state = %d\n", lckptr->lck_owner_state);*/
    

    // if lock if free set the lock type and continue
    if(lckptr->lck_owner_state == UNLOCKED){
        
        lckptr->lck_owner_state = type;
        // set the lock mapping. used by releaseall.
        lockmap[ldes][currpid] = TRUE;
    }
    // if lock is currently acquired by a READER
    else if(lckptr->lck_owner_state == READ) {
        // if the type is also READ
        if(type == READ){
            largest_write_lprio = lpriority;
            if(nonempty(lckptr->lck_wqueue)){
                largest_write_lprio = firstkey(lckptr->lck_wqueue);
            }
            // if the request does not have sufficient priority
            // add it to read wait list
            if(lpriority < largest_write_lprio){
                prptr = &proctab[currpid];
                prptr->prstate = PR_WAIT;
                insert(currpid, lckptr->lck_rqueue, lpriority);
                resched();
            }
            // lock is acquired by the READER
            else{
                lockmap[ldes][currpid] = TRUE;
            }
        }
        // if type is WRITE go to write wait list
        else{
            prptr = &proctab[currpid];
            prptr->prstate = PR_WAIT;
            insert(currpid, lckptr->lck_wqueue, lpriority);
            resched();
        }
    }
    // lock is currently acquired by a WRITER add the process
    // to wait list
    else{
        prptr = &proctab[currpid];
        prptr->prstate = PR_WAIT;
        // if type is READ
        if(type == READ){
            insert(currpid, lckptr->lck_rqueue, lpriority);
        }
        // if type if WRITE
        else {
            insert(currpid, lckptr->lck_wqueue, lpriority);
        }
        resched();
    }
    
    
    // if the lock is deleted this process never aquired the 
    // lock returning due to ldelete ready call
    if(lockmap[ldes][currpid] == FALSE){
        restore(mask);
        return DELETED;
    }

    restore(mask);
	return OK;
}
