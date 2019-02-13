/* ready.c - ready */

#include <xinu.h>

/*qid16	readylist;			[> Index of ready list		<]*/
qid16	readylist_srtime;
qid16	readylist_tssched;

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
	if(prptr->grp == SRTIME) insert(pid, readylist_srtime, prptr->prprio);
	else insert(pid, readylist_tssched, prptr->prprio);
	resched();

	return OK;
}
