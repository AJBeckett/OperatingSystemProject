/*
By Chris Resby and Samuel Gardiner

*/


#include <iostream>
#include "Loader.h"

using namespace std;
using namespace Loader;

namespace STS {
	void STScheduling()
	{
		int count = 0;
		PCB current;
		while (!m_Memory.readyQueue.empty()) {
			PCB current;
			current = m_Memory.readyQueue.front();
			//cout << current.program_counter << endl;
			m_Memory.readyQueue.pop();
			// WHY NOT COPY ALL THE DATA BUFFERS?
			for (int i = current.BaseAddress; i < current.BaseAddress + 16; i = i+4){
				m_Memory[count*4] = m_Memory.disk[i/4];
				m_Memory[count * 4].valid = true;
				m_Memory.pageTable[i / 4] = count;
				count++;
			}
		}
		for (int i = 0; i < 30; i++) {
			m_Memory.readyQueue.push(m_Memory.PCB_arr[i]);
		}
	}
}