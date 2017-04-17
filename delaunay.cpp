// QuadEdge class
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "edge.h"

using namespace std;

/* Reads .node file and returns sorted list of points in file */
static double* readNode(string filename){
	// opens given .node file and constructs quadedge structure given the info
	string line;
	ifstream myfile(filename.c_str());
	if (myfile.is_open()){
		bool firstLine = true;
		while ( getline(myfile,line) ){
			if(firstLine){
			// First line: 
			// <# of vertices> <dimension (must be 2)> <# of attributes> <# of boundary markers (0 or 1)>
				firstLine = false; 
			}else{
			// Remaining lines: 
			// <vertex #> <x> <y> [attributes] [boundary marker] 
				istringstream iss(line); 
				int n;
				while (iss >> n) {
					cout << n << ", ";
				}
				cout << endl;
			} 
    	}
    	myfile.close();
	}else{
		cout << "Unable to open file" << filename << "\n";
	}
}

int main (int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Useage: ./program /data/filename.node" << endl;
		return 1;	
	}
	cout << "Reading from file " << argv[1] << "\n";
	double *pts = readNode(argv[1])
	QuadEdge* q = new QuadEdge();
	for(int i = 0; i < 4; i+=1){
		Edge e = q->e[i];
		cout << "edge idx: " << e.idx << "\n";
		//cout << "edge.invrot idx: " << e.invrot()->idx << "\n";
	}
	return 0;
}
