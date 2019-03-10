#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <Memory.h>

using namespace std;

void Loader(){
    stringstream str;
    string word;
    bool job = false;
    int x;
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
        if(word == "END"){
            count = 0;
        }
        if(word == "//"){
            ;
        }
        if(word == "JOB"){
            job = true;
            reader >> hex >> x;
            p1.process_id = x;
            reader >> hex >> x;
            p1.code_size = x;
            reader >> hex >> x;
            p1.priority = x;
        }
        if(word == "DATA"){
            job = false;
            reader >> hex >> x;
            p1.s.in_buf = new int[x];
            reader >> hex >> x;
            p1.s.out_buf = new int[x];
            reader >> hex >> x;
            p1.s.temp_buf = new int [x];
        }
        if(job == true){
            reader >> hex >> disk[z];
            z = z + p1.code_size;
        }
        if(!job){
            reader >> hex >> p1.s.in_buf[count];
            count++;
        }
        
        PCB_arr[y] = p1;
        y++;
    }
    reader.close();
}