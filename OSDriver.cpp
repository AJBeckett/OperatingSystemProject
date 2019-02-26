#include <stdio.h>
#include <stdlib.h>

loader(){
    while(){
        ReadFile();
        //extract attributes to PCB
        //insert hex or instruction into RAM
    }
}

memory(){
    
}

effective_address(){
    
}

scheduler(){
    
}

dispatcher(){
    
}

waitforinterrupt(){
    
}

int main()
{
	loader();
    while(){
        scheduler();
        dispatcher();
        CPU::main();
        waitforinterrupt();
    }
}
