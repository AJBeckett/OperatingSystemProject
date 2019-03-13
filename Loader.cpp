#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <Memory.h>

using namespace std;

int main(){
    stringstream str;
    string word;
    bool job = false;
    int x;
    long h;
    int y = 0;
    int z = 0;
    int count = 0;
    PCB p1;
    ifstream reader;
    reader.open("C:\\Users\\Sam\\OSProgramFile.txt");
    if(!reader){
        cout << "File failed to open." << endl;
        exit(1);
    }
    while(reader >> word){
        if(word == "JOB"){
            job = true;
            z = count;
            p1.program_counter = z;
            reader >> hex >> x;
            p1.process_id = x;
            reader >> hex >> x;
            p1.code_size = x;
            reader >> hex >> x;
            p1.priority = x;
        }
        else if(word == "Data"){
            job = false;
            reader >> hex >> x;
            p1.s.in_buf = new int[x];
            reader >> hex >> x;
            p1.s.out_buf = new int[x];
            reader >> hex >> x;
            p1.s.temp_buf = new int[x];
        }
        else if(word == "END" || word == "//END"){
            PCB_arr[y] = p1;
            y++;
        }
        else if(word == "//"){
            
        }
        else{
            if(job == true){
                
            }
            else{
                 
            }
        }
    }
    for(int i = 0; i < 30; i++){
        cout << "This JOB's code_size is : " << PCB_arr[i].code_size << endl;
    }
    reader.close();
    return 0;
}