/*
By Derick Ehouman and Alissa Beckett

Modules were implemented by Johnny Santana, Samuel Gardiner, Derick Ehouman, and Alissa Beckett.

*/
#include "stdafx.h"
#include <wchar.h>
#include <iomanip>
#include "CPU.h"

using namespace std;

CPU g_Cpu;

int wmain(int argc, wchar_t* argv[])
{
	Loader::Loader();
	LTS::LTScheduler();
	STS::STScheduling();

	/*cout << m_Memory.disk[904 / 4].pageArr[904 % 4] << endl;

	for (int i = 0; i < 224; i++) {
		cout << m_Memory.pageTable[i] << endl;
	}*/

	ofstream coreDump;
	coreDump.open("C:\\Users\\Derick\\source\\repos\\OperatingSystemProject-Phase-2\\CoreDump.txt");
	
	while (!m_Memory.readyQueue.empty()) 
	{
		PCB curRam = m_Memory.readyQueue.front();
		int jobId;
		stringstream stream;
		stream << curRam.process_id;
		stream >> hex >> jobId;
		coreDump << "JOB " << jobId << " – BINARY SECTION\n";

		for (int i = 0; i < curRam.code_size; i++) {
			coreDump << setfill('0') << setw(8) << hex << uppercase << m_Memory.disk[(i + curRam.BaseAddress) / 4].pageArr[(i + curRam.BaseAddress) % 4] << "\n";
		}

		coreDump << "JOB " << jobId << " – INPUT SECTION\n";

		for (int i = 0; i < curRam.code_size; i++) {
			coreDump << setfill('0') << setw(8) << hex << uppercase << m_Memory.disk[(i + curRam.BaseAddress + curRam.code_size) / 4].pageArr[(i + curRam.BaseAddress + curRam.code_size) % 4] << "\n";
		}
		
		coreDump << "JOB " << jobId << " – OUTPUT SECTION\n";
		g_Cpu.Execute(curRam, coreDump);

		m_Memory.readyQueue.pop();
	}
	coreDump.close();
	
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

