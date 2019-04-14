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

// keeps track of all information about the meta data frames
typedef struct mframeinfo_ {
    uint8       status;         // FR_FREE or FR_TAKEN
    pid32       pid;            // pid of the process this frame belong to
    uint8       is_data;        // is this frame can be replaced
    uint32      ref_count;      // reference count for this frame
} mframeinfo;


// backing store map info
typedef struct bsmapinfo_ {
    pid32       pid;            // pid this backing store map belong to
    uint32      n;              // n'th bsmap for this pid

} bsmapinfo;

// backing store quesry result
typedef struct bsoffsetinfo_ {
    int32     bs;
    int32     offset;
} bsoffsetinfo;

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

// otherwise won't compile
#ifndef MAX_BS_ENTRIES
#define MAX_BS_ENTRIES MAX_ID - MIN_ID + 1
#endif


extern int32	currpolicy;

#define META_ADDR   0x00400000      // starting addr of meta region
#define saddrofmeta(mframe) (META_ADDR+mframe*NBPG)

#define MFRAMES     NFRAMES   // meta data frames, can be changed
#define FR_FREE     1       // frame free
#define FR_TAKEN    0       // frame taken
#define MFRAME_ERR  3073    // all meta frames taken
extern mframeinfo mframetab[MFRAMES]; // keeps track of free frames in metadata
extern uint32 *pt0, *pt1, *pt2, *pt3, *pt_device;   // global page tables

// backing store map
extern bsmapinfo bsmaptab[MAX_BS_ENTRIES];

// keeps track of frames in FIFO order
extern uint32   fifoqueue[MFRAMES];
extern uint32   fifohead;
extern uint32   fifotail;


// keeps track number  of page faults
extern uint32 numpfaults;

#endif // __PAGING_H_
