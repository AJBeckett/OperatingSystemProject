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
		while (!m_Memory.readyQueue.empty()) {
			PCB current;
			current = m_Memory.readyQueue.front();
			m_Memory.readyQueue.pop();
			for (int i = current.program_counter; i < current.program_counter + current.code_size + current.s.in_buf; i++) {
				m_Memory.ram[i] = m_Memory.disk[i];
			}
		}
	}
}