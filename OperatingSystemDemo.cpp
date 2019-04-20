/*
By Derick Ehouman and Alissa Beckett

Modules were implemented by Johnny Santana, Samuel Gardiner, Derick Ehouman, and Alissa Beckett.

*/



#include "targetver.h"
#include "stdafx.h"
#include <wchar.h>

#include "CPU.h"

using namespace std;

CPU g_Cpu;



int wmain(int argc, wchar_t* argv[])
{
	Loader::Loader();
	LTS::LTScheduler();
	STS::STScheduling();

	cout << m_Memory.disk[904 / 4].pageArr[904 % 4] << endl;

	for (int i = 0; i < 224; i++) {
		cout << m_Memory.pageTable[i] << endl;
	}

	//g_Cpu.Execute(m_Memory.readyQueue.front());
	/*while (!m_Memory.waitQueue.empty()) {
		g_Cpu.Execute(m_Memory.waitQueue.front());
		m_Memory.waitQueue.pop();
	}*/
	/*while (!m_Memory.readyQueue.empty()) {
		g_Cpu.Execute(m_Memory.readyQueue.front());
		m_Memory.readyQueue.pop();
	}*/

	//g_Cpu.Execute(m_Memory.readyQueue.front());
	return 0;
}

