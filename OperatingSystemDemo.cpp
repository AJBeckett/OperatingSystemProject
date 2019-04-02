/*
By Derick Ehouman and Alissa Beckett

Modules were implemented by Johnny Santana, Samuel Gardiner, Derick Ehouman, and Alissa Beckett.

*/



#include "targetver.h"
#include <wchar.h>

#include "CPU.h"

using namespace std;

CPU g_Cpu;


int wmain(int argc, wchar_t *argv[])
{
	// I haveto initialize my machine, READ the data file, and then go to work

	// our process has fake data in Debug
	PCB process;
	
	Loader::Loader();
	LTS::LTScheduler();
	STS::STScheduling();

	g_Cpu.AssignProcess(process);

	cout << m_Memory.ram[92];

	g_Cpu.Execute();

	return 0;
}

