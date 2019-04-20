/*
By Johhny Santana
*/
#pragma once

#include <stdint.h>
#include "Windows.h"
#include "queue"
#include "PCB.h"

#include <crtdbg.h>

using namespace std;

/*
	Per specification, a WORD is 4 bytes or 32bits.
*/

// Sample of Memory by Johnny Santana 
class Memory
{
private:
	page ram[521]; 

public:
	PCB PCB_arr[30];
	queue <PCB> readyQueue;
	queue <PCB> waitQueue;
	page disk[1024];
	int pageTable[1024];
	
	DWORD& MMU(int address) {
		int p = address / 4;
		int d = address % 4;
		int f = pageTable[p];
		
		return ram[f].pageArr[d];
	}

	Memory()
	{

	}

	page memory(DWORD address)
	{
		_ASSERT(address < 256);

		// Address comes in as a byte offset
		//  convert to WORD offset
		DWORD p = address / 4;
		
		return ram[p];
	}

	page& operator[](DWORD address)
	{
		_ASSERT(address < 521*sizeof(DWORD));

		DWORD p = address / 4;
		//OFFSET BASED ON BASEADDRESS FOR ACTIVE PCB
		return ram[p];
	}

};

