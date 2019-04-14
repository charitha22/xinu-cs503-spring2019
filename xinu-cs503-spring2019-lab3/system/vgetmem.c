/* vgetmem.c - vgetmem */

#include <xinu.h>

char  	*vgetmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
  // Lab3 TODO.
    intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
    struct procent * prptr;

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

    // check if the memory is initialized
    // if not initialize
    prptr = &proctab[currpid];
    if(prptr->isvmeminit == FALSE){
        vmeminit(&(prptr->vmemlist));
        prptr->isvmeminit = TRUE;
    }

    /*kprintf("vmeminit success\n");*/
    /*kprintf("vmemlist.mnext addr = 0x%x\n", (uint32)(prptr->vmemlist.mnext));*/
    /*kprintf("vmemlist.mnext len = 0x%x\n", prptr->vmemlist.mnext->mlength);*/

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/
    
    // TODO : check this
	prev = &(proctab[currpid].vmemlist);
	curr = (proctab[currpid].vmemlist).mnext;
	while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			(proctab[currpid].vmemlist).mlength -= nbytes;
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
            /*kprintf("leftover = 0x%x\n", (uint32)leftover);*/
			prev->mnext = leftover;
            /*kprintf("hello1\n");*/
			leftover->mnext = curr->mnext;
            /*kprintf("hello2\n");*/
			leftover->mlength = curr->mlength - nbytes;
			(proctab[currpid].vmemlist).mlength -= nbytes;
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	restore(mask);

	return (char *)SYSERR;
}
