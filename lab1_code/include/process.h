/* process.h - isbadpid */

// for scheduling groups
#define SRTIME 0
#define TSSCHED 1

#define XTEST 1
#define XDEBUG 0    /* set this to 0 when submitting */

/* For grading */
#if XTEST
#define XTEST_KPRINTF(...) kprintf(__VA_ARGS__)
#else
#define XTEST_KPRINTF(...)
#endif
/* For debugging */
#if XDEBUG
#define XDEBUG_KPRINTF(...) kprintf(__VA_ARGS__)
#else
#define XDEBUG_KPRINTF(...)
#endif

#define ALPHA       0.7
#define MAX_BURST   2147483647      // max burst value. equal to INT_MAX

/* Maximum number of processes in the system */

#ifndef NPROC
#define	NPROC		8
#endif		

// number of process groups
#ifndef NPROCG
#define	NPROCG		2
#endif		


// initial group priority
#define GPRIO_DEFAULT   10


/* Process state constants */

#define	PR_FREE		0	/* Process table entry is unused	*/
#define	PR_CURR		1	/* Process is currently running		*/
#define	PR_READY	2	/* Process is on ready queue		*/
#define	PR_RECV		3	/* Process waiting for message		*/
#define	PR_SLEEP	4	/* Process is sleeping			*/
#define	PR_SUSP		5	/* Process is suspended			*/
#define	PR_WAIT		6	/* Process is on semaphore queue	*/
#define	PR_RECTIM	7	/* Process is receiving with timeout	*/

/* Miscellaneous process definitions */

#define	PNMLEN		16	/* Length of process "name"		*/
#define	NULLPROC	0	/* ID of the null process		*/

/* Process initialization constants */

#define	INITSTK		65536	/* Initial process stack size		*/
#define	INITPRIO	20	/* Initial process priority		*/
#define	INITRET		userret	/* Address to which process returns	*/

/* Inline code to check process ID (assumes interrupts are disabled)	*/

#define	isbadpid(x)	( ((pid32)(x) < 0) || \
			  ((pid32)(x) >= NPROC) || \
			  (proctab[(x)].prstate == PR_FREE))

#define isbadgroup(x) (!(x == SRTIME || x == TSSCHED))
/* Number of device descriptors a process can have open */

#define NDESC		5	/* must be odd to make procent 4N bytes	*/

/* Definition of the process table (multiple of 32 bits) */

struct procent {		/* Entry in the process table		*/
	uint16	prstate;	/* Process state: PR_CURR, etc.		*/
	pri16	prprio;		/* Process priority			*/
	char	*prstkptr;	/* Saved stack pointer			*/
	char	*prstkbase;	/* Base of run time stack		*/
	uint32	prstklen;	/* Stack length in bytes		*/
	char	prname[PNMLEN];	/* Process name				*/
	sid32	prsem;		/* Semaphore on which process waits	*/
	pid32	prparent;	/* ID of the creating process		*/
	umsg32	prmsg;		/* Message sent to this process		*/
	bool8	prhasmsg;	/* Nonzero iff msg is valid		*/
	int16	prdesc[NDESC];	/* Device descriptors for process	*/
    int     grp;        // process group

    uint32  curr_burst; // variables for burst time compuations
    uint32  exp_burst;
    uint32  next_exp_burst;
    bool8    b_continue; // if process was blocked by other process
                        // set this to true; burst is continued
    uint32  tquantum;   // time quantum for TSSCHED processes
    int32  uid;         // user id
};

// size of the solaris dispatch table
#define DTABSIZE 60

// strcut for entries in solaris dispatch table, used by TSS schedular
struct tsd_ent{
    uint32 ts_quantum;
    uint32 ts_tqexp;
    uint32 ts_slpret;
};

/* Marker for the top of a process stack (used to help detect overflow)	*/
#define	STACKMAGIC	0x0A0AAAA9

extern	struct	procent proctab[];
extern	int32	prcount;	/* Currently active processes		*/
extern	pid32	currpid;	/* Currently executing process		*/
extern  pri16   procgprio[]; // priority of each scheduling group
extern  struct tsd_ent tsd_tab[];   // solaris dispatch table

