#include <xinu.h>

void pfhandler(){
    intmask mask;
    uint32 cr2;         // faulty address
    uint32 vp;          // virtual page number
    uint32* pd;         // page dir for this process
    uint32* pd_entry;         // page table
    uint32 p;           // which page table
    uint32 q;           // which page
    bsoffsetinfo bsoffset; // which backing store info

    uint32* new_pt;     // if pd entry is not present
    uint32  freemframe; // free meta frame to load page
    char* freemframeaddr;  // address of the free page selected
    

    mask = disable();
    
    /*kprintf("\nCALL : pfhandler\n");*/
    // update the number of page faults
    numpfaults ++;

    // get faulted address
    __asm__ __volatile__ (
        "mov   %%cr2, %%eax\n\t"
        "mov   %%eax, %0\n\t"
        :"=m" (cr2)
        :
        :"%eax"
    );
    
    /*kprintf("faulty address : 0x%x\n", cr2);*/
    /*panic("page fault");*/
    
    // get vp number
    vp = cr2 >> 12;

    // TODO : check if cr2 is valid address


    pd = proctab[currpid].pagedir;

    // compute p and q
    p = cr2 >> 22;
    q = (cr2 >> 12) & 0x3ff;

    /*kprintf("page dir = 0x%x\n", (uint32)pd);*/
    /*kprintf("p = %d, q = %d\n", p , q);*/

    // get the address of the page table
    pd_entry = (uint32*)(pd + p);

    // check if the p'th page table exists
    if((*pd_entry & 1) == 0){
        /*kprintf("page table does not exist\n");*/
        // get a new frame for page table
        new_pt = allocatept(currpid);
        // call hook for a page table create
        hook_ptable_create((uint32)new_pt >> 12);   

        /*kprintf("meta frame selected for page table = %d\n", (uint32)new_pt >> 12);*/
       
        // set the pd entry for the new page table 
        // and mark page table present
        * pd_entry |= (((uint32)new_pt) | 1 );

    }

    // increment the ref count
    incrementrefcount(*pd_entry);

    // now pd entry is present
    bsoffset = findposbs(currpid, (char*)cr2);

    // obtain a free frame for data
    freemframe = allocmetaframe(currpid, TRUE);
    
    if(freemframe == MFRAME_ERR){
        /*panic("free frames not availeble, this case not handled");*/
        if(currpolicy == FIFO){
            freemframe = allocateframeFIFO();             
            fifoenqueue(freemframe);
        }
        else{
            panic("CGA policy is not implemented");
        }
    }
    else{
        // free frame is availble update the replacement data structures
        if(currpolicy == FIFO){
            fifoenqueue(freemframe);
        }
        else{
            // GCA is not implemented
            panic("CGA policy is not implemented");
        }
    }

    /*kprintf("free mframe id selected for data = %d\n", freemframe);*/
    // set the virtual page number in the inverted page table
    mframetab[freemframe].vp = vp;

    freemframeaddr = (char*)saddrofmeta(freemframe);
    // update the page table for the new page
    // and mark page present
    uint32* addrofpt = (*pd_entry & 0xfffff000);
    *(addrofpt +  q) |= ((uint32)freemframeaddr | 1);

    // read the content from backing strore to this free page
    if(read_bs(freemframeaddr, bsoffset.bs, bsoffset.offset) == SYSERR){
        panic("backing store read unsuccessful");
    }

    /*kprintf("dirtybit before  write = 0x%x\n", *(addrofpt+q) & 0x40);*/
    /**(uint32*)cr2 = 1;*/
    /*kprintf("dirtybit after  write = 0x%x\n", *(addrofpt+q) & 0x40);*/


    
    // call hook for page fault
    hook_pfault(currpid, (void*)cr2, vp, (uint32)freemframeaddr >> 12);

    /*panic("page fault");*/
    restore(mask);
}
