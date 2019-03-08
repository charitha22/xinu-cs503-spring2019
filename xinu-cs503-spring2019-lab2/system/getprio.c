/* getprio.c - getprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getprio  -  Return the scheduling priority of a process
 *------------------------------------------------------------------------
 */
// NOTE : this functio is updated to get the effiective priority with
// priority inheritance
// TODO : check side effects
syscall	getprio(
	  pid32		pid		/* Process ID			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	prio = proctab[pid].prinh != 0 ? proctab[pid].prinh : proctab[pid].prprio;
	restore(mask);
	return prio;
}
