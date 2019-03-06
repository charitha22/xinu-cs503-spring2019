/*	linit.c	- linit	initialize lock system */
#include <xinu.h>

/* Lab 2: Complete this function */

// declare any global variables here
struct  lockent locktab[NLOCKS];    // lock table
bool8   lockmap[NLOCKS][NPROC];


void linit(void) {
  
	// your implementation goes here
        // make sure to call this in initialize.c

    struct  lockent *lockptr;   // Ptr to lock table entry
    int32 i, j;

    // intitialize the locks
    for(i = 0; i < NLOCKS; i++){
        lockptr = &locktab[i];
        lockptr->lckstate = L_FREE;
        lockptr->lck_owner_state = UNLOCKED;
        lockptr->lck_rqueue = newqueue();
        lockptr->lck_wqueue = newqueue();
    }

    // initialize the lock mapping
    for(i=0; i<NLOCKS; i++){
        for(j=0; j<NPROC; j++){
            lockmap[i][j] = FALSE;
        }
    }

}
