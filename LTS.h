/*
By Samuel Gardiner
*/

#include "STScheduler.h"
#include <queue>

using namespace std;
using namespace Loader;

namespace LTS{
	void LTScheduler() {
		PCB p;
		int pri = 100;
		for (int i = 0; i < 30; i++) {
			pri = m_Memory.PCB_arr[i].code_size;
			for (int j = i; j < 30; j++) {
				if (m_Memory.PCB_arr[j].code_size < pri) {
					p = m_Memory.PCB_arr[j];
					m_Memory.PCB_arr[j] = m_Memory.PCB_arr[i];
					m_Memory.PCB_arr[i] = p;
					pri = p.code_size;
				}
			}
		}
		for (int i = 0; i < 30; i++) {
			m_Memory.readyQueue.push(m_Memory.PCB_arr[i]);
		}
	}
}