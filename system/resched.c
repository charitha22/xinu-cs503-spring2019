/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;


int get_shedinfo(){
    qid16 curr, last;
    int16 srtime_cnt = 0, tssched_cnt = 0;
    int gwin;

    // set the priority of current process's group to default
    chgprio(proctab[currpid].grp , GPRIO_DEFAULT);

    // traverse the list and update info, ignore null process
    // and current process in counting
    curr = firstid(readylist);
    last = lastid(readylist);

    do{
        if(curr != NULLPROC && curr != currpid){
            if(proctab[curr].grp == SRTIME) srtime_cnt++;
            else if(proctab[curr].grp == TSSCHED) tssched_cnt++;
        }
        curr = queuetab[curr].qnext;
    } while(curr != last);

    
    /*XDEBUG_KPRINTF("no of srtime = %d\nno of tssched =  %d\n", */
                    /*info.srtime_count, info.tssched_count);*/
    // set new group priorities
    chgprio(SRTIME, (pri16)getgprio(SRTIME) + srtime_cnt);
    chgprio(TSSCHED, (pri16)getgprio(TSSCHED) + tssched_cnt);

    // select winner
    if(getgprio(SRTIME) >= getgprio(TSSCHED)) gwin = SRTIME;
    else gwin = TSSCHED;

    return gwin;
}


/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
    int     win_grp;

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		/*if (ptold->prprio > firstkey(readylist)) {*/
			/*return;*/
		/*}*/

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

    // check group info
    win_grp = get_shedinfo();

	/* Force context switch to highest priority ready process */

	currpid = dequeue(readylist);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
