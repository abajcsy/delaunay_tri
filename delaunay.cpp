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
	double *p = (double*)malloc(2*sizeof(double));
	p[0] = v->getPt()[0];
	p[1] = v->getPt()[1];
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

	// stores all unique parsed triangle information
	vector< vector<int> > tris;

	for(int i = 0; i < triList.size(); i++){
		bool isLexLeast = 1;
		for(int j = 1; j < triList[i].size(); j++){
			if(!lex_compare(triList[i][0], triList[i][j])){
				isLexLeast = 0;
			}
		}
		if(isLexLeast){
			vector<int> t;
			t.push_back((i+1)); 
			for(int j = 0; j < triList[i].size(); j++){
				t.push_back(triList[i][j]->getNodeNum());
			}
			tris.push_back(t);
		}
	}

	ofstream myfile (newfilename.c_str());
	if (myfile.is_open()) {
		myfile << tris.size() << " 3 0\n";
		for(int i = 0; i < tris.size(); i++){
			for(int j = 0; j < tris[i].size(); j++){
				myfile << " " << tris[i][j];
			}
			myfile << endl;
		}
		myfile.close();
	}
	else cout << "Unable to open file";
}

/* Prints delaunay triangulation to console. 
 */
static vector< vector<Vertex*> > printTriangles(vector<Vertex*> points){
	//TODO THIS ONLY WORKS FOR ONE TRIANGLE RIGHT NOW
	// list of all triangle vertices
	vector< vector<Vertex*> > triList;

	cout << "------- PRINTING TRIANGULATION -------\n";

	for(int i =0; i < points.size(); i++){
		vector<Vertex*> tri;
		tri.push_back(points[i]);
		cout << "----printing vertex " << points[i]->getNodeNum() << "----\n";
		Edge *curr_e = points[i]->getEdge();
		tri.push_back(curr_e->getDest());
		curr_e->print();
		Edge *e = curr_e->onext();
		while(!Edge::equal(e, curr_e)){ 
			e->print();
			tri.push_back(e->getDest());
			e = e->onext();
		}
		triList.push_back(tri);	
	}

	/*for(int idx = 0; idx < triangles.size(); idx++){
		// list of 3 vertices that make up one triangle
		vector<Vertex*> tri;

		Edge *e = triangles[idx];
		cout << "Triangle " << (idx+1) << ":\n";
		cout << "-- V" << e->getOrigin()->getNodeNum() << ": (" << e->getOrigin()->getPt()[0] << ", " << e->getOrigin()->getPt()[1] << ")\n";
		// store current vertex for first triangle
		tri.push_back(e->getOrigin());
		Edge *curr_e = e->lnext();
		int i = 2;
		while(curr_e != NULL && !Vertex::equal(curr_e->getOrigin(), e->getOrigin())){
			cout << "-- V" << curr_e->getOrigin()->getNodeNum() << ": (" << curr_e->getOrigin()->getPt()[0] << ", " << curr_e->getOrigin()->getPt()[1] << ")\n";
			tri.push_back(curr_e->getOrigin());
			curr_e = curr_e->lnext();
			i += 1;
		}	

		// save list of vertices to list
		triList.push_back(tri);
	} */
	cout << "--------------------------------------\n";
	return triList;
}

/* Tests if point X is to the right of a given edge e.
 */
static double rightOf(Vertex *X, Edge *e){
	double *p1 = vertexToDoubleArr(X); 				
	double *edest = vertexToDoubleArr(e->getDest()); 	
	double *eorg = vertexToDoubleArr(e->getOrigin()); 
	return orient2d(p1,edest,eorg);
}

/* Tests if point X is to the left of a given edge e.
 */
static double leftOf(Vertex *X, Edge *e){
	double *p1 = vertexToDoubleArr(X); 				
	double *eorg = vertexToDoubleArr(e->getOrigin()); 	
	double *edest = vertexToDoubleArr(e->getDest()); 
	cout << "in leftOf(): orient2d = " << orient2d(p1,eorg,edest) << endl;
	return orient2d(p1,eorg,edest);
}

