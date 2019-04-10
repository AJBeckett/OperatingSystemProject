/*
By Chris Resby and Samuel Gardiner

*/


#include <iostream>
#include "Loader.h"

using namespace std;
using namespace Loader;

namespace STS {
	PCB arr[30];
	void STScheduling()
	{
		while (!m_Memory.readyQueue.empty()) {
			PCB current;
			int count = 0;
			current = m_Memory.readyQueue.front();
			//cout << current.program_counter << endl;
			arr[count] = current;
			count++;
			m_Memory.readyQueue.pop();
			// WHY NOT COPY ALL THE DATA BUFFERS?
			for (int i = current.program_counter; i < current.BaseAddress + current.code_size + current.s.in_buf; i++){ 
				m_Memory[i*4] = m_Memory.disk[i];
			}
		}
		for (int i = 0; i < 30; i++) {
			m_Memory.readyQueue.push(m_Memory.PCB_arr[i]);
		}
	}
}