/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

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
