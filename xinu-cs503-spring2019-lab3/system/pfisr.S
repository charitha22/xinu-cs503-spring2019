// page fault interrupt service routine

    .text
    .globl  pfisr
pfisr:
    cli                 # disable interrupts
    pushal              # save all regs
    call pfhandler      # call handler
    popal               # restore regs
    popl                # remove error code
    sti                 # restore interrupts
    iret
