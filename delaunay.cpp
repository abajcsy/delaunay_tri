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

/* Helper function to convert to proper data types for predicates.h
 */
static double* vertexToDoubleArr(Vertex *v){
	double p[2] = {v->getPt()[0], v->getPt()[1]};
	return p;
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
		myfile << triList.size() << " 3 0\n";
		for(int i = 0; i < triList.size(); i++){
			myfile << (i+1) << " " << triList[i][0]->getNodeNum() << " " << triList[i][1]->getNodeNum() << " " << triList[i][2]->getNodeNum();
		}
		myfile.close();
	}
	else cout << "Unable to open file";
}

/* Prints delaunay triangulation to console. 
 */
static vector< vector<Vertex*> > printTriangles(vector<Edge*> triangles){
	//TODO THIS ONLY WORKS FOR ONE TRIANGLE RIGHT NOW

	// list of all triangle vertices
	vector< vector<Vertex*> > triList;
	// list of 3 vertices that make up one triangle
	vector<Vertex*> tri;

	cout << "------- PRINTING TRIANGULATION -------\n";
	Edge *e = triangles[1];
	cout << "Triangle 1: \n";
	cout << "-- V" << e->getOrigin()->getNodeNum() << ": (" << e->getOrigin()->getPt()[0] << ", " << e->getOrigin()->getPt()[1] << ")\n";
	// store current vertex for first triangle
	tri.push_back(e->getOrigin());
	Edge *curr_e = e->lnext();
	int i = 2;
	while(curr_e != NULL && curr_e != e){
		cout << "-- V" << curr_e->getOrigin()->getNodeNum() << ": (" << curr_e->getOrigin()->getPt()[0] << ", " << curr_e->getOrigin()->getPt()[1] << ")\n";
		tri.push_back(curr_e->getOrigin());
		curr_e = curr_e->lnext();
		i += 1;
	}	

	// save list of vertices to list
	triList.push_back(tri);

	cout << "--------------------------------------\n";
	return triList;
}

/* Tests if point X is to the right of a given edge e.
 */
static double rightOf(Vertex *X, Edge *e){
	double *p1 = vertexToDoubleArr(X); 				//{X->pt[0], X->pt[1]};
	double *p2 = vertexToDoubleArr(e->getDest()); 	//{e->getDest()->pt[0], e->getDest()->pt[1]};
	double *p3 = vertexToDoubleArr(e->getOrigin()); //{e->getOrigin()->pt[0], e->getOrigin()->pt[1]};
	return orient2d(p1,p2,p3);
}

/* Tests if point X is to the left of a given edge e.
 */
