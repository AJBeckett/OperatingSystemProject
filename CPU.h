/*
By Johnny Santana Samuel Gardiner, Derick Ehouman, and Alissa Beckett
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <Memory.h>
#include "PCB.h" //Johnny: Figured we need pcb.h for cpu
#include "Memory.h"

class CPU
{
private:
	state	m_State;	// Active context
	Memory	m_Memory;
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

	CPU() : m_State(), m_Memory()
	{

	}


	void AssignProcess(PCB& newProcess)
	{
		m_ActiveProcess = newProcess;
	}

	void SwitchProcess(PCB& newProcess)
	{
		m_ActiveProcess = newProcess;
	}

	void SaveContext(state& savedState)
	{
		savedState = m_State;
	}

	DWORD ProgramCounter()
	{
		return m_ActiveProcess.program_counter;
	}

	void IncrementProgramCounter()
	{
		m_ActiveProcess.program_counter++;
	}


	DWORD Fetch() {
		// Each instruction is 32 bits long
		DWORD instruction = m_Memory[ProgramCounter() * sizeof(DWORD)];
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
	// Sample of CPU.h by Johnny Santana Samuel Gardiner, Derick Ehouman, and Alissa Beckett
	void Execute() {
		do
		{
			DWORD instruction = Fetch();

			TDecoding decoding;
			decoding.Instruction = instruction;

			IncrementProgramCounter();

			switch (decoding.PrefixOpcode.OpCode)
			{
			case 0x00: // Instruction is RD, Type is I/O
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x03);
				DWORD address = decoding.Prefix11.Address + m_State.m_Registers[decoding.Prefix11.RegisterIndex1];
				DWORD data = m_Memory[address];
				SetRegister(decoding.Prefix11.RegisterIndex0, data);

				wprintf_s(L"RD (OpCode: 0x%X) Reg%d <-- 0x%x (Memory[0x%X])\n",
					decoding.PrefixOpcode.OpCode, decoding.Prefix11.RegisterIndex0, data, address);
				break;
			}
			case 0x01: // Instruction is WR, Type is I/O
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x03);
				DWORD address = decoding.Prefix11.Address;
				DWORD accumulatorValue = m_State.m_Registers.Accumulator;
				m_Memory[address] = accumulatorValue;

				wprintf_s(L"WR (OpCode: 0x%X) Memory[0x%X] = 0x%X from Accumulator\n",
					decoding.PrefixOpcode.OpCode, address, accumulatorValue);
	
				break;
			}
			case 0x02: // Instruction is ST, Type is I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				//DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);
				m_Memory[destinationRegister] = baseRegister;

				wprintf_s(L"ST (OpCode: 0x%X) Memory[0x%X] = 0x%X (from Reg%d)\n",
					decoding.PrefixOpcode.OpCode, destinationRegister, baseRegister, baseRegisterIndex);

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

				wprintf_s(L"LW (OpCode: 0x%X) Reg%d <-- 0x%X from memory[0x%X]\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, memoryValue, effectiveAddress);
				break;
			}
			case 0x04: // Instruction MOV, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;

				DWORD sourceRegister = m_State.m_Registers.ReadRegister(sourceRegisterIndex1);
				SetRegister(destinationRegisterIndex, sourceRegister);

				wprintf_s(L"MOV (OpCode: 0x%X) Reg%d <-- 0x%X from source register %d\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, sourceRegister, sourceRegisterIndex1);


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

				wprintf_s(L"ADD (OpCode: 0x%X) Reg%d <-- 0x%X (Reg%d + Reg%d)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, answer, decoding.Prefix00.SourceRegisterIndex1, decoding.Prefix00.SourceRegisterIndex0);

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

				wprintf_s(L"SUB (OpCode: 0x%X) Reg%d <-- 0x%X (Reg%d - Reg%d)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, answer, decoding.Prefix00.SourceRegisterIndex0, decoding.Prefix00.SourceRegisterIndex1);

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

				wprintf_s(L"MUL (OpCode: 0x%X) Reg%d <-- 0x%X (Reg%d * Reg%d)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, answer, decoding.Prefix00.SourceRegisterIndex0, decoding.Prefix00.SourceRegisterIndex1);

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

				wprintf_s(L"DIV (OpCode: 0x%X) Reg%d <-- 0x%X (Reg%d / Reg%d)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, answer, decoding.Prefix00.SourceRegisterIndex0, decoding.Prefix00.SourceRegisterIndex1);
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

				wprintf_s(L"AND (OpCode: 0x%X) Reg%d <-- 0x%X (Reg%d && Reg%d)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, answer, decoding.Prefix00.SourceRegisterIndex0, decoding.Prefix00.SourceRegisterIndex1);
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

				wprintf_s(L"OR (OpCode: 0x%X) Reg%d <-- 0x%X (Reg%d || Reg%d)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, answer, decoding.Prefix00.SourceRegisterIndex0, decoding.Prefix00.SourceRegisterIndex1);
				break;
			}
			case 0x0B: // Instruction MOVI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				SetRegister(destinationRegisterIndex, address);

				wprintf_s(L"MOVI (OpCode: 0x%X) Reg%d <-- 0x%X\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, address);
				break;
			}
			case 0x0C: // Instruction ADDI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);

				DWORD addValue = address + destinationRegister;

				wprintf_s(L"ADDI (OpCode: 0x%X) Reg%d <-- 0x%X (Address:0x%X + 0x%X)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, addValue, address, destinationRegister);

				SetRegister(destinationRegisterIndex, addValue);
				break;
			}
			case 0x0D: // Instruction MULI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);
				DWORD multiValue = address * destinationRegister;

				wprintf_s(L"MULI (OpCode: 0x%X) Reg%d <-- 0x%X (Address:0x%X + 0x%X)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, multiValue, address, destinationRegister);

				SetRegister(destinationRegisterIndex, multiValue);

				break;
			}
			case 0x0E: // Instruction DIVI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD destinationRegister = m_State.m_Registers.ReadRegister(destinationRegisterIndex);
				DWORD divValue = address / destinationRegisterIndex;

				wprintf_s(L"DIVI (OpCode: 0x%X) Reg%d <-- 0x%X (Address:0x%X + 0x%X)\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, divValue, address, destinationRegister);

				SetRegister(destinationRegisterIndex, divValue);

				break;
			}
			case 0x0F: // Instruction LDI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				
				SetRegister(destinationRegisterIndex, address);

				wprintf_s(L"LDI (OpCode: 0x%X) Reg%d <-- 0x%X\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, address);
				break;
			}
			case 0x10: // Instruction SLT, type R
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
				DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
				DWORD sourceRegister1 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex1];
				DWORD sourceRegister0 = m_State.m_Registers[decoding.Prefix00.SourceRegisterIndex0];

				DWORD value = 0;
				if(sourceRegister0 < sourceRegister1)
					value = 1;

				SetRegister(destinationRegisterIndex, value);

				wprintf_s(L"SLT (OpCode: 0x%X) Reg%d <-- 0x%X\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, value);


				break;
			}
			case 0x11: // Instruction SLTI, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				DWORD value = 0;
				if(baseRegister < address)
					value = 1;

				SetRegister(destinationRegisterIndex, value);

				wprintf_s(L"SLTI (OpCode: 0x%X) Reg%d <-- 0x%X\n",
					decoding.PrefixOpcode.OpCode, destinationRegisterIndex, value);

				break;
			}
			case 0x12: // Instruction HLT, type J
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x02);
				wprintf_s(L"HLT (OpCode: 0x%X)\n",
					decoding.PrefixOpcode.OpCode);
				goto HALT;

				break;
			}
			case 0x13: // Instruction NOP, no type
			{
				//no operation
				wprintf_s(L"NOP (OpCode: 0x%X)\n",
					decoding.PrefixOpcode.OpCode);
				break;
			}
			case 0x14: // Instruction JMP, type J
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x02);
				DWORD address = decoding.Prefix10.Address;
				m_ActiveProcess.program_counter = address;
				wprintf_s(L"JMP (OpCode: 0x%X)  program_counter <== 0x%X\n",
					decoding.PrefixOpcode.OpCode, address);
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

				if(baseRegister == destinationRegister)
				{
					wprintf_s(L"BEQ (OpCode: 0x%X)  program_counter <== 0x%X\n",
						decoding.PrefixOpcode.OpCode, address);
					m_ActiveProcess.program_counter = address / 4;
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

				if (baseRegister != destinationRegister)
				{
					wprintf_s(L"BNE (OpCode: 0x%X)  program_counter <== 0x%X\n",
						decoding.PrefixOpcode.OpCode, address);

					m_ActiveProcess.program_counter = address/4;
				}
				break;
			}
			case 0x17: // Instruction BEZ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if(baseRegister == 0)
				{
					wprintf_s(L"BEZ (OpCode: 0x%X)  program_counter <== 0x%X\n",
						decoding.PrefixOpcode.OpCode, address);
					m_ActiveProcess.program_counter = address / 4;
				}
				
				break;
			}
			case 0x18: // Instruction BNZ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if(baseRegister != 0)
				{
					wprintf_s(L"BNZ (OpCode: 0x%X)  program_counter <== 0x%X\n",
						decoding.PrefixOpcode.OpCode, address);
					m_ActiveProcess.program_counter = address / 4;
				}
				
				break;
			}
			case 0x19: // Instruction BGZ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				DWORD baseRegister = m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if(baseRegister > 0)
				{
					wprintf_s(L"BGZ (OpCode: 0x%X)  program_counter <== 0x%X\n",
						decoding.PrefixOpcode.OpCode, address);
					m_ActiveProcess.program_counter = address / 4;
				}
				
				break;
			}
			case 0x1A: // Instruction BLZ, type I
			{
				_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
				DWORD address = decoding.Prefix01.Address;
				DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
				int baseRegister = (int)m_State.m_Registers.ReadRegister(baseRegisterIndex);

				if(baseRegister < 0)
				{
					wprintf_s(L"BLZ (OpCode: 0x%X)  program_counter <== 0x%X\n",
						decoding.PrefixOpcode.OpCode, address);
					m_ActiveProcess.program_counter = address / 4;
				}
				
			}
			};

		} while (1);
	HALT:
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