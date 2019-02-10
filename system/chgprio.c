#include <xinu.h>

// function to check valid group 
// numbers
bool8 isbadgroup(int group){
    return !(group == SRTIME || group == TSSCHED);
}


pri16 chgprio(int group, pri16 newprio){
    intmask mask;
    pri16 oldprio;

    mask = disable();
    if(isbadgroup(group)){
        restore(mask);
        return (pri16) SYSERR;
    }
    // change the priority of the group
    oldprio = procgprio[group];
    procgprio[group] = newprio;

    restore(mask);
    return oldprio;
}
