#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <cstdlib>
#include <algorithm>
#include <math.h> 

#include "vertex.h"
#include "edge.h"
#include "quadedge.h"
#include "predicates.h"

using namespace std;
using std::ifstream;

/* Lexicographic comparator for sorting.
 */
bool lex_compare(Vertex *v1, Vertex *v2) {
	if(v1->pt[0] == v2->pt[0]){
		return v1->pt[1] < v2->pt[1];
	}
	return v1->pt[0] < v2->pt[0];
}

/* Parses .node file into a list of points. 
 */
static vector<Vertex*> parse(string filename){
	vector<Vertex*> data;
	ifstream f(filename.c_str());

	string line;

	int firstLine = 1;
	while(getline(f,line)) {
		if(!firstLine){
			Vertex *v = new Vertex();
			//vector<double> newPt;
			istringstream iss(line);
			double value;
			int idx = 0;
			while(iss >> value){
				if (idx == 0){
					// set vertex ID number
					int num = (int)value;
					v->setNodeNum(num);
				}else if(idx == 1){
					// set x value of vertex
					v->setPtX(value);
				}else if(idx == 2){
					// set y value of vertex
					v->setPtY(value);
				}
				idx +=1;
			}
			data.push_back(v);
		}else{
			firstLine = 0;
		}
	}
	return data;
}

/* Writes .ele file from delaunay triangulation 
 * 
 *  First line: <# of triangles> <nodes per triangle> <# of attributes>
 *  Remaining lines: <triangle #> <node> <node> <node> ... [attributes] 
 */
static void writeEle(string filename, vector< vector<Vertex*> > triList){
	string delimiter = ".";
	string name = filename.substr(0, filename.find(delimiter));
	string newfilename = name + ".ele";
	cout << "new file name: " << newfilename << endl;

	ofstream myfile (newfilename.c_str());
	if (myfile.is_open()) {
		myfile << triList.size() << " 2 0\n";
		for(int i = 0; i < triList.size(); i++){
			myfile << (i+1) << " " << triList[i][0]->getNodeNum() << " " << triList[i][1]->getNodeNum() << " " << triList[i][3]->getNodeNum();
		}
		myfile.close();
	}
	else cout << "Unable to open file";
}

/* Prints delaunay triangulation to console. 
 */
static vector< vector<Vertex*> > printTriangles(vector<Edge*> triangles){
	vector< vector<Vertex*> > triList;
	cout << "------- PRINTING TRIANGULATION -------\n";
	Edge *e = triangles[1];
	cout << "Triangle 1: \n";
	cout << "-- V1: (" << e->getOrigin()->getPt()[0] << ", " << e->getOrigin()->getPt()[1] << ")\n";
	Edge *curr_e = e->lnext();
	int i = 2;
	while(curr_e != NULL && curr_e != e){
		cout << "-- V" << curr_e->getOrigin()->getNodeNum() << ": (" << curr_e->getOrigin()->getPt()[0] << ", " << curr_e->getOrigin()->getPt()[1] << ")\n";
		curr_e = curr_e->lnext();
		i += 1;
	}	
	cout << "--------------------------------------\n";
	return triList;
}

/* Tests if point X is to the right of a given edge e.
 */
static double rightOf(Vertex *X, Edge *e){
	cout << "In rightof \n";
	double p1[2] = {X->pt[0], X->pt[1]};
	double p2[2] = {e->getDest()->pt[0], e->getDest()->pt[1]};
	double p3[2] = {e->getOrigin()->pt[0], e->getOrigin()->pt[1]};
	return orient2d(p1,p2,p3);
}

/* Tests if point X is to the left of a given edge e.
 */
static double leftOf(Vertex *X, Edge *e){
	cout << "In leftof \n";
	double p1[2] = {X->pt[0], X->pt[1]};
	cout << p1[0] << ", " << p1[1] << endl; 

	e->print();
	double p2[2] = {e->getDest()->pt[0], e->getDest()->pt[1]};
	cout << p2[0] << ", " << p2[1] << endl; 
	double p3[2] = {e->getOrigin()->pt[0], e->getOrigin()->pt[1]};
	cout << "blaaaa\n"; 
	cout << orient2d(p1,p3,p2) << endl;
	return orient2d(p1,p3,p2);
}

/* Tests whether the edge e is above basel
 */
static double valid(Edge *e, Edge *basel){
	return rightOf(e->getDest(), basel);
}

/* Constructs delaunay triangulation with the divide and conquer algorithm
 * Reference: Gubias and Stolfi.
 */