/* Tests whether the edge e is above basel.
 */
static double valid(Edge *e, Edge *basel){
	return rightOf(e->getDest(), basel);
}

static vector<Edge*> altCutDT(vector<Vertex*> S){
	//TODO IMPLEMENT THIS
	vector<Edge*> ret;
	return ret;
}

/* Constructs delaunay triangulation with the divide and conquer algorithm
 * Reference: Gubias and Stolfi.
 */
static vector<Edge*> divideConquerDT(vector<Vertex*> S){
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
		Edge::splice(a->sym(),b);

		cout << "a: \n";
		a->print();
		cout << "b: \n";
		b->print();

		a->setOrigin(S[0]);
		a->setDest(S[1]);
		b->setOrigin(S[1]);
		b->setDest(S[2]);

		cout << "NEW a:\n";
		a->print();
		cout << "NEW b:\n";
		b->print();

		double *p1 = vertexToDoubleArr(S[0]); 
		double *p2 = vertexToDoubleArr(S[1]); 
		double *p3 = vertexToDoubleArr(S[2]); 
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
		vector<Edge*> ldoldi = divideConquerDT(L);
		vector<Edge*> rdirdo = divideConquerDT(R);
		Edge *ldo = ldoldi[0]; 
		Edge *ldi = ldoldi[1];
		Edge *rdi = rdirdo[0]; 
		Edge *rdo = rdirdo[1];
		cout << "Got left and right triangulations" << endl;
		// compute lower common tangent of L and R
		while(true){
			cout << "rdi:\n";
			rdi->print();
			cout << "ldi:\n";
			ldi->print();
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
		cout << "rdi->sym() printed:\n";
		rdi->sym()->print();
		cout << "rdo printed:\n";
		rdo->print();
		// create first cross edge base1 from rdi.Org to ldi.Org
		Edge *basel = Edge::connect(rdi->sym(), ldi); 
		cout << "basel connected:\n";
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
				double *pa = vertexToDoubleArr(basel->getDest()); 	
				double *pb = vertexToDoubleArr(basel->getOrigin()); 		
				double *pc = vertexToDoubleArr(lcand->getDest()); 		
				double *pd = vertexToDoubleArr(lcand->onext()->getDest()); 
				while(incircle(pa, pb, pc, pd) > 0){
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
				double *pa = vertexToDoubleArr(basel->getDest()); 			
				double *pb = vertexToDoubleArr(basel->getOrigin()); 		
				double *pc = vertexToDoubleArr(rcand->getDest()); 			
				double *pd = vertexToDoubleArr(rcand->oprev()->getDest()); 
				while(incircle(pa, pb, pc, pd) > 0){
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
			double *pa = vertexToDoubleArr(lcand->getDest()); 
			double *pb = vertexToDoubleArr(lcand->getOrigin()); 
			double *pc = vertexToDoubleArr(rcand->getOrigin()); 
			double *pd = vertexToDoubleArr(rcand->getDest()); 	
			if(!valid(lcand, basel) || (valid(rcand, basel) && incircle(pa, pb, pc, pd) > 0)){
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

	if (argc < 3) {
		cout << "Useage: <program> <.node file> <-d (for divide and conquer) or -a (for alternating cut)>" << endl;
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
	vector<Edge*> triangles;
	if (argv[2][0] == '-') {
		if(argv[2][1] == 'd'){
			triangles = divideConquerDT(points);
		}else if(argv[2][1] == 'a'){
			triangles = altCutDT(points);
		}
	}

	cout << "\nPrinting edge connected to each vertex...\n";
	for(int i =0; i < points.size(); i++){
		cout << "----printing vertex " << points[i]->getNodeNum() << "----\n";
		Edge *curr_e = points[i]->getEdge();
		curr_e->print();
		Edge *e = curr_e->onext();
		while(!Edge::equal(e, curr_e)){ 
			e->print();
			e = e->onext();
		}
	}
	
	vector< vector<Vertex*> > triList = printTriangles(points);

	cout << "Writing triangles to file... \n";
	writeEle(argv[1], triList);
	
	return 0;
}
