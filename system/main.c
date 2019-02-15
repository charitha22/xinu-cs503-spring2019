/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

    /*XDEBUG_KPRINTF("solaris dispatch table\n");*/
    /*for(int i=0; i<DTABSIZE; i++){*/
        /*XDEBUG_KPRINTF("%d %d %d\n", */
            /*tsd_tab[i].ts_quantum, tsd_tab[i].ts_tqexp, tsd_tab[i].ts_slpret);*/
    /*}*/

	/* Run the Xinu shell */

	recvclr();
	pid32 pid = create(shell, 8195, SRTIME, 50, "shell", 1, CONSOLE);
    XTEST_KPRINTF("Spawning new shell with PID = %d...\n", pid);
    resume(pid);

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		pid = create(shell, 4096, SRTIME, 20, "shell", 1, CONSOLE);
        XTEST_KPRINTF("Spawning new shell with PID = %d...\n", pid);
        resume(pid);
	}
	return OK;
    
}
