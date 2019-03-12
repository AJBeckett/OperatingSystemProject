#include <Memory.h>
#include <queue>

using namespace std;

void LTScheduler(){
    PCB p;
    int pri = 100;
    for(int i = 0; i < 30; i++){
        for(int j = i; j < 30; j++){
            if(PCB_arr[j].priority < pri){
                p = PCB_arr[j];
                PCB_arr[j] = PCB_arr[i];
                PCB_arr[i] = p;
                pri = p.priority;
            }
        }
    }
    for(int i = 0; i < 30; i++){
        rq.push(PCB_arr[i]);
    }
}