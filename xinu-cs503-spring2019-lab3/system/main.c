#include <xinu.h>

extern void page_policy_test(void);

bool8 verify_cregs(){
    uint32 cr0, cr3;
    __asm__ __volatile__ (
        "mov %%cr0, %%eax\n\t"
        "mov %%eax, %0\n\t"
        "mov %%cr3, %%eax\n\t"
        "mov %%eax, %1\n\t"
        :"=m" (cr0), "=m" (cr3)
        :
        :"%eax"
    );

    /*kprintf("CR0 : 0x%x\nCR3 : 0x%x\n", cr0, cr3);*/
    if(cr0 != 0x80000013) return FALSE;

    return TRUE;
}

process	main(void)
{
  srpolicy(FIFO);

  /* Start the network */
  /* DO NOT REMOVE OR COMMENT BELOW */
#ifndef QEMU
  netstart();
#endif

  /*  TODO. Please ensure that your paging is activated 
      by checking the values from the control register.
  */
  if(!verify_cregs()) {
    kprintf("paging is not enabled");
    return SYSERR;
  }

  // trigger a page fault
  /*uint32 a = *(uint32*)0x01000000;*/
  /* Initialize the page server */
  /* DO NOT REMOVE OR COMMENT THIS CALL */
  psinit();

  page_policy_test();

  return OK;
}
