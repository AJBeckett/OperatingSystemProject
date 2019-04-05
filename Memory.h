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
	DWORD ram[2084]; 

public:
	PCB PCB_arr[30];
	queue <PCB> readyQueue;
	DWORD disk[4096];
	

	Memory()
	{

	}

	DWORD memory(DWORD address)
	{
		_ASSERT(address < 1024);

		// Address comes in as a byte offset
		//  convert to WORD offset
		DWORD offset = address / 4;
		return ram[offset];
	}

	DWORD& operator[](DWORD address)
	{
		_ASSERT(address < 2048*sizeof(DWORD));

		//OFFSET BASED ON BASEADDRESS FOR ACTIVE PCB
		return ram[address / 4];
	}

};

