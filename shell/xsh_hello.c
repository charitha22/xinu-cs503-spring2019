#include <xinu.h>
#include <stdio.h>

shellcmd xsh_hello (int nargs, char *args[]){
    // call hello and return    
    hello(); 
    return 0;
}

