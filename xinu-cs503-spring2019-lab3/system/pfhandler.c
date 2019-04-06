#include <xinu.h>

void pfhandler(){
    intmask mask;
    mask = disable();
    
    panic("page fault");

    restore(mask);
}
