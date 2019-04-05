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

	Loader::Loader();
	LTS::LTScheduler();
	STS::STScheduling();

	/*for (int i = 0; i < 30; i++) {
		g_Cpu.Execute(m_Memory.PCB_arr[i]);
	}*/
	
	cout << m_Memory[220] << endl;

	g_Cpu.Execute(m_Memory.PCB_arr[0]);

	cout << m_Memory[220] << endl;
	return 0;
}

