#include <stdio.h>
#include <stdlib.h>
#include "pcb.H"
fetch(){
    
}

decode(){
    
}

execute(){
    
}

DMA(){
    while(){
        switch(type){
            case 0: Read(ch, next(p_rec), buf[next_io]);
            case 1: Write(ch, next(p_rec), buf[next_io]);
        }
        next_io := next_io + 4;
    }
    signal(ComputeOnly);
}

ComputeOnly(){
    while(){
        ir : = Fetch(memory[map(PC)]);// fetch instruction at RAM address – mapped PC
        OSDriver::decode(ir, oc, addrptr); // part of decoding of the instruction in instr reg (ir),
        //returning the opcode (oc) and a pointer to a list of
        //significant addresses in ‘ir’ – saved elsewhere
        PC := PC + 1; // ready for next instruction, increase PC by 1 (word)
        execute(oc) {
        case 0: // corresponding code using addrptr of operands
        case 1: // corresponding code or send interrupt
        …
        }
    }
}

main(){
    
}
