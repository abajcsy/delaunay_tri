// QuadEdge class
#include <iostream>
#include "edge.h"

using namespace std;

class QuadEdge {
	public:
		Edge e[4];			// four edge records containing data and next fields

		QuadEdge();
};

//************* QuadEdge functions **************//

// Creates new quadedge structure
QuadEdge::QuadEdge(){
	// assign indicies for each edge in quadedge
	for(int i=0; i<4; i=i+1){
		this->e[i].idx = i;	
	}

	// set up next CCW edge pointers (around origin) for all edges
	this->e[0].next = &(e[0]);
	this->e[1].next = &(e[3]);
	this->e[2].next = &(e[2]);
	this->e[3].next = &(e[1]);
	
	// set data for all edges
	
}

int main () {
  QuadEdge* q = new QuadEdge();
	for(int i = 0; i < 4; i+=1){
		Edge e = q->e[i];
		cout << "edge idx: " << e.idx << "\n";
		cout << "edge.invrot idx: " << e.invrot()->idx << "\n";
	}
  return 0;
}
