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
		reader.open("ProgramFile.txt");
		if (!reader) {
			cout << "File failed to open." << endl;
			exit(1);
		}
		// sample of Loader.h By Samuel Gardiner and Johnny Santana
		while (reader >> word) {
			if (word == "JOB") {
				programCount = memoryCount-1;
				p1.program_counter = programCount;
				if (p1.program_counter == 1) {
					p1.program_counter = 0;
				}
				reader >> hex >> currentValue;
				p1.process_id = currentValue;
				reader >> hex >> currentValue;
				p1.code_size = currentValue;
				reader >> hex >> currentValue;
				p1.priority = currentValue;
				for (int i = 0; i < p1.code_size; i++) {
					reader >> instruction;
					m_Memory.disk[i+programCount] = instruction;
					memoryCount++;
				}
			}
			else if (word == "Data") {
				programCount = memoryCount-1;
				reader >> hex >> currentValue;
				p1.s.in_buf = currentValue;
				reader >> hex >> currentValue;
				p1.s.out_buf = currentValue;
				reader >> hex >> currentValue;
				p1.s.temp_buf = currentValue;
				for (int i = 0; i < 44; i++) {
					reader >> instruction;
					m_Memory.disk[i + programCount] = instruction;
					memoryCount++;
				}
			}
			else if (word == "END" || word == "//END") {
				m_Memory.PCB_arr[currentProcess] = p1;
				currentProcess++;
			}
			else {
				
			}
		}
		reader.close();
	}
}