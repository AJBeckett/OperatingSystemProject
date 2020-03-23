/*
By Samuel Gardiner and Alissa Beckett

*/
#pragma once

#include <iostream>
#include <cmath>
#include <string>

using namespace std;

struct registers {
	// Registers are only 32 bits wide
	// Register indexes in instruction are 4 bits - 16 total registers

	union
	{
		DWORD Registers[16];
		struct
		{
			union
			{
				DWORD Reg0;
				DWORD Accumulator;
			};
			union
			{
				DWORD Reg1;
				DWORD ZeroRegister;
			};
			DWORD Reg2;
			DWORD Reg3;
			DWORD Reg4;
			DWORD Reg5;
			DWORD Reg6;
			DWORD Reg7;
			DWORD Reg8;
			DWORD Reg9;
			DWORD Reg10;
			DWORD Reg11;
			DWORD Reg12;
			DWORD Reg13;
			DWORD Reg14;
			DWORD Reg15;
		};
	};

	registers() : Registers{ 0 }
	{

	}

	DWORD& operator[](DWORD index)
	{
		_ASSERT(index < 16); // 0 - 15 registers
		return Registers[index];
	}

	DWORD ReadRegister(DWORD index)
	{
		_ASSERT(index < 16); // 0 - 15 registers
		DWORD value = Registers[index];
		return value;
	}

	void SetRegister(DWORD index, DWORD value)
	{
		_ASSERT(index < 16); // 0 - 15 registers
		Registers[index] = value;
	}

};

struct page {
	DWORD pageArr[4];
	boolean valid = false;
};

struct state {
	int in_buf;
	int out_buf;
	int temp_buf;

	registers m_Registers;
};

struct TStats
{
	clock_t Start;
	clock_t End;
};

struct sched {
	float burst_time;
	float pri;
	float time_slice;
	float remain_time;
};

struct status {
	bool running;
	bool ready;
	bool blocked;
	bool N;
};

//Sample of PCB.h by Samuel Gardiner and Alissa Beckett
struct PCB {
	int process_id;
	int cpuid; // information the assigned CPU (for multiprocessor system)
	int program_counter; // the job’s pc holds the address of the instruction to fetch
	state s; // record of environment that is saved on interrupt
		// including the pc, registers, permissions, buffers, caches, active
		// pages/blocks
	DWORD BaseAddress;
	int code_size; // extracted from the //JOB control line
	registers regs; // accumulators, index, general
	sched sch; // burst-time, program_Counter, queue-type, time-slice, remain-time
	status stat; // {running, ready, blocked, new}
	int priority; // of the process, extracted from the //JOB control line

	PCB() : process_id(0), cpuid(0), program_counter(0), s(), code_size(0), 
		regs(), sch(), stat(), priority(0)
	{
		
	}

	PCB& operator =(const PCB& source)
	{
		if (this != &source)
		{
			process_id = source.process_id;
			cpuid = source.cpuid;
			program_counter = source.program_counter;
			s = source.s;
			code_size = source.code_size;
			BaseAddress = source.BaseAddress;
			regs = source.regs;
			sch = source.sch;
			stat = source.stat;
			priority = source.priority;
		}
		return *this;
	}
};