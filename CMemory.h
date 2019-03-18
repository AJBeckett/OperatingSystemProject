#pragma once

#include "OperatingSystemDemo.h"
#include <C:\Users\aliss\Downloads\OperatingSystemDemo\OperatingSystemDemo\PCB.h>
#include <C:\Users\aliss\Downloads\OperatingSystemDemo\OperatingSystemDemo\CPU.h>

#include <array>

namespace OperatingSystem
{

	struct TRegisters {
		// Registers are only 4 bits wide

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

		TRegisters() : Registers{ 0 }
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
			DWORD value = Registers[index] & 0xf;
			return value;
		}

		void SetRegister(DWORD index, DWORD value)
		{
			_ASSERT(index < 16); // 0 - 15 registers
			Registers[index] = value & 0xFFFF;
		}
	};

	// State of a Process
	struct TContext
	{
		DWORD		m_ProgramCounter;	// Instruction poitner for this process(thread)
		TRegisters	m_Registers;		// Registers of this process(thread)

		TContext() : m_ProgramCounter(0), m_Registers()
		{

		}

		TContext& operator =(const TContext& source)
		{
			if(this != &source)
			{
				m_ProgramCounter = source.m_ProgramCounter;
				m_Registers = source.m_Registers;
			}
			return *this;
		}
	};
	
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

	class CMemory
	{
	private:
		std::array<BYTE, 4096> m_PhysicalDisk = { 0 };  // disk is 1024 DWORDs or 4k
		std::array<DWORD, 2048> m_PhysicalRam = { 0 };  // RAM is 2048 DWORDS or 8k

	public:
		CMemory()
		{
#pragma region Initialization of Physical Ram for Debug mode Testing
#ifdef _DEBUG
			m_PhysicalRam =
			{
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
			};
#endif
#pragma endregion

		}

		// address comes in physical address
		//  This is literal address, but memory stored as DWORDS
		//  Thus, 0x5C works, but program counter must be multiplied by size of DWORD to get to right 
		DWORD Memory(DWORD address)
		{
			_ASSERT(address < 1024);

			// physical address converted to DWORD - can't read BYTES or WORDS from memory directly
			ULONG offset = address / 4;
			return m_PhysicalRam[offset];
		}

	};

	// Identifies a Process, supports its operation 
	class CProcessControlBlock
	{
	private:
		DWORD		m_Id;
		DWORD		m_SizeOfCode;		// in DWORDS
		DWORD		m_Priority;			//
		DWORD		m_BaseAddress = 0;	// Where this process(thread) is in physical memory
		TContext	m_ContextState;		// Record of environment that is saved on interrupt	 

	public:
		CProcessControlBlock()
		{
#ifdef _DEBUG
			// JOB 1 17 2
			m_Id = 1;
			m_SizeOfCode = 0x17;
			m_Priority = 2;
#endif
		}

		CProcessControlBlock(DWORD id, DWORD sizeOfCode, DWORD priority) : 
			m_ContextState(), 
			m_Id(id), 
			m_SizeOfCode(sizeOfCode),
			m_Priority(priority)
		{

		}

		DWORD BaseAddress()
		{
			return m_BaseAddress;
		}

		void SaveState(TContext& state)
		{
			m_ContextState = state;
		}

		TContext GetState()
		{
			return m_ContextState;
		}

		DWORD ProcessID()
		{
			return m_Id;
		}
		DWORD SizeOfCode()
		{
			return m_SizeOfCode;
		}
		DWORD Priority()
		{
			return m_Priority;
		}
	};

	class CCpu
	{
	private:
		TContext	m_Context;	// Active context
		CMemory		m_Memory;
		CProcessControlBlock m_ActiveProcess;

	public:

		CCpu()
		{

#ifdef _DEBUG
			m_Context.m_Registers.Registers[10] = 10;
			ASSERT(m_Context.m_Registers.Reg10 == m_Context.m_Registers.Registers[10]);
			m_Context.m_Registers.SetRegister(10, 0);
			ASSERT(m_Context.m_Registers.Reg10 == 0);
#endif
		}

		void AssignProcess(CProcessControlBlock& newProcess)
		{
			m_ActiveProcess = newProcess;
		}

		void SwitchProcess(CProcessControlBlock& newProcess)
		{
			// Save state into current process
			m_ActiveProcess.SaveState(m_Context);
			// Retrieve state of new process
			m_Context = newProcess.GetState();
			m_ActiveProcess = newProcess;
		}

		void SaveContext(TContext& savedState)
		{
			// Make a copy of the active context
			savedState = m_Context;
		}

		DWORD ProgramCounter()
		{
			return m_Context.m_ProgramCounter;
		}

		void IncrementProgramCounter()
		{
			m_Context.m_ProgramCounter++;
		}

		DWORD Fetch()
		{
			DWORD instruction = m_Memory.Memory(m_ActiveProcess.BaseAddress() + ProgramCounter()*sizeof(DWORD));
			return instruction;
		}

		// Given instruction, return opcode, registers in active context are updated.
		BYTE Decode(DWORD instruction, TDecoding& decoding)
		{
			decoding.Instruction = instruction;
			return decoding.PrefixOpcode.OpCode;
		}

		void SetRegister(DWORD registerIndex, DWORD value)
		{
			_ASSERT(registerIndex < 16);
			m_Context.m_Registers[registerIndex] = value;
		}


		void Execute()
		{
			do
			{
				DWORD instruction = Fetch();
				instruction = 0xC10000AC;

				OperatingSystem::TDecoding decoding;
				BYTE opCode = Decode(instruction, decoding);
				IncrementProgramCounter();

				switch (decoding.PrefixOpcode.OpCode)
				{
				case 0x00: // Instruction is RD, Type is I/O
				{
					// 0xC050005C;  11 - 000000 - 0101 - 0000 – 0x005C
					//			IO – RD – index of destination register (0x05) – ignored index – address of input buffer (0x5C)
					//      0x000A --> R5
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x03);
					DWORD registerData = m_Memory.Memory(decoding.Prefix11.Address);  // The code/data has to have already been read into memory
					SetRegister(decoding.Prefix11.RegisterIndex0, registerData);
					wprintf_s(L"RD (OpCode: 0x%X) 0x%X (0x%x) into register %d\n",
						decoding.PrefixOpcode.OpCode, decoding.Prefix11.Address, registerData, decoding.Prefix11.RegisterIndex0);
					break;
				}
				case 0x01: // Instruction is WR, Type is I/O
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x03);
					DWORD address = m_Memory.Memory(decoding.Prefix11.Address);
					DWORD accumulatorValue = m_Context.m_Registers.Accumulator;
					//m_Memory.WriteMemory(address, accumulatorValue);


					break;
				}
				case 0x02: // Instruction is ST, Type is I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;

					// m_Memory.WriteMemory(effectiveAddress, destinationRegister);
					wprintf_s(L"ST (OpCode: 0x%X) 0x%X (0x%x) into register %d\n",
						decoding.PrefixOpcode.OpCode, decoding.Prefix01.Address, decoding.Prefix01.DestinationRegisterIndex, decoding.Prefix01.BaseRegisterIndex);

					break;
				}
				case 0x03: //Instruction LW, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;

					DWORD memoryValue = m_Memory.Memory(effectiveAddress);
					m_Context.m_Registers.SetRegister(destinationRegisterIndex, memoryValue);
					break;
				}
				case 0x04: // Instruction MOV, type R
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
					DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
					DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;
					DWORD sourceRegisterIndex0 = decoding.Prefix00.SourceRegisterIndex0;

					SetRegister(destinationRegisterIndex, sourceRegisterIndex1);


					break;
				}
				case 0x05: // Instruction ADD, type R
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
					DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
					DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;
					DWORD sourceRegisterIndex0 = decoding.Prefix00.SourceRegisterIndex0;

					DWORD answer = sourceRegisterIndex0 + sourceRegisterIndex1;
					SetRegister(destinationRegisterIndex, answer);

					break;
				}
				case 0x06: // Instruction SUB, type R
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
					DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
					DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;
					DWORD sourceRegisterIndex0 = decoding.Prefix00.SourceRegisterIndex0;

					DWORD answer = sourceRegisterIndex1 - sourceRegisterIndex0;
					SetRegister(destinationRegisterIndex, answer);
					break;
				}
				case 0x07: // Instruction MUL, type R
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
					DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
					DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;
					DWORD sourceRegisterIndex0 = decoding.Prefix00.SourceRegisterIndex0;

					DWORD answer = sourceRegisterIndex1 * sourceRegisterIndex0;
					SetRegister(destinationRegisterIndex, answer);
					break;
				}
				case 0x08: // Instruction DIV, type R
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
					DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
					DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;
					DWORD sourceRegisterIndex0 = decoding.Prefix00.SourceRegisterIndex0;

					DWORD answer = sourceRegisterIndex1 / sourceRegisterIndex0;
					SetRegister(destinationRegisterIndex, answer);
					break;
				}
				case 0x09: // Instruction AND, type R
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
					DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
					DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;
					DWORD sourceRegisterIndex0 = decoding.Prefix00.SourceRegisterIndex0;
					//Skipped because we don't understand
					break;
				}
				case 0x0A: // Instruction OR, type R
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
					DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
					DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;
					DWORD sourceRegisterIndex0 = decoding.Prefix00.SourceRegisterIndex0;
					//also skipped because we dont understand
					break;
				}
				case 0x0B: // Instruction MOVI, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;

					SetRegister(destinationRegisterIndex, address);
					break;
				}
				case 0x0C: // Instruction ADDI, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;

					DWORD addValue = address + destinationRegisterIndex;

					SetRegister(destinationRegisterIndex, addValue);

					break;
				}
				case 0x0D: // Instruction MULI, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;

					DWORD multiValue = address * destinationRegisterIndex;

					SetRegister(destinationRegisterIndex, multiValue);


					break;
				}
				case 0x0E: // Instruction DIVI, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;
					DWORD divValue = address / destinationRegisterIndex;

					SetRegister(destinationRegisterIndex, divValue);

					break;
				}
				case 0x0F: // Instruction LDI, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;

					SetRegister(destinationRegisterIndex, address);

					break;
				}
				case 0x10: // Instruction SLT, type R
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x00);
					DWORD destinationRegisterIndex = decoding.Prefix00.DestinationRegisterIndex;
					DWORD sourceRegisterIndex1 = decoding.Prefix00.SourceRegisterIndex1;
					DWORD sourceRegisterIndex0 = decoding.Prefix00.SourceRegisterIndex0;

					if (sourceRegisterIndex0 < sourceRegisterIndex1)
						SetRegister(destinationRegisterIndex, 1);
					else
						SetRegister(destinationRegisterIndex, 0);

					break;
				}
				case 0x11: // Instruction SLTI, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;

					if (baseRegisterIndex < address)
						SetRegister(destinationRegisterIndex, 1);
					else
						SetRegister(destinationRegisterIndex, 0);

					break;
				}
				case 0x12: // Instruction HLT, type J
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x02);
					DWORD address = decoding.Prefix10.Address;


					break;
				}
				case 0x13: // Instruction NOP, no type
				{
					//no operation
					break;
				}
				case 0x14: // Instruction JMP, type J
				{
					//_ASSERT(decoding.PrefixOpcode.Prefix == 0x02);
					//DWORD address = decoding.Prefix10.Address;
					// none in programs

					break;
				}
				case 0x15: // Instruction BEQ, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;

					//if (baseRegisterIndex = destinationRegisterIndex)



					break;
				}
				case 0x16: // Instruction BNE ,type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;
					break;
				}
				case 0x17: // Instruction BEZ, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;
					break;
				}
				case 0x18: // Instruction BNZ, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;
					break;
				}
				case 0x19: // Instruction BGZ, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;
					break;
				}
				case 0x1A: // Instruction BLZ, type I
				{
					_ASSERT(decoding.PrefixOpcode.Prefix == 0x01);
					DWORD address = decoding.Prefix01.Address;
					DWORD baseRegisterIndex = decoding.Prefix01.BaseRegisterIndex;
					DWORD destinationRegisterIndex = decoding.Prefix01.DestinationRegisterIndex;
					DWORD destinationRegister = m_Context.m_Registers.ReadRegister(destinationRegisterIndex);
					DWORD baseRegister = m_Context.m_Registers.ReadRegister(baseRegisterIndex);
					DWORD effectiveAddress = address + baseRegister;
					break;
				}
				};

			} while (1);//(ProgramCounter() < m_ActiveProcess.SizeOfCode());  // Until we switch processes, or get to end of Code (program counter reaches Code length) and eliminate process
		}
	};


}