#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <cstdlib>

#include "edge.h"
#include "quadedge.h"

using namespace std;
using std::ifstream;

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = " ";

/* Parses .node file into a list of points. 
 */
static vector< vector<double> > parse(string filename){
	vector< vector<double> > data;
	ifstream f(filename.c_str());

	string line;

	int firstLine = 1;

	while(getline(f,line)) {
		if(!firstLine){
			vector<double> line_data;
			istringstream iss(line);
			double value;
			int idx = 0;
			while(iss >> value){
				if (idx == 1 || idx == 2){
					line_data.push_back(value);
				}
				idx +=1;
			}
			data.push_back(line_data);
		}else{
			firstLine = 0;
		}
	}

	return data;
}

/* Constructs delaunay triangulation with the divide and conquer algorithm
 * Reference: Gubias and Stolfi.
 */
static vector<Edge*> delaunay(vector< vector<double> > S){
	vector<Edge*> ret;
	if(S.size() == 2){
		cout << "Case: |S| = 2" << endl;
		Edge *a = Edge::makeEdge();
		a->setOrigin(S[0]);
		a->setDest(S[1]);

		// setup return value [a, a.Sym]
		ret.push_back(a);
		ret.push_back(a->sym());
	}else if(S.size() == 3){
		cout << "Case: |S| = 3" << endl;

	}else{ // S.size() >= 4
		cout << "Case: |S| >= 4" << endl;
		
	}
	return ret; 
}

int main (int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Useage: ./program /data/filename.node" << endl;
		return 1;	
	}
	cout << "Reading points from file " << argv[1] << "\n";
	vector< vector<double> > points = parse(argv[1]);
	for(int i =0; i < points.size(); i++){
		cout << "(" << points[i][0] << ", " << points[i][1] << ")\n";
	}

	delaunay(points);

	/*QuadEdge* qedge = new QuadEdge();
	for(int i = 0; i < 4; i+=1){
		Edge* e = qedge->e;
		cout << "edge idx: " << e[i].idx << "\n";
		cout << "edge.invrot idx: " << e->sym()->idx << "\n";
	}*/
	return 0;
}
