#include <iostream>
#include <Memory.h>

// scheduling 
using namespace std; 


// Function to sort the Process acc. to priority 
bool comparison(PCB a, PCB b) 
{ 
    return (a.priority > b.priority); 
} 
  
// Function to find the waiting time for all 
// processes 
void findWaitingTime(PCB proc[], int n, 
                     int wt[]) 
{ 
    // waiting time for first process is 0 
    wt[0] = 0; 
  
    // calculating waiting time 
    for (int  i = 1; i < n ; i++ ) 
        wt[i] =  proc[i-1].burst_time + wt[i-1] ; 
} 
  
  
//Function to calculate average time 
void findavgTime(PCB proc[], int n) 
{ 
    int wt[n], tat[n], total_wt = 0, total_tat = 0; 
  
    //Function to find waiting time of all processes 
    findWaitingTime(proc, n, wt); 
  
  
    //Display processes along with all details 
    cout << "\nProcesses  "<< " Burst time  "
         << " Waiting time\n  "; 
  
    // Calculate total waiting time 
    for (int  i=0; i<n; i++) 
    { 
        total_wt = total_wt + wt[i]; 
        cout << "   " << proc[i].process_id << "\t\t"
             << proc[i].burst_time << "\t    " << wt[i] 
             <<endl; 
    } 
  
    cout << "\nAverage waiting time = "
         << (float)total_wt / (float)n; 
   
} 
  
void STScheduling(PCB proc[], int n) 
{   
  Memory a;
  
       for(int i = 0; i < 30; i++)
        {
          rq.pop(proc[i]); 
        }
  
    for (int  i = 0 ; i <  n; i++) 
        a.ram = proc[i].process_id; 
        
  
    findavgTime(proc, n);
    
} 
  
