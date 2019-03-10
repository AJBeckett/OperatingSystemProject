typedef struct state{
    
}

typedef struct registers{
    
}

typedef struct sched{
    float burst_time;
    float pri;
    float time_slice;
    float remain_time;
}

typedef struct status{
    bool running;
    bool ready;
    bool blocked;
    bool new;
}

typedef struct PCB{
    int cpuid: // information the assigned CPU (for multiprocessor system)
    int program-counter // the job’s pc holds the address of the instruction to fetch
    struct state: // record of environment that is saved on interrupt
        // including the pc, registers, permissions, buffers, caches, active
        // pages/blocks
    int code-size; // extracted from the //JOB control line
    struct registers: // accumulators, index, general
    struct sched: // burst-time, priority, queue-type, time-slice, remain-time
    struct status; // {running, ready, blocked, new}
    int priority; // of the process, extracted from the //JOB control line
}