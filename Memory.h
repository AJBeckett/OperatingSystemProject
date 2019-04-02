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
public:
	PCB PCB_arr[30];
	queue <PCB> readyQueue;
	DWORD disk[4096];
	DWORD ram[2084]; /* = {
	#ifdef _DEBUG
		// JOB ID#, hex # of DWORDS of code, Priority
		//  Parse Code header, loop # dwords, read into RAM, set PCB Code address and priority
		// JOB 1 17 2
		// 0000:
			0xC050005C,
			0x4B060000,
			0x4B010000,
			0x4B000000,
			0x4F0A005C,
			0x4F0D00DC,
			0x4C0A0004,
			0xC0BA0000,
			0x42BD0000,
			0x4C0D0004,
			0x4C060001,
			0x10658000,
			0x56810018,
			0x4B060000,
			0x4F0900DC,
			0x43970000,
			0x05070000,
			0x4C060001,
			0x4C090004,
			0x10658000,
			0x5681003C,
			0xC10000AC,
			0x92000000,
			// Data 14 C C  Input buffer size, output buffer size, temporary buffer
			//  Parse Data header, loop # dwords - store these values?
			//				 20 DWORDS, 12 DWORDS, 12 DWORDS 
			// Data offset is 23 * 4 (size of dword) is offset 92 which is 5C
			// 005c:
				0x0000000A,
				0x00000006,
				0x0000002C,
				0x00000045,
				0x00000001,
				0x00000007,
				0x00000000,
				0x00000001,
				0x00000005,
				0x0000000A,
				0x00000055,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
				// END
		#else
				0
		#endif
	};*/

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

		return ram[address / 4];
	}

};

