/*	lcreate.c - lcreate	*/
#include <xinu.h>

/* Lab 2: Complete this function */
local int32 newlock(void);

int32 lcreate() {

  // your implementation
  intmask   mask;
  int32     lock;
  
  mask = disable();

  if( (lock = newlock() ) == SYSERR){
    restore(mask);
    return SYSERR;
  }


  restore(mask);

  return lock;
}

local int32 newlock(void){
    static int32 nextlock = 0;
    int32   lock;
    int32   i;

    for(i=0; i<NLOCKS; i++){
        lock = nextlock++;
        if(nextlock >= NLOCKS)
            nextlock = 0;
        if(locktab[lock].lckstate == L_FREE){
            locktab[lock].lckstate = L_USED;
            locktab[lock].lck_ctime = msclktime;
            return lock;
        }
    }
    return SYSERR;
}
