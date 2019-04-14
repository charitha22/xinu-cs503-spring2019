#include <xinu.h>

mframeinfo mframetab[MFRAMES];
uint32 *pt0, *pt1, *pt2, *pt3, *pt_device;   

bsmapinfo bsmaptab[MAX_BS_ENTRIES];
uint32 fifoqueue[MFRAMES];
uint32 fifohead;
uint32 fifotail;

// counter for total page faults
uint32 numpfaults;

// allocates a frame and returns the frame number
// TODO : handle errors
uint32 allocmetaframe(pid32 pid, uint8 is_data){
    static uint32 free_f = 0;
    uint32 i;
        
    for(i=0; i<MFRAMES; i++){
        
        free_f %= MFRAMES;
        
        if(mframetab[free_f].status == FR_FREE) {
            mframetab[free_f].status = FR_TAKEN;
            mframetab[free_f].pid = pid;
            mframetab[free_f].is_data = is_data;
            return free_f++;
        }
        else{
            free_f++;
        }
    }

    return MFRAME_ERR;
}


uint32* allocatept(pid32 pid){
    uint32 mframe, i;
    uint32* pt_saddr; // starting addr of page table
    
    mframe = allocmetaframe(pid, FALSE);
    if(mframe == MFRAME_ERR){
        panic("free frames not available for page table");
    }
    pt_saddr = (uint32*)saddrofmeta(mframe);
    
    // set pt_pres abd pt_write for all pages
    for(i=0; i<PAGETABSIZE; i++){
        *(pt_saddr + i) = (uint32)2;
    }

    return pt_saddr;

}

uint32* allocatepd(pid32 pid){
    uint32 mframe, i;
    uint32* pd_saddr; //starting addr of page dir
    
    /*kprintf("allocating page dir for pid %d\n", pid);*/
    mframe = allocmetaframe(pid, FALSE);
    if(mframe == MFRAME_ERR){
        panic("free frames not available for page directory");
    }


    pd_saddr = (uint32*)saddrofmeta(mframe);

    // set pd_write in every dir entry
    for(i=0; i<PAGEDIRSIZE; i++){
        *(pd_saddr + i) = (uint32)2;
    }

    return pd_saddr;
}


// only for null process
uint32* globalpagetablesinit(void){
    uint32* page_dir;
    /*uint32* pt0, *pt1, *pt2, *pt3, *pt_device;*/

    page_dir = allocatepd(NULLPROC);
    
    // first page table, frames 0, 1023
    pt0 = allocatept(NULLPROC);
    identitymappt(pt0, (char*)0x0);
    
    // frames 1024 - 4096
    pt1 = allocatept(NULLPROC);
    identitymappt(pt1, (char*)0x400000);
    pt2 = allocatept(NULLPROC);
    identitymappt(pt2, (char*)0x800000);
    pt3 = allocatept(NULLPROC);
    identitymappt(pt3, (char*)0xc00000);

    // frames for device
    pt_device = allocatept(NULLPROC); 
    identitymappt(pt_device, (char*)0x90000000);

    // set the page dir for each page table
    setpagedirectory(page_dir, pt0);
    setpagedirectory(page_dir, pt1);
    setpagedirectory(page_dir, pt2);
    setpagedirectory(page_dir, pt3);
    setpagedirectory(page_dir, pt_device);

    return page_dir;
}

// for processes created by create, vcreate
uint32* createpagedir(pid32 pid){
    uint32* page_dir;

    page_dir = allocatepd(pid);

    // set the 5 global page tables, assumes
    // they are allocate dy null process
    setpagedirectory(page_dir, pt0);
    setpagedirectory(page_dir, pt1);
    setpagedirectory(page_dir, pt2);
    setpagedirectory(page_dir, pt3);
    setpagedirectory(page_dir, pt_device);
   
    return page_dir;

}


void setpagedirectory(uint32* pd_addr, uint32* pt_addr){
    
    uint32 offset;
    
    // offset in page dir is the most sig 10 bits
    offset = *(pt_addr) >> 22;
    /*kprintf("offset %d\n", offset);*/

    // set the page directory entry and mark pd_pres
    *(pd_addr + offset) |= ((uint32)pt_addr |(uint32)1);
    /*uint32* ptr = pd_addr + offset;*/
    /*kprintf("setting page directory");*/
    /*kprintf("setting address %x to value %x\n", ptr, *ptr);*/

}

void identitymappt(uint32* page_table, char* start){
    // set all entries in page table to ponit to
    // 4096 chunks starting from 'start'
    uint32 i;

    for(i=0; i<PAGETABSIZE; i++){
        *(page_table + i) |= ((uint32)(start+NBPG*i) | 1);
        /*uint32* ptr = (page_table+i);*/
        /*kprintf("setting address %x to value %x\n", ptr, *ptr);*/
    }
    /*kprintf("done page table\n");*/
}

void printpagedir(uint32* pagedir){
    intmask mask;
    uint32 i;

    mask = disable();
    kprintf("adress of the page table = 0x%x\n", (uint32)pagedir);
    kprintf("address : content \n");
    
    for(i=0; i<PAGEDIRSIZE; i++){
        kprintf("0x%x : 0x%x\n", (uint32)(pagedir + i), *(pagedir+i));
    }

    restore(mask);
}

void incrementrefcount(uint32* faddr){
    uint32 mframenum;

    if(((uint32)faddr < META_ADDR) | ((uint32)faddr >= 0x1000000)){
        return;
    }

    mframenum = ((uint32)faddr - META_ADDR) >> 12;

    mframetab[mframenum].ref_count++;
    
}

void decrementrefcount(uint32* faddr){
    uint32 mframenum;

    if(((uint32)faddr < META_ADDR) |( (uint32)faddr >= 0x1000000)){
        return;
    }

    mframenum = ((uint32)faddr - META_ADDR)/NBPG;

    mframetab[mframenum].ref_count--;
 
}

bsoffsetinfo findposbs(pid32 pid, char* vaddr){
    uint32 vp;
    uint32 n; 
    bsoffsetinfo result;
    uint32 i;

    // init the result
    result.bs = -1;
    result.offset = -1;

    // get the vp number
    vp = (((uint32) vaddr) - 0x1000000) >> 12;
    
    // if pid has multiple backing stores on which backing store
    // this vddr is in 0,1,2,....
    n = vp / 200;

    
    // iterate the bsmaptab
    for(i=0; i<MAX_BS_ENTRIES; i++){
        if(bsmaptab[i].pid == pid && bsmaptab[i].n == n){
            result.bs = i;
            result.offset = vp - 200*n; // TODO : check this
            return result;
        }
    }
    
    
    return result;


}


// Simple queue structure
// TODO : no error checks 
void fifoenqueue(uint32 mframeid){
    fifoqueue[fifohead] = mframeid;
    fifohead ++;
    fifohead = fifohead%MFRAMES;
}

uint32 fifodequeue(){
    uint32 result;
    result = fifoqueue[fifotail];
    fifoqueue[fifotail] = -1;
    fifotail++;
    fifotail = fifotail%MFRAMES;

    return result;
}






