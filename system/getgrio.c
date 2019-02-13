#include <xinu.h>

syscall getgprio(
    int16 group // process group
    )
{
    intmask mask;
    uint32 prio;

    mask = disable();
    if(isbadgroup(group)){
        restore(mask);
        return SYSERR;
    }
    prio = procgprio[group];

    restore(mask);
    return prio;
}
