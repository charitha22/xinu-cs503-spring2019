/* ready.c - ready */

#include <xinu.h>

qid16	readylist_srtime;			/* Index of srtime ready list		*/
qid16	readylist_tssched;			/* Index of tssched ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
    // check the scheduling group and insert appropriately
    if(prptr->grp == SRTIME) insert(pid, readylist_srtime, prptr->prprio);
    else insert(pid, readylist_tssched, prptr->prprio);

	resched();

	return OK;
}
