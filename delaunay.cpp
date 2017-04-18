#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>

#include "edge.h"
#include "quadedge.h"

using namespace std;
using std::ifstream;

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = " ";

static void parse(string filename){
	// create a file-reading object
	ifstream fin;
	fin.open(filename.c_str()); // open a file
	if (!fin.good()) {
		return; // exit if file not found
	}
	// read each line of the file
	while (!fin.eof()){
		// read an entire line into memory
		char buf[MAX_CHARS_PER_LINE];
		fin.getline(buf, MAX_CHARS_PER_LINE);

		// parse the line into blank-delimited tokens
		int n = 0; // a for-loop index

		// array to store memory addresses of the tokens in buf
		const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0

		// parse the line
		token[0] = strtok(buf, DELIMITER); // first token
		if (token[0]){ // zero if line is blank
			for (n = 1; n < MAX_TOKENS_PER_LINE; n++) {
				token[n] = strtok(0, DELIMITER); // subsequent tokens
				if (!token[n]) {
					break; // no more tokens
				}		  
			}
		}

		// process (print) the tokens
		for (int i = 0; i < n; i++){ // n = #of tokens
			cout << "Token[" << i << "] = " << token[i] << endl;
		}
		cout << endl;	
	}
}

static Edge* delaunay(vector<double*> S){
	if(S.size() == 2){
		Edge *a = Edge::makeEdge();
		a->setOrigin(S[0]);
		a->setDest(S[1]);
		return a; 
	}else if(S.size() == 3){
		return NULL;
	}else{ // S.size() >= 4
		return NULL;
	}
}

int main (int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Useage: ./program /data/filename.node" << endl;
		return 1;	
	}
	cout << "Reading from file " << argv[1] << "\n";
	//vector<double*> pts = parse(argv[1]);
	//for (vector<char>::const_iterator i = pts.begin(); i != pts.end(); ++i)
    //	cout << *i << ' ';

	QuadEdge* qedge = new QuadEdge();
	for(int i = 0; i < 4; i+=1){
		
		Edge* e = qedge->e;
		cout << "edge idx: " << e[i].idx << "\n";
		//cout << "edge.invrot idx: " << e.invrot()->idx << "\n";
	}
	return 0;
}