static vector<Edge*> delaunay(vector<Vertex*> S){
	vector<Edge*> ret;
	if(S.size() == 2){
		cout << "Case: |S| = 2" << endl;
		Edge *a = Edge::makeEdge();
		a->setOrigin(S[0]);
		a->setDest(S[1]);
		cout << "a: \n";
		a->print();
		// setup return value [a, a.Sym]
		ret.push_back(a);
		ret.push_back(a->sym());
	}else if(S.size() == 3){
		cout << "Case: |S| = 3" << endl;
		Edge *a = Edge::makeEdge();
		Edge *b = Edge::makeEdge();
		cout << "a: \n";
		a->print();
		cout << "b: \n";
		b->print();
		Edge::splice(a->sym(),b);
		cout << "bla" << endl;

		a->setOrigin(S[0]);
		a->setDest(S[1]);
		b->setOrigin(S[1]);
		b->setDest(S[2]);
		double p1[2] = {S[0]->pt[0], S[0]->pt[1]};
		double p2[2] = {S[1]->pt[0], S[1]->pt[1]};
		double p3[2] = {S[2]->pt[0], S[2]->pt[1]};

		cout << "NEW a:\n";
		a->print();
		cout << "NEW b:\n";
		b->print();

		if(orient2d(p1,p2,p3) > 0){
			cout << "Connecting b to a with c:\n";
			Edge *c = Edge::connect(b,a);
			c->print();
			cout << "Returning [a, b.Sym]\n";
			// setup return value [a, b.Sym]
			ret.push_back(a);
			ret.push_back(b->sym());
		}else if(orient2d(p1,p3,p2) > 0){
			cout << "Connecting b to a with c:\n";
			Edge *c = Edge::connect(b,a);
			c->print();
			c->sym()->print();
			cout << "Returning [c.Sym, c]\n";
			// setup return value [c.Sym, c]
			ret.push_back(c->sym());
			ret.push_back(c);
		}else{ // points are colinear
			cout << "Returning [a, b.Sym]\n";
			// setup return value [a, b.Sym]
			ret.push_back(a);
			ret.push_back(b->sym());
		}	
	}else{ // S.size() >= 4
		cout << "Case: |S| >= 4" << endl;
		// split points in half, L = left half of S, R = right half of S 
		vector<Vertex*> L(S.begin(), S.begin() + S.size()/2),
               		    R(S.begin() + S.size()/2, S.end());
		vector<Edge*> ldoldi = delaunay(L);
		vector<Edge*> rdirdo = delaunay(R);
		Edge *ldo = ldoldi[0]; 
		cout << "ldo printed:\n";
		ldo->print();
		Edge *ldi = ldoldi[1];
		cout << "ldi printed:\n";
		ldi->print();
		Edge *rdi = rdirdo[0]; 
		//rdi->print();
		Edge *rdo = rdirdo[1];
		//rdo->print();
		cout << "Got left and right triangulations" << endl;
		// compute lower common tangent of L and R
		while(true){
			if(leftOf(rdi->getOrigin(), ldi) > 0){
				cout << "rdi origin is leftof ldi\n";
				ldi = ldi->lnext();
			}else if(rightOf(ldi->getOrigin(), rdi) > 0){
				cout << "ldi origin is rightof rdi\n";
				rdi = rdi->rprev();
			}else{
				break;
			}
		}
		// create first cross edge base1 from rdi.Org to ldi.Org
	}
	return ret; 
}

int main (int argc, char* argv[]) {

	if (argc < 2) {
		cout << "Useage: ./program /data/filename.node" << endl;
		return 1;	
	}

	// initialize geometric functions
	exactinit();

	cout << "Reading points from file " << argv[1] << "...\n";
	vector<Vertex*> points = parse(argv[1]);
	for(int i =0; i < points.size(); i++){
		cout << "node " << points[i]->nodeNum << ": (" << points[i]->getPt()[0] << ", " << points[i]->getPt()[1] << ")\n";
	}

	// sort points lexicographically (break ties with y-coord)
	std::sort(points.begin(), points.end(), lex_compare);

	// TODO GET RID OF DUPLICATE POINTS

	cout << "Sorted points lexicographically: \n";
	for(int i =0; i < points.size(); i++){
		cout << "(" << points[i]->getPt()[0] << ", " << points[i]->getPt()[1] << ")\n";
	}

	// compute delaunay triangulation with standard diviclearde & conquer algorithm
	vector<Edge*> triangles = delaunay(points);
	printTriangles(triangles);

	//cout << "Writing triangles to file \n";
	//writeEle(argv[1]);
	
	return 0;
}
