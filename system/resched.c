/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;


int get_shedinfo(){
    qid16 curr, tail;
    int16 srtime_cnt = 0, tssched_cnt = 0;
    int gwin;

    // set the priority of current process's group to default
    chgprio(proctab[currpid].grp , GPRIO_DEFAULT);

    // traverse the srtime ready list
    if(nonempty(readylist_srtime)){
        curr = firstid(readylist_srtime);
        tail = queuetail(readylist_srtime);

        do{
            if(curr != NULLPROC && curr != currpid){
                srtime_cnt++;
            }
            curr = queuetab[curr].qnext;
        } while(curr != tail);
    }
    
    // traverse the srtime ready list
    if(nonempty(readylist_tssched)){
        curr = firstid(readylist_tssched);
        tail = queuetail(readylist_tssched);

        do{
            if(curr != NULLPROC && curr != currpid){
                tssched_cnt++;
            }
            curr = queuetab[curr].qnext;
        } while(curr != tail);
    }
    
    /*XDEBUG_KPRINTF("no of srtime = %d\nno of tssched =  %d\n", */
                    /*srtime_cnt, tssched_cnt);*/
     /*set new group priorities*/
    chgprio(SRTIME, (pri16)getgprio(SRTIME) + srtime_cnt);
    chgprio(TSSCHED, (pri16)getgprio(TSSCHED) + tssched_cnt);

    // select winner
    if(getgprio(SRTIME) >= getgprio(TSSCHED)) gwin = SRTIME;
    else gwin = TSSCHED;

    /*XDEBUG_KPRINTF("gwin = %d\n", gwin);*/

    return gwin;
}

void update_burst(struct procent * ptr){
    uint32 burst;
    // if previous burst was not inturrpted voluntarily
    // increment the current burst
    if(preempt == QUANTUM) burst = QUANTUM;
    else burst = QUANTUM - preempt;
    
    // new formula
    burst *= 1000;
   
    /*XDEBUG_KPRINTF(" burst = %d\n", burst);*/
    if(ptr->b_continue) ptr->curr_burst += burst;
    else ptr->curr_burst = burst;
    
    // if the process if force to context switch then continue the burst
    if(preempt == QUANTUM && ptr->prstate == PR_CURR) ptr->b_continue = TRUE;
    else ptr->b_continue = FALSE;

    // update the old current expected burst
    if(! ptr->b_continue) ptr->exp_burst = ptr->next_exp_burst;

    // compute the next expected burst
    // TODO : verify the eq
    ptr->next_exp_burst = (ptr->curr_burst * 7/10) + (ptr->exp_burst * 3/10);

    // hack
    /*if(currpid  == NULLPROC) ptr->next_exp_burst = MAX_BURST;*/
    
    // since the next burst is updated. remove this process
    // from the ready_list list and re-insert it
    if(ptr->prstate == PR_READY){
        getitem(currpid);
        queuetab[currpid].qnext = EMPTY;
        queuetab[currpid].qprev = EMPTY;
        insert(currpid, readylist_srtime, MAX_BURST - ptr->next_exp_burst);
    }


    /*XDEBUG_KPRINTF(" exepected burst = %d process name = %s  burst = %d\n", */
                /*ptr->next_exp_burst, ptr->prname, burst);*/


}

void update_prio(struct procent* ptr){
    struct tsd_ent entry;
    entry = tsd_tab[ptr->prprio];
    // if the process is cpu bound
    if(preempt == QUANTUM && ptr->prstate == PR_CURR) {
        
        ptr->prprio = entry.ts_tqexp;
        ptr->tquantum = entry.ts_quantum;
    }
    // if io bound
    else {
        ptr->prprio = entry.ts_slpret;
        ptr->tquantum = entry.ts_quantum;
        
    }
    // For safety. if this process is already in ready list
    // resort the list
    if(ptr->prstate == PR_READY){
        getitem(currpid);
        queuetab[currpid].qnext = EMPTY;
        queuetab[currpid].qprev = EMPTY;
        insert(currpid, readylist_tssched, ptr->prprio);

    }
}

pid32 select_process(int grp){
    pid32 wpid;
    
    // if srtime, get the last key from the srtime ready list
    if(grp == SRTIME){
        wpid = dequeue(readylist_srtime);
    }
    else{
        wpid = dequeue(readylist_tssched);
    }


    /*XDEBUG_KPRINTF("wpid = %d wpro = %s\n" , wpid, proctab[wpid].prname);*/


    return wpid;
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
    
    // if the process if SRTIME update burst time
    // for TSSCHED update the priority using dispatch table
    if (ptold->grp == SRTIME) update_burst(ptold);
    else update_prio(ptold);

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		/*if (ptold->prprio > firstkey(readylist)) {*/
			/*return;*/
		/*}*/

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		if(ptold->grp == SRTIME) insert(currpid, readylist_srtime, MAX_BURST - ptold->next_exp_burst);
		else insert(currpid, readylist_tssched, ptold->prprio);
	}

    // check group info
    win_grp = get_shedinfo();

	/* Force context switch to highest priority ready process */

	if(win_grp == SRTIME) currpid = select_process(SRTIME);
	else currpid = select_process(TSSCHED);
    
    /*XDEBUG_KPRINTF("selected process = %s\n", proctab[currpid].prname);*/
    
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
    // for TSSCHED select the appropriate time quantum 
    if(ptnew->grp == TSSCHED) preempt = ptnew->tquantum;
	else preempt = QUANTUM;		/* Reset time slice for process	*/

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
