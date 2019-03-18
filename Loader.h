/*
By Samuel Gardiner and Johnny Santana
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <Windows.h>

#include "Memory.h"
#include "PCB.h"

using namespace std;

void Loader() {
	stringstream str;
	string word;
	DWORD instruction;
	//currentValue is the current word being read
	//currentProcess is the index of the current process in the pcb array
	//programCount is the index on disk where the current process instructions begin.
	//memoryCount is the index in disk that is currently being pointed to.
	int currentValue, currentProcess = 0, programCount = 0, memoryCount = 0;
	PCB p1;
	ifstream reader;
	reader.open("C:\\Users\\Sam\\OSProgramFile.txt");
	if (!reader) {
		cout << "File failed to open." << endl;
		exit(1);
	}
	while (reader >> word) {
		if (word == "JOB") {
			programCount = memoryCount;
			p1.program_counter = programCount;
			reader >> hex >> currentValue;
			p1.process_id = currentValue;
			reader >> hex >> currentValue;
			p1.code_size = currentValue;
			reader >> hex >> currentValue;
			p1.priority = currentValue;
		}
		else if (word == "Data") {
			reader >> hex >> currentValue;
			p1.s.in_buf = currentValue;
			reader >> hex >> currentValue;
			p1.s.out_buf = currentValue;
			reader >> hex >> currentValue;
			p1.s.temp_buf = currentValue;
		}
		else if (word == "END" || word == "//END") {
			PCB_arr[currentProcess] = p1;
			currentProcess++;
		}
		else if (word == "//") {

		}
		else {
			reader >> instruction;
		}
	}
	for (int i = 0; i < 30; i++) {
		cout << data[i] << endl;
	}
	reader.close();
}