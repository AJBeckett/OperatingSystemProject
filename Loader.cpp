#include <PCB.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace std;

int main(){
    string line;
    int x;
    ifstream reader;
    reader.open("C:\\Users\\Sam\\OSProgramFile.txt");
    if(!reader){
        cout << "File failed to open." << endl;
        exit(1);
    }
     while(getline(reader, line)){
        cout << line << endl;
        if (line.length() > 0 && line.compare(3, 3, "JOB") == 0){
            reader.close();
        }
        if(line == "// END"){
            reader.close();
        }
        //extract attributes to PCB
        //insert hex or instruction into RAM
    }
    reader.close();
}