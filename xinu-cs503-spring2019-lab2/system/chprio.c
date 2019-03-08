/* chprio.c - chprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  chprio  -  Change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
pri16	chprio(
	  pid32		pid,		/* ID of process to change	*/
	  pri16		newprio		/* New priority			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	pri16	oldprio;		/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (pri16) SYSERR;
	}
	prptr = &proctab[pid];
	oldprio = prptr->prprio;
	prptr->prprio = newprio;
    // if this process is in the ready list
    // remove it and insert it again
    if(prptr->prstate == PR_READY){
        getitem(pid);
        insert(pid, readylist, max(prptr->prprio, prptr->prinh));
    }
	restore(mask);
	return oldprio;
}


pri16   chinhprio(  
        pid32 pid, 
        pri16 newprio)
{
    intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	pri16	oldinhprio;		/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (pri16) SYSERR;
	}
	prptr = &proctab[pid];
	oldinhprio = prptr->prinh;
	prptr->prinh = newprio;
    
    // update the ready list based on inherited
    // priority
    if(prptr->prstate == PR_READY){
        getitem(pid);
        insert(pid, readylist, prptr->prinh);
    }
	restore(mask);
	return oldinhprio;

}
