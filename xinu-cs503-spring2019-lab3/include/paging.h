/* paging.h */

#ifndef __PAGING_H_
#define __PAGING_H_

/* Structure for a page directory entry */

typedef struct pd_t_ {
	unsigned int pd_pres	: 1;		/* page table present?		*/
	unsigned int pd_write : 1;		/* page is writable?		*/
	unsigned int pd_user	: 1;		/* is use level protection?	*/
	unsigned int pd_pwt	: 1;		/* write through cachine for pt? */
	unsigned int pd_pcd	: 1;		/* cache disable for this pt?	*/
	unsigned int pd_acc	: 1;		/* page table was accessed?	*/
	unsigned int pd_mbz	: 1;		/* must be zero			*/
	unsigned int pd_fmb	: 1;		/* four MB pages?		*/
	unsigned int pd_global: 1;		/* global (ignored)		*/
	unsigned int pd_avail : 3;		/* for programmer's use		*/
	unsigned int pd_base	: 20;		/* location of page table?	*/
} pd_t;

/* Structure for a page table entry */

typedef struct pt_t_{
	unsigned int pt_pres	: 1;		/* page is present?		*/
	unsigned int pt_write : 1;		/* page is writable?		*/
	unsigned int pt_user	: 1;		/* is use level protection?	*/
	unsigned int pt_pwt	: 1;		/* write through for this page? */
	unsigned int pt_pcd	: 1;		/* cache disable for this page? */
	unsigned int pt_acc	: 1;		/* page was accessed?		*/
	unsigned int pt_dirty : 1;		/* page was written?		*/
	unsigned int pt_mbz	: 1;		/* must be zero			*/
	unsigned int pt_global: 1;		/* should be zero in 586	*/
	unsigned int pt_avail : 3;		/* for programmer's use		*/
	unsigned int pt_base	: 20;		/* location of page?		*/
} pt_t;

#define PAGEDIRSIZE	1024
#define PAGETABSIZE	1024

#define NBPG		4096	/* number of bytes per page	*/
#define FRAME0		1024	/* zero-th frame		*/

#ifndef NFRAMES
#define NFRAMES		3072	/* number of frames		*/
#endif

#define MAP_SHARED 1
#define MAP_PRIVATE 2

#define FIFO 3
#define GCA 4

#define MAX_ID		7		/* You get 8 mappings, 0 - 7 */
#define MIN_ID		0

extern int32	currpolicy;

#define META_ADDR   0x00400000      // starting addr of meta region
#define saddrofmeta(mframe) (META_ADDR+mframe*NBPG)

#define MFRAMES     3072    // meta data frames
#define FR_FREE     1       // frame free
#define FR_TAKEN    0       // frame taken
#define MFRAME_ERR  3073    // all meta frames taken
extern uint8 mframetab[MFRAMES]; // keeps track of free frames in metadata

#endif // __PAGING_H_