/*  lock.h	*/

// declare variables, #defines, 
#ifndef NLOCKS
#define NLOCKS      100
#endif

#define READ        0
#define WRITE       1
#define L_USED      2
#define L_FREE      3
#define UNLOCKED    4

#define DELETED     5

struct	lockent {
	//struct members
    byte    lckstate;       // can be one of L_USED or L_FREE
    byte    lck_owner_state; // can be one of READ, WRITE, UNLOCKED
    qid16   lck_rqueue;     // write processes waiting on this
    qid16   lck_wqueue;     // read processes waiting on this
    //qid16   lck_queue;
};

/* Lab 3 lock table */

extern struct lockent locktab[NLOCKS];

// array to check if lock l is helpd by process p
extern bool8 lockmap[NLOCKS][NPROC]; 

#define	isbadlock(x)	((int32)(x) < 0 || (x) >= NLOCKS)
