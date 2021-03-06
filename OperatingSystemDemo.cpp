/*
By Derick Ehouman and Alissa Beckett

Modules were implemented by Johnny Santana, Samuel Gardiner, Derick Ehouman, and Alissa Beckett.

*/

#include <wchar.h>
#include <iomanip>
#include <thread>

#include "CPU.h"

using namespace std;

CPU g_Cpu1, g_Cpu2, g_Cpu3, g_Cpu4;

ofstream coreDump;


void DriverExecute1(PCB process)
{
	g_Cpu1.Execute(process, coreDump);
}

void DriverExecute2(PCB process)
{
	g_Cpu2.Execute(process, coreDump);
}

void DriverExecute3(PCB process)
{
	g_Cpu3.Execute(process, coreDump);
}

void DriverExecute4(PCB process)
{
	g_Cpu4.Execute(process, coreDump);
}


int wmain(int argc, wchar_t* argv[])
{
	Loader::Loader();
	LTS::LTScheduler();
	STS::STScheduling();

	cout << m_Memory.disk[904 / 4].pageArr[904 % 4] << endl;

	wprintf(L"------------Page Table---------- \n");
	for (int i = 0; i < 224; i++) 
	{
		
		cout << m_Memory.pageTable[i] << endl;
	}

	coreDump.open("CoreDump.txt");
	
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
		g_Cpu1.Execute(curRam, coreDump);

		m_Memory.readyQueue.pop();
	}
	coreDump.close();

	while (!m_Memory.readyQueue.empty()) {

		thread cpuThread1(DriverExecute1, m_Memory.readyQueue.front());
		thread cpuThread2(DriverExecute2, m_Memory.readyQueue.front());
		thread cpuThread3(DriverExecute3, m_Memory.readyQueue.front());
		thread cpuThread4(DriverExecute4, m_Memory.readyQueue.front());

		cpuThread1.join();
		cpuThread2.join();
		cpuThread3.join();
		cpuThread4.join();

	}

	return 0;
}