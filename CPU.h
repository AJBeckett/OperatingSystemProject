/*
By Johnny Santana, Samuel Gardiner, Derick Ehouman, and Alissa Beckett
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "LTS.h"

using namespace std;

class CPU
{
private:
	//int initialCount = 0;
	int count = 0;
	state	m_State;	// Active context
	PCB		m_ActiveProcess;
public:
	unsigned int processID;

	using TPrefixOpcode = struct
	{
		DWORD	Reserved : 24;
		DWORD	OpCode : 6;
		DWORD	Prefix : 2;
	};
	using TPrefix00 = struct // Type R
	{
		DWORD Zero : 12;
		DWORD DestinationRegisterIndex : 4;
		DWORD SourceRegisterIndex1 : 4;
		DWORD SourceRegisterIndex0 : 4;
		DWORD OpCode : 6;
		DWORD Prefix : 2;
	};

	using TPrefix01 = struct // Type I
	{
		DWORD Address : 16;
		DWORD DestinationRegisterIndex : 4;
		DWORD BaseRegisterIndex : 4;
		DWORD OpCode : 6;
		DWORD Prefix : 2;
	};

	using TPrefix10 = struct // Type J
	{
		DWORD Address : 24;
		DWORD OpCode : 6;
		DWORD Prefix : 2;
	};

	using TPrefix11 = struct // Type I/O
	{
		DWORD Address : 16;
		DWORD RegisterIndex1 : 4;
		DWORD RegisterIndex0 : 4;
		DWORD OpCode : 6;
		DWORD Prefix : 2;
	};

	union TDecoding
	{
		DWORD		Instruction;
		TPrefixOpcode PrefixOpcode;
		TPrefix00   Prefix00;
		TPrefix01	Prefix01;
		TPrefix10	Prefix10;
		TPrefix11	Prefix11;
	};

	/*CPU() : m_State(), m_Memory.ram()
	{

	}*/


	void AssignProcess(PCB& newProcess)
	{
		m_ActiveProcess = newProcess;
	}

	void SwitchProcess(PCB& newProcess)
	{
		// Save state into current process
		//m_ActiveProcess.SaveState(m_State);
		// Retrieve state of new process
		//m_State = newProcess.GetState();
		m_ActiveProcess = newProcess;
	}

	void SaveContext(state& savedState)
	{
		// Make a copy of the active State
		savedState = m_State;
	}

	DWORD ProgramCounter()
	{
		//return initialCount;
		return m_ActiveProcess.program_counter;
	}

	void IncrementProgramCounter()
	{
		m_ActiveProcess.program_counter++;
		//count++;
	}


	DWORD Fetch() {
		// Each instruction is 32 bits long
		// TO DO:
		//   Active Process base address in memory?
		DWORD& instruction = m_Memory[(m_ActiveProcess.BaseAddress + m_ActiveProcess.program_counter) * sizeof(DWORD)];
		return instruction;
	}

	uint8_t Decode(DWORD instruction, TDecoding& decoding) {
		decoding.Instruction = instruction;
		return decoding.PrefixOpcode.OpCode;

	}

	void SetRegister(DWORD registerIndex, DWORD value)
	{
		_ASSERT(registerIndex < 16);
		m_State.m_Registers[registerIndex] = value;
	}

	void WriteMemory(DWORD address, DWORD index)
	{
		_ASSERT(index < 16); // 0-15 registers
		address = m_State.m_Registers[index];
	}

	void Execute(PCB process) 
	{
		AssignProcess(process);

		do
		{
			
			DWORD instruction = Fetch();
			// instruction = 0xC10000AC;

			TDecoding decoding;
			decoding.Instruction = instruction;

			IncrementProgramCounter();

			switch (decoding.PrefixOpcode.OpCode)
			{
			case 0x00: // Instruction is RD, Type is I/O
			{
				// 0xC050005C;  11 - 000000 - 0101 - 0000 – 0x005C
				//			IO – RD – index of destination register (0x05) – ignored index – address of input buffer (0x5C)
				//      0x000A --> R5
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x03);
				DWORD address = decoding.Prefix11.Address + process.BaseAddress * 4;
				DWORD data = m_Memory[address];

				SetRegister(decoding.Prefix11.RegisterIndex0, data);;

				wprintf_s(L"RD (OpCode: 0x%X) 0x%X into register %d\n",
					decoding.PrefixOpcode.OpCode, data, decoding.Prefix11.RegisterIndex0);
				break;
			}
			case 0x01: // Instruction is WR, Type is I/O
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x03);
				DWORD address = decoding.Prefix11.Address + process.BaseAddress * 4;
				DWORD accumulatorValue = m_State.m_Registers.Accumulator;
				DWORD register1 = decoding.Prefix11.RegisterIndex1;
				
				m_Memory[address] = accumulatorValue;

				wprintf_s(L"WR (OpCode: 0x%X) RAM[0x%X] <-- 0x%x (Reg%d)\n",
					decoding.PrefixOpcode.OpCode,
					address,
					accumulatorValue,
					decoding.Prefix11.RegisterIndex0);
				break;
			}
			case 0x02: // Instruction is ST, Type is I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);
				DWORD address = decoding.Prefix01.Address + destinationRegister + process.BaseAddress * 4;
				//(I am an address in memory)
				//m_Memory.ram[R13] = R11
				m_Memory[address] = baseRegister;

				//destinationRegister = effectiveAddress;

				wprintf_s(L"ST (OpCode: 0x%X) RAM[0x%X] <-- 0x%X (Reg%d)\n",
					decoding.PrefixOpcode.OpCode,
					address,
					baseRegister,
					decoding.Prefix01.DestinationRegisterIndex);;
				break;
			}
			case 0x03: //Instruction LW, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);
				DWORD effectiveAddress = address + baseRegister;

				DWORD memoryValue = m_Memory[effectiveAddress];
				m_State.m_Registers.SetRegister(destinationRegisterIndex, memoryValue);

				wprintf_s(L"LW (OpCode: 0x%X) Content of Address 0x%X (0x%X) into reg%d\n",
					decoding.PrefixOpcode.OpCode, address, memoryValue, decoding.Prefix01.DestinationRegisterIndex);
				break;
			}
			case 0x04: // Instruction MOV, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;

				SetRegister(destinationRegisterIndex, sourceRegisterIndex1);

				wprintf_s(L"MOV (OpCode: 0x%X) Content of reg%d: (0x%X) moved to reg%d\n",
					decoding.PrefixOpcode.OpCode, sourceRegisterIndex1, m_State.m_Registers.ReadRegister(sourceRegisterIndex1), decoding.Prefix00.DestinationRegisterIndex);


				break;
			}
			case 0x05: // Instruction ADD, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegister1 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex1];
				DWORD sourceRegister0 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex0];

				DWORD answer = sourceRegister0 + sourceRegister1;
				SetRegister(destinationRegisterIndex, answer);

				wprintf_s(L"ADD (OpCode: 0x%X) Content of Reg%d: (0x%X) added to Content of Reg%d: (0x%X) into reg%d: (0x%X)\n",
					decoding.PrefixOpcode.OpCode,
					decoding.Prefix00.SourceRegisterIndex0,
					sourceRegister1,
					decoding.Prefix00.SourceRegisterIndex1,
					sourceRegister0,
					decoding.Prefix00.DestinationRegisterIndex,
					answer);

				break;
			}
			case 0x06: // Instruction SUB, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegister1 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex1];
				DWORD sourceRegister0 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex0];

				DWORD answer = sourceRegister0 - sourceRegister1;
				SetRegister(destinationRegisterIndex, answer);

				wprintf_s(L"SUB (OpCode: 0x%X) Content of reg%d: (0x%x) subtracted by Content of reg%d: (0x%x) into reg%d: (0x%X)\n",
					decoding.PrefixOpcode.OpCode, decoding.Prefix00.SourceRegisterIndex0, sourceRegister1, decoding.Prefix00.SourceRegisterIndex1, sourceRegister0, decoding.Prefix00.DestinationRegisterIndex, answer);

				break;
			}
			case 0x07: // Instruction MUL, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegister1 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex1];
				DWORD sourceRegister0 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex0];

				DWORD answer = sourceRegister0 * sourceRegister1;
				SetRegister(destinationRegisterIndex, answer);

				wprintf_s(L"MUL (OpCode: 0x%X) Content of reg%d: (0x%x) multiplied by the Content of reg%d: (0x%x) into reg%d: (0x%X)\n",
					decoding.PrefixOpcode.OpCode, decoding.Prefix00.SourceRegisterIndex0, sourceRegister1, decoding.Prefix00.SourceRegisterIndex1, sourceRegister0, decoding.Prefix00.DestinationRegisterIndex, answer);
				break;
			}
			case 0x08: // Instruction DIV, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegister1 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex1];
				DWORD sourceRegister0 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex0];

				DWORD answer = sourceRegister0 / sourceRegister1;
				SetRegister(destinationRegisterIndex, answer);

				wprintf_s(L"DIV (OpCode: 0x%X) Content of reg%d: (0x%x) divided by the Content of reg%d: (0x%x) into reg%d: (0x%X)\n",
					decoding.PrefixOpcode.OpCode, decoding.Prefix00.SourceRegisterIndex0, sourceRegister1, decoding.Prefix00.SourceRegisterIndex1, sourceRegister0, decoding.Prefix00.DestinationRegisterIndex, answer);
				break;
			}
			case 0x09: // Instruction AND, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegister1 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex1];
				DWORD sourceRegister0 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex0];

				DWORD answer = sourceRegister0 && sourceRegister1;
				SetRegister(destinationRegisterIndex, answer);

				wprintf_s(L"AND (OpCode: 0x%X) reg%d: 0x%X AND reg%d: 0x%X into reg%d: (0x%X)\n",
					decoding.PrefixOpcode.OpCode, decoding.Prefix00.SourceRegisterIndex0, sourceRegister1, decoding.Prefix00.SourceRegisterIndex1, sourceRegister0, decoding.Prefix00.DestinationRegisterIndex, answer);
				break;
			}
			case 0x0A: // Instruction OR, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegister1 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex1];
				DWORD sourceRegister0 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex0];

				DWORD answer = sourceRegister0 || sourceRegister1;
				SetRegister(destinationRegisterIndex, answer);

				wprintf_s(L"OR (OpCode: 0x%X) reg%d: 0x%X OR reg%d: 0x%X into reg%d: (0x%X)\n",
					decoding.PrefixOpcode.OpCode, decoding.Prefix00.SourceRegisterIndex0, sourceRegister1, decoding.Prefix00.SourceRegisterIndex1, sourceRegister0, decoding.Prefix00.DestinationRegisterIndex, answer);
				break;
			}
			case 0x0B: // Instruction MOVI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				// move immediate data (address) into register
				SetRegister(destinationRegisterIndex, address);

				wprintf_s(L"MOVI (OpCode: 0x%X) Transfer 0x%X into reg%d\n",
					decoding.PrefixOpcode.OpCode, address, destinationRegisterIndex);
				break;
			}
			case 0x0C: // Instruction ADDI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				//DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);

				DWORD addValue = address + destinationRegister;

				SetRegister(destinationRegisterIndex, addValue);

				wprintf_s(L"ADDI (OpCode: 0x%X) Add 0x%X to the content of reg%d: (0x%x) to get 0x%X\n",
					decoding.PrefixOpcode.OpCode, address, destinationRegisterIndex, destinationRegister, addValue);

				break;
			}
			case 0x0D: // Instruction MULI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);
				DWORD multiValue = address * destinationRegister;

				SetRegister(destinationRegisterIndex, multiValue);

				wprintf_s(L"MULTI (OpCode: 0x%X) Multiply 0x%X to the content of reg%d: (0x%x) to get 0x%X\n",
					decoding.PrefixOpcode.OpCode, address, destinationRegisterIndex, destinationRegister, multiValue);

				break;
			}
			case 0x0E: // Instruction DIVI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);
				DWORD divValue = address / destinationRegister;

				SetRegister(destinationRegisterIndex, divValue);

				wprintf_s(L"DIVI (OpCode: 0x%X) Divide 0x%X by the content of reg%d: (0x%x) to get 0x%X\n",
					decoding.PrefixOpcode.OpCode, address, destinationRegisterIndex, destinationRegister, divValue);
				break;
			}
			case 0x0F: // Instruction LDI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				
				SetRegister(destinationRegisterIndex, address);

				wprintf_s(L"LDI (OpCode: 0x%X) Loads 0x%X into reg%d\n",
					decoding.PrefixOpcode.OpCode, address, destinationRegisterIndex);
				break;
			}
			case 0x10: // Instruction SLT, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegister1 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex1];
				DWORD sourceRegister0 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex0];

				if (sourceRegister0 < sourceRegister1) {
					SetRegister(destinationRegisterIndex, 1);

					wprintf_s(L"SLT (OpCode: 0x%X) content of reg%d: (0x%X) < content of reg%d: (0x%x); reg%d set to 1\n",
					decoding.PrefixOpcode.OpCode, decoding.Prefix00.SourceRegisterIndex0, sourceRegister0, decoding.Prefix00.SourceRegisterIndex1, sourceRegister1, destinationRegisterIndex);
				}

				else {
					SetRegister(destinationRegisterIndex, 0);

					wprintf_s(L"SLT (OpCode: 0x%X) content of reg%d: (0x%X) >= content of reg%d: (0x%x); reg%d set to 0\n",
						decoding.PrefixOpcode.OpCode, decoding.Prefix00.SourceRegisterIndex0, sourceRegister0, decoding.Prefix00.SourceRegisterIndex1, sourceRegister1, destinationRegisterIndex);
				}

				break;
			}
			case 0x11: // Instruction SLTI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if (baseRegister < address) {
					SetRegister(destinationRegisterIndex, 1);

					wprintf_s(L"SLTI (OpCode: 0x%X) content of reg%d: (0x%X) < address: (0x%x); reg%d set to 1\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, address, destinationRegisterIndex);
				}
				else {
					SetRegister(destinationRegisterIndex, 0);

					wprintf_s(L"SLTI (OpCode: 0x%X) content of reg%d: (0x%X) >= address: (0x%x); reg%d set to 0\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, address, destinationRegisterIndex);
				}

				break;
			}
			case 0x12: // Instruction HLT, type J
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x02);
				goto HALT;

				break;
			}
			case 0x13: // Instruction NOP, no type
			{
				//no operation
				break;
			}
			case 0x14: // Instruction JMP, type J
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x02);
				DWORD address = decoding.Prefix10.Address;
				m_ActiveProcess.program_counter = address;

				break;
			}
			case 0x15: // Instruction BEQ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if (baseRegister == destinationRegister) {
					m_ActiveProcess.program_counter = address / 4;

					wprintf_s(L"BEQ (OpCode: 0x%X) content of reg%d: (0x%X) == content of reg%d: (0x%x); branch to address 0x%X\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, destinationRegisterIndex, destinationRegister, address);
				}
				else {
					wprintf_s(L"BEQ (OpCode: 0x%X) content of reg%d: (0x%X) != content of reg%d: (0x%x); continue to next instruction\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, destinationRegisterIndex, destinationRegister);

				}

				break;
			}
			case 0x16: // Instruction BNE ,type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if (baseRegister != destinationRegister) {
					m_ActiveProcess.program_counter = address / 4;
					//count = address/4;

					wprintf_s(L"BNE (OpCode: 0x%X) content of reg%d: (0x%X) != content of reg%d: (0x%x); branch to address 0x%X\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, destinationRegisterIndex, destinationRegister, address);
				}
				else {
					wprintf_s(L"BNE (OpCode: 0x%X) content of reg%d: (0x%X) == content of reg%d: (0x%x); continue to next instruction\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, destinationRegisterIndex, destinationRegister);

				}
				break;
			}
			case 0x17: // Instruction BEZ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if (baseRegister == 0) {
					m_ActiveProcess.program_counter = address / 4;

					wprintf_s(L"BEZ (OpCode: 0x%X) content of reg%d: (0x%X) == 0; branch to address 0x%X\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, address);
				}
				else {
					wprintf_s(L"BEZ (OpCode: 0x%X) content of reg%d: (0x%X) != 0; continue to next instruction\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister);
				}

				break;
			}
			case 0x18: // Instruction BNZ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if (baseRegister != 0){
					m_ActiveProcess.program_counter = address/4; 
					wprintf_s(L"BNZ (OpCode: 0x%X) content of reg%d: (0x%X) != 0; branch to address 0x%X\n",
						decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, address);
				}
				else {
				wprintf_s(L"BNZ (OpCode: 0x%X) content of reg%d: (0x%X) == 0; continue to next instruction\n",
					decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister);
				}
				break;
			}
			case 0x19: // Instruction BGZ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if (baseRegister > 0){
					m_ActiveProcess.program_counter = address/4;

				wprintf_s(L"BGZ (OpCode: 0x%X) content of reg%d: (0x%X) > 0; branch to address 0x%X\n",
					decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, address);
				}
				else {
				wprintf_s(L"BGZ (OpCode: 0x%X) content of reg%d: (0x%X) <= 0; continue to next instruction\n",
					decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister);
				}
				break;
			}
			case 0x1A: // Instruction BLZ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				int baseRegister = (int)m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if (baseRegister < 0){
					m_ActiveProcess.program_counter = address/4;

				wprintf_s(L"BLZ (OpCode: 0x%X) content of reg%d: (0x%X) < 0; branch to address 0x%X\n",
					decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister, address);
				}
				else {
				wprintf_s(L"BLZ (OpCode: 0x%X) content of reg%d: (0x%X) >= 0; continue to next instruction\n",
					decoding.PrefixOpcode.OpCode, baseRegisterIndex, baseRegister);
				}
				break;
			}
			};

		} while (1);
	
	HALT:
		count = 0;
		wprintf_s(L"Process Halted\n");


	}

	void DMA() {
		/*while ()
		{
			switch (type)
			{
			case 0: Read(ch, next(p_rec), buf[next_io]);
			case 1: Write(ch, next(p_rec), buf[next_io]);
			}
		next_io: = next_io + 4;
		}
		signal(ComputeOnly);
		*/
	}

	void ComputeOnly()
	{
		/*
		while ()
		{
		ir: = Fetch(memory[map(PC)]);// fetch instruction at RAM address – mapped PC
			OSDriver::decode(ir, oc, addrptr); // part of decoding of the instruction in instr reg (ir),
			//returning the opcode (oc) and a pointer to a list of
			//significant addresses in ‘ir’ – saved elsewhere
		PC: = PC + 1; // ready for next instruction, increase PC by 1 (word)
			execute(oc)
			{
			case 0: // corresponding code using addrptr of operands
			case 1: // corresponding code or send interrupt
				…
			}
		}
		*/
	}
};
//#endif
/*
main(){

} */