static double leftOf(Vertex *X, Edge *e){
	double *p1 = vertexToDoubleArr(X); 				//{X->pt[0], X->pt[1]};
	double *p2 = vertexToDoubleArr(e->getDest()); 	//{e->getDest()->pt[0], e->getDest()->pt[1]};
	double *p3 = vertexToDoubleArr(e->getOrigin()); //{e->getOrigin()->pt[0], e->getOrigin()->pt[1]};
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
		double *p1 = vertexToDoubleArr(S[0]); //{S[0]->pt[0], S[0]->pt[1]};
		double *p2 = vertexToDoubleArr(S[1]); //{S[1]->pt[0], S[1]->pt[1]};
		double *p3 = vertexToDoubleArr(S[2]); //{S[2]->pt[0], S[2]->pt[1]};

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
		Edge *ldi = ldoldi[1];
		Edge *rdi = rdirdo[0]; 
		Edge *rdo = rdirdo[1];
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
		cout << "ldo printed:\n";
		ldo->print();
		cout << "ldi printed:\n";
		ldi->print();
		cout << "rdi printed:\n";
		rdi->print();
		cout << "rdo printed:\n";
		rdo->print();
		// create first cross edge base1 from rdi.Org to ldi.Org
		Edge *basel = Edge::connect(rdi->sym(), ldi); 
		cout << "basel:\n";
		basel->print();
		if(Vertex::equal(ldi->getOrigin(), ldo->getOrigin())){
			cout << "ldoldi equal\n";
			ldo = basel->sym();
		}
		if(Vertex::equal(rdi->getOrigin(), rdo->getOrigin())){
			cout << "rdirdo equal\n";
			rdo = basel;
		}
		// merge loop
		while(true){
			cout << "in merge loop...\n";
			// locate first L point (lcand.Dest) to be encountered by rising
			// bubble and delete L edges out of basel.Dest that fail circle test
			Edge *lcand = basel->sym()->onext();
			cout << "lcand:\n";
			lcand->print();
			if(valid(lcand, basel)){
				double *pa = vertexToDoubleArr(basel->getDest()); 		// {basel->getDest()->getPt()[0], basel->getDest()->getPt()[1]};
				double *pb = vertexToDoubleArr(basel->getOrigin()); 		// {basel->getOrigin()->getPt()[0], basel->getOrigin()->getPt()[1]};
				double *pc = vertexToDoubleArr(lcand->getDest()); 			// {lcand->getDest()->getPt()[0], lcand->getDest()->getPt()[1]};
				double *pd = vertexToDoubleArr(lcand->onext()->getDest()); // {lcand->onext()->getDest()->getPt()[0], lcand->onext()->getDest()->getPt()[1]};
				while(incircle(pa, pb, pc, pd)){
					Edge *t = lcand->onext();
					Edge::deleteEdge(lcand);
					lcand = t;
				}
			}
			Edge *rcand = basel->oprev();
			cout << "rcand:\n";
			rcand->print();
			// symmetrically locate the first R point to be hit, delete R edges
			if(valid(rcand, basel)){
				double *pa = vertexToDoubleArr(basel->getDest()); 			// {basel->getDest()->getPt()[0], basel->getDest()->getPt()[1]};
				double *pb = vertexToDoubleArr(basel->getOrigin()); 		// {basel->getOrigin()->getPt()[0], basel->getOrigin()->getPt()[1]};
				double *pc = vertexToDoubleArr(rcand->getDest()); 			// {rcand->getDest()->getPt()[0], rcand->getDest()->getPt()[1]};
				double *pd = vertexToDoubleArr(rcand->oprev()->getDest()); // {rcand->oprev()->getDest()->getPt()[0], rcand->oprev()->getDest()->getPt()[1]};
				while(incircle(pa, pb, pc, pd)){
					Edge *t = rcand->oprev();
					Edge::deleteEdge(rcand);
					rcand = t;
				}
			}
			// if both lcand and rcand are invalid, then basel is upper common tangent
			if(!valid(lcand, basel) && !valid(rcand, basel)){
				cout << "lcand and rcand invalid. basel is upper common tangent.\n";
				break;
			}
			// the next cross edge is to be connected to either lcand.Dest or 
			// rcand.Dest. If both are valid then choose the appropriate one
			// using the incircle test.
			double *pa = vertexToDoubleArr(lcand->getDest()); 	// {lcand->getDest()->getPt()[0], lcand->getDest()->getPt()[1]};
			double *pb = vertexToDoubleArr(lcand->getOrigin()); // {lcand->getOrigin()->getPt()[0], lcand->getOrigin()->getPt()[1]};
			double *pc = vertexToDoubleArr(rcand->getOrigin()); // {rcand->getOrigin()->getPt()[0], rcand->getOrigin()->getPt()[1]};
			double *pd = vertexToDoubleArr(rcand->getDest()); 	//{rcand->getDest()->getPt()[0], rcand->getDest()->getPt()[1]};
			if(!valid(lcand, basel) || (valid(rcand, basel) && incircle(pa, pb, pc, pd))){
				// add cross edge basel from rcand.Dest to basel.Dest
				cout << "adding cross edge basel from rcand.Dest to basel.Dest\n";
				basel = Edge::connect(rcand, basel->sym());
			}else{
				// add cross edge basel from basel.Org to lcand.Dest
				cout << "adding cross edge basel from basel.Org to lcand.Dest\n";
				basel = Edge::connect(basel->sym(), lcand->sym());
			}
		}
		// setup return value [ldo, rdo]
		ret.push_back(ldo);
		ret.push_back(rdo);
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
	//vector< vector<Vertex*> > triList = printTriangles(triangles);

	//cout << "Writing triangles to file... \n";
	//writeEle(argv[1], triList);
	
	return 0;
}
