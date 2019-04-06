#include <xinu.h>

uint8 mframetab[MFRAMES];

// allocates a frame and returns the frame number
// TODO : handle errors
uint32 allocmetaframe(){
    static uint32 free_f = 0;
    uint32 i;
    
    for(i=0; i<MFRAMES; i++){
        
        free_f %= MFRAMES;
        
        if(mframetab[free_f] == FR_FREE) {
            mframetab[free_f] = FR_TAKEN;
            return free_f++;
        }
        else{
            free_f++;
        }
    }

    return MFRAME_ERR;
}


uint32* allocatept(void){
    uint32 mframe, i;
    uint32* pt_saddr; // starting addr of page table
    
    mframe = allocmetaframe();
    pt_saddr = (uint32*)saddrofmeta(mframe);
    
    // set pt_pres abd pt_write for all pages
    for(i=0; i<PAGETABSIZE; i++){
        *(pt_saddr + i) = (uint32)3;
    }

    return pt_saddr;

}

uint32* allocatepd(void){
    uint32 mframe, i;
    uint32* pd_saddr; //starting addr of page dir

    mframe = allocmetaframe();
    pd_saddr = (uint32*)saddrofmeta(mframe);

    // set pd_write in every dir entry
    for(i=0; i<PAGEDIRSIZE; i++){
        *(pd_saddr + i) = (uint32)2;
    }

    return pd_saddr;
}


uint32* globalpagetablesinit(void){
    uint32* page_dir;
    uint32* pt0, *pt1, *pt2, *pt3, *pt_device;

    page_dir = allocatepd();
    
    // first page table, frames 0, 1023
    pt0 = allocatept();
    identitymappt(pt0, (char*)0x0);
    
    // frames 1024 - 4096
    pt1 = allocatept();
    identitymappt(pt1, (char*)0x400000);
    pt2 = allocatept();
    identitymappt(pt2, (char*)0x800000);
    pt3 = allocatept();
    identitymappt(pt3, (char*)0xc00000);

    // frames for device
    pt_device = allocatept(); 
    identitymappt(pt_device, (char*)0x90000000);

    // set the page dir for each page table
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
        *(page_table + i) |= (uint32)(start+NBPG*i);
        /*uint32* ptr = (page_table+i);*/
        /*kprintf("setting address %x to value %x\n", ptr, *ptr);*/
    }
    /*kprintf("done page table\n");*/
}




