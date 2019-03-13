#include <iostream>
#include <cmath>
#include <string>

struct state{
    int *in_buf;
    int *out_buf;
    int *temp_buf;
};

struct registers{
    
};

struct sched{
    float burst_time;
    float pri;
    float time_slice;
    float remain_time;
};

struct status{
    bool running;
    bool ready;
    bool blocked;
    bool N;
};

struct PCB{
    int process_id;
    int cpuid; // information the assigned CPU (for multiprocessor system)
    int program_counter; // the job’s pc holds the address of the instruction to fetch
    state s; // record of environment that is saved on interrupt
        // including the pc, registers, permissions, buffers, caches, active
        // pages/blocks
    int code_size; // extracted from the //JOB control line
    registers regs; // accumulators, index, general
    sched sch; // burst-time, priority, queue-type, time-slice, remain-time
    status stat; // {running, ready, blocked, new}
    int priority; // of the process, extracted from the //JOB control line
};