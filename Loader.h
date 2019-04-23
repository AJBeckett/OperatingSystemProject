/*
By Samuel Gardiner and Johnny Santana
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include "Memory.h"

using namespace std;

namespace Loader {
	Memory m_Memory;
	void Loader() {
		for (int i = 0; i < 1024; i++) {
			m_Memory.pageTable[i] = -1;
		}
		stringstream str;
		string word;
		DWORD instruction;
		//currentValue is the current word being read
		//currentProcess is the index of the current process in the pcb array
		//programCount is the index on disk where the current process instructions begin.
		//memoryCount is the index in disk that is currently being pointed to.

		int currentValue, currentProcess = 0, programCount = 0, memoryCount = 1;
		PCB p1;
		ifstream reader;
		reader.open("C:\\Users\\Derick\\source\\repos\\OperatingSystemProject-Phase-2\\ProgramFile.txt");
		//reader.open("Program4.txt");
		if (!reader) {
			cout << "File failed to open." << endl;
			exit(1);
		}

		DWORD baseAddress = 0;
		DWORD dataSize = 0;

		// sample of Loader.h By Samuel Gardiner and Johnny Santana
		while (reader >> word) 
		{
			if (word == "JOB") 
			{
				p1.program_counter = 0; // ALWAYS ZERO

				reader >> hex >> currentValue;
				p1.process_id = currentValue;
				reader >> hex >> currentValue;
				p1.code_size = currentValue;
				reader >> hex >> currentValue;
				p1.priority = currentValue;
				for (int i = 0; i < p1.code_size; i++) {
					reader >> instruction;
					m_Memory.disk[(i + baseAddress) / 4].pageArr[(i+baseAddress)%4] = instruction;
					memoryCount++;
				}
			}
			else if (word == "Data") 
			{
				programCount = memoryCount-1;
				reader >> hex >> currentValue;
				p1.s.in_buf = currentValue;
				reader >> hex >> currentValue;
				p1.s.out_buf = currentValue;
				reader >> hex >> currentValue;
				p1.s.temp_buf = currentValue;
				// Data size is in_buf + out_buf + temp_buf
				// 14 + C + C (20 + 12 + 12)
				dataSize = p1.s.in_buf + p1.s.out_buf + p1.s.temp_buf;
				for (int i = 0; i < dataSize; i++) {
					reader >> instruction;
					m_Memory.disk[(i + baseAddress + p1.code_size) /4].pageArr[(i+baseAddress+p1.code_size)%4] = instruction;
					memoryCount++;
				}
			}
			else if (word == "END" || word == "//END") 
			{
				p1.BaseAddress = baseAddress;
				m_Memory.PCB_arr[currentProcess] = p1;
				currentProcess++;
				baseAddress = baseAddress + p1.code_size + dataSize;
			}

			else {
				
			}
		}
		reader.close();
	}
}