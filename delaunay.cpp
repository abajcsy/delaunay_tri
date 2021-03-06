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
#include <ctime>

#include "vertex.h"
#include "edge.h"
#include "quadedge.h"
#include "predicates.h"

using namespace std;
using std::ifstream;

/* Lexicographic comparator for sorting. (compares x, then y)
 * returns if v1 < v2 based on x-coord
 */
bool lex_compare(Vertex *v1, Vertex *v2) {
	if(v1->pt[0] == v2->pt[0]){
		return v1->pt[1] < v2->pt[1];
	}
	return v1->pt[0] < v2->pt[0];
}

/* y-coordinate based comparator for sorting. (compares y, then x)
 * returns if v1 < v2 based on y-coord
 */
bool y_compare(Vertex *v1, Vertex *v2) {
	if(v1->pt[1] == v2->pt[1]){
		return v1->pt[0] < v2->pt[0];
	}
	return v1->pt[1] < v2->pt[1];
}

/* Helper function to convert to proper data types for predicates.h
 */
static double* vertexToDoubleArr(Vertex *v){
	double *p = (double*)malloc(2*sizeof(double));
	p[0] = v->getPt()[0];
	p[1] = v->getPt()[1];
	//cout << "=> vertexToDoubleArr(): (" << p[0] << ", " << p[1] << ")\n";
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
static void writeEle(string filename, vector< vector<int> > tris){
	string delimiter = ".";
	string name = filename.substr(0, filename.find(delimiter));
	string newfilename = name + ".ele";
	cout << "New file name: " << newfilename << endl;

	ofstream myfile (newfilename.c_str());
	if (myfile.is_open()) {
		myfile << tris.size() << " 3 0\n";
		for(int i = 0; i < tris.size(); i++){
			myfile << (i+1);
			for(int j = 0; j < tris[i].size(); j++){
				myfile << " " << tris[i][j];
			}
			myfile << endl;
		}
		myfile.close();
	}
	else cout << "Unable to open file";
}

/* Parses and prints delaunay triangulation to console. 
 */
static vector< vector<int> > parseTriangles(vector<Vertex*> points){
	// list of all triangle vertices
	vector< vector<int> > list;

	//cout << "------- PRINTING TRIANGULATION -------\n";

	for(int i =0; i < points.size(); i++){
		//v.push_back(points[i]->getNodeNum();
		Edge *og_e = points[i]->getEdge();

		Edge *curr_e = og_e;
		Edge *next_e = curr_e->onext();

		do{
			Vertex *o = og_e->getOrigin();
			Vertex *c = curr_e->getDest();
			Vertex *n = next_e->getDest();

			double *p1 = vertexToDoubleArr(o); 
			double *p2 = vertexToDoubleArr(c); 
			double *p3 = vertexToDoubleArr(n); 
			bool isCCW = (orient2d(p1,p2,p3) > 0);

			// if original vertex lex least from next two points, 
			// and all the pts are CCW from each other, add it to list
			if(lex_compare(o,c) && lex_compare(o, n) && isCCW){			
				vector<int> v;
				v.push_back(o->getNodeNum());
				v.push_back(c->getNodeNum());
				v.push_back(n->getNodeNum());
				list.push_back(v);
			} 
			curr_e = curr_e->onext();
			next_e = curr_e->onext();
		}while(!Edge::equal(curr_e, og_e));
	}


	/*cout << "printing all the triangles....\n";
	for(int i = 0; i < list.size(); i++){
		for(int j = 0; j < list[i].size(); j++){
			cout << " " << list[i][j];
		}
		cout << endl;
	}
	cout << "--------------------------------------\n";*/
	return list;
}

/* Tests if point X is to the right of a given edge e.
 */
static double rightOf(Vertex *X, Edge *e){
	double *p1 = vertexToDoubleArr(X); 				
	double *edest = vertexToDoubleArr(e->getDest()); 	
	double *eorg = vertexToDoubleArr(e->getOrigin()); 
	//cout << "in rightOf(): orient2d = " << orient2d(p1,edest,eorg) << endl;
	return orient2d(p1,edest,eorg);
}

/* Tests if point X is to the left of a given edge e.
 */
static double leftOf(Vertex *X, Edge *e){
	double *p1 = vertexToDoubleArr(X); 				
	double *eorg = vertexToDoubleArr(e->getOrigin()); 	
	double *edest = vertexToDoubleArr(e->getDest()); 
	//cout << "in leftOf(): orient2d = " << orient2d(p1,eorg,edest) << endl;
	return orient2d(p1,eorg,edest);
}

/* Tests whether the edge e is above basel.
 */
static bool valid(Edge *e, Edge *basel){
	return (rightOf(e->getDest(), basel)>0);
}

/* Delaunay triangulation with alternating cut modification
 * uses:
 *	- nth_element() "is a partial sorting algorithm that rearranges elements in [first, last) such that:
 *  The element pointed at by nth is changed to whatever element would occur in that position if [first, last) was sorted.
 *  All of the elements before this new nth element are less than or equal to the elements after the new nth element."
 *  http://en.cppreference.com/w/cpp/algorithm/nth_element 
 * cutDir - x (0) or y (1) cut direction 
 */
static vector<Edge*> altCutDT(vector<Vertex*> S, bool cutDir){
	//cout << "in altCutDT()\n";
	vector<Edge*> ret;
	if(S.size() == 2){
		//cout << "Case: |S| = 2" << endl;
		Edge *a = Edge::makeEdge();
		a->setOrigin(S[0]);
		a->setDest(S[1]);
		//cout << "a: \n";
		//a->print();
		// setup return value [a, a.Sym]
		ret.push_back(a);
		ret.push_back(a->sym());
	}else if(S.size() == 3){
		// sort points
		std::sort(S.begin(), S.end(), lex_compare);
	
		//cout << "Case: |S| = 3" << endl;
		Edge *a = Edge::makeEdge();
		Edge *b = Edge::makeEdge();
		Edge::splice(a->sym(),b);

		a->setOrigin(S[0]);
		a->setDest(S[1]);
		b->setOrigin(S[1]);
		b->setDest(S[2]);

		double *p1 = vertexToDoubleArr(S[0]); 
		double *p2 = vertexToDoubleArr(S[1]); 
		double *p3 = vertexToDoubleArr(S[2]); 
		if(orient2d(p1,p2,p3) > 0){
			//cout << "Connecting b to a with c:\n";
			Edge *c = Edge::connect(b,a);
			//c->print();
			//cout << "Returning [a, b.Sym]\n";
			// setup return value [a, b.Sym]
			ret.push_back(a);
			ret.push_back(b->sym());
		}else if(orient2d(p1,p3,p2) > 0){
			//cout << "Connecting b to a with c:\n";
			Edge *c = Edge::connect(b,a);
			//c->print();
			//cout << "Returning [c.Sym, c]\n";
			// setup return value [c.Sym, c]
			ret.push_back(c->sym());
			ret.push_back(c);
		}else{ // points are colinear
			//cout << "Returning [a, b.Sym]\n";
			// setup return value [a, b.Sym]
			ret.push_back(a);
			ret.push_back(b->sym());
		}	
	}else{ // S.size() >= 4
		//cout << "Case: |S| >= 4" << endl;
		/* nth_element(RandomIt first, RandomIt nth, RandomIt last)
		 * rearranges elements [first, last) such that all elements
	  	 * before new nth element are <= nth ascending
		 */
		if(cutDir == 0){
			// cut on x-coords
			nth_element(S.begin(), S.begin() + S.size()/2, S.end(), lex_compare);
		}else{
			// cut on y-coords
			nth_element(S.begin(), S.begin() + S.size()/2, S.end(), y_compare);
		}
		// split points in half, L = left half of S, R = right half of S 
		vector<Vertex*> L(S.begin(), S.begin() + S.size()/2),
               		    R(S.begin() + S.size()/2, S.end());

		vector<Edge*> ldoldi = altCutDT(L, !cutDir);
		vector<Edge*> rdirdo = altCutDT(R, !cutDir);

		Edge *ldo = ldoldi[0]; 
		Edge *ldi = ldoldi[1];
		Edge *rdi = rdirdo[0]; 
		Edge *rdo = rdirdo[1];

		// if in the horizontal y-cut case, adjust the convex hull pointers
		if(cutDir == 1){
			while(y_compare(ldo->getOrigin(), ldo->rnext()->getOrigin())){
				ldo = ldo->rnext();
			}

			while(y_compare(ldi->lnext()->getOrigin(), ldi->getOrigin())){
				ldi = ldi->lnext();			
			}

			while(y_compare(rdi->getOrigin(), rdi->rnext()->getOrigin())){
				rdi = rdi->rnext();
			}

			while(y_compare(rdo->lnext()->getOrigin(), rdo->getOrigin())){
				rdo = rdo->lnext();
			}

			Edge *tmpldo = ldo;
			Edge *tmpldi = ldi;

			// swap pointers appropriately
			ldi = rdo;
			rdo = tmpldi;
			ldo = rdi;			
			rdi = tmpldo;
			
		}
		
		// compute lower common tangent of L and R
		while(true){
			if(leftOf(rdi->getOrigin(), ldi) > 0){
				//cout << "rdi origin is leftof ldi\n";
				ldi = ldi->lnext();
			}else if(rightOf(ldi->getOrigin(), rdi) > 0){
				//cout << "ldi origin is rightof rdi\n";
				rdi = rdi->rprev();
			}else{
				break;
			}
		}
		// create first cross edge base1 from rdi.Org to ldi.Org
		Edge *basel = Edge::connect(rdi->sym(), ldi); 

		if(Vertex::equal(ldi->getOrigin(), ldo->getOrigin())){
			ldo = basel->sym();
		}
		if(Vertex::equal(rdi->getOrigin(), rdo->getOrigin())){
			rdo = basel;
		}


		// merge loop
		while(true){
			//cout << "in merge loop...\n";
			// locate first L point (lcand.Dest) to be encountered by rising
			// bubble and delete L edges out of basel.Dest that fail circle test
			Edge *lcand = basel->sym()->onext();
			if(valid(lcand, basel)){
				double *pa = vertexToDoubleArr(basel->getDest()); 	
				double *pb = vertexToDoubleArr(basel->getOrigin()); 		
				double *pc = vertexToDoubleArr(lcand->getDest()); 		
				double *pd = vertexToDoubleArr(lcand->onext()->getDest()); 
				while(incircle(pa, pb, pc, pd) > 0){
					//cout << "locating first L point (lcand.Dest) to be encountered by rising\n";
					Edge *t = lcand->onext();
					Edge::deleteEdge(lcand);
					lcand = t;
					// update lcand for incircle test
					pc = vertexToDoubleArr(lcand->getDest()); 		
					pd = vertexToDoubleArr(lcand->onext()->getDest()); 
				}
			}
			Edge *rcand = basel->oprev();
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
					// update rcand for incircle test
					pc = vertexToDoubleArr(rcand->getDest()); 			
					pd = vertexToDoubleArr(rcand->oprev()->getDest()); 
				}
			}
			// if both lcand and rcand are invalid, then basel is upper common tangent
			if(!valid(lcand, basel) && !valid(rcand, basel)){
				//cout << "lcand and rcand invalid. basel is upper common tangent.\n";
				break;
			}
			// the next cross edge is to be connected to either lcand.Dest or 
			// rcand.Dest. If both are valid then choose the appropriate one
			// using the incircle test.
			double *pa = vertexToDoubleArr(lcand->getDest()); 
			double *pb = vertexToDoubleArr(lcand->getOrigin()); 
			double *pc = vertexToDoubleArr(rcand->getOrigin()); 
			double *pd = vertexToDoubleArr(rcand->getDest()); 	
			if(!valid(lcand, basel) || (valid(rcand, basel) && (incircle(pa, pb, pc, pd) > 0))){
				// add cross edge basel from rcand.Dest to basel.Dest
				//cout << "adding cross edge basel from rcand.Dest to basel.Dest\n";
				basel = Edge::connect(rcand, basel->sym());
				//cout << "new basel:\n";
				//basel->print();
				//cout << endl;
			}else{
				// add cross edge basel from basel.Org to lcand.Dest
				//cout << "adding cross edge basel from basel.Org to lcand.Dest\n";
				basel = Edge::connect(basel->sym(), lcand->sym());	
				//cout << "new basel:\n";
				//basel->print();
				//cout << endl;
			}
		}

		// if in the horizontal y-cut case, adjust the convex hull pointers
		if(cutDir == 1){
			while(lex_compare(ldo->rprev()->getOrigin(), ldo->getOrigin())){
				ldo = ldo->rprev();
			}
			while(lex_compare(rdo->getOrigin(), rdo->lprev()->getOrigin())){
				rdo = rdo->lprev();
			}
		} 
		// setup return value [ldo, rdo]
		ret.push_back(ldo);
		ret.push_back(rdo);
	}
	return ret; 
}

/* Constructs delaunay triangulation with the divide and conquer algorithm
 * Reference: Gubias and Stolfi.
 */
static vector<Edge*> divideConquerDT(vector<Vertex*> S){
	//cout << "in divideConquerDT()\n";
	vector<Edge*> ret;
	if(S.size() == 2){
		//cout << "Case: |S| = 2" << endl;
		Edge *a = Edge::makeEdge();
		a->setOrigin(S[0]);
		a->setDest(S[1]);
		//cout << "a: \n";
		//a->print();
		// setup return value [a, a.Sym]
		ret.push_back(a);
		ret.push_back(a->sym());
	}else if(S.size() == 3){
		//cout << "Case: |S| = 3" << endl;
		Edge *a = Edge::makeEdge();
		Edge *b = Edge::makeEdge();
		Edge::splice(a->sym(),b);

		a->setOrigin(S[0]);
		a->setDest(S[1]);
		b->setOrigin(S[1]);
		b->setDest(S[2]);

		//cout << "NEW a:\n";
		//a->print();
		//cout << "NEW b:\n";
		//b->print();

		double *p1 = vertexToDoubleArr(S[0]); 
		double *p2 = vertexToDoubleArr(S[1]); 
		double *p3 = vertexToDoubleArr(S[2]); 
		if(orient2d(p1,p2,p3) > 0){
			//cout << "Connecting b to a with c:\n";
			Edge *c = Edge::connect(b,a);
			//c->print();
			//cout << "Returning [a, b.Sym]\n";
			// setup return value [a, b.Sym]
			ret.push_back(a);
			ret.push_back(b->sym());
		}else if(orient2d(p1,p3,p2) > 0){
			//cout << "Connecting b to a with c:\n";
			Edge *c = Edge::connect(b,a);
			//c->print();
			//cout << "Returning [c.Sym, c]\n";
			// setup return value [c.Sym, c]
			ret.push_back(c->sym());
			ret.push_back(c);
		}else{ // points are colinear
			//cout << "Returning [a, b.Sym]\n";
			// setup return value [a, b.Sym]
			ret.push_back(a);
			ret.push_back(b->sym());
		}	
	}else{ // S.size() >= 4
		//cout << "Case: |S| >= 4" << endl;
		// split points in half, L = left half of S, R = right half of S 
		vector<Vertex*> L(S.begin(), S.begin() + S.size()/2),
               		    R(S.begin() + S.size()/2, S.end());
		vector<Edge*> ldoldi = divideConquerDT(L);
		vector<Edge*> rdirdo = divideConquerDT(R);
		Edge *ldo = ldoldi[0]; 
		Edge *ldi = ldoldi[1];
		Edge *rdi = rdirdo[0]; 
		Edge *rdo = rdirdo[1];
		//cout << "Got left and right triangulations" << endl;
		// compute lower common tangent of L and R
		while(true){
			//cout << "rdi:\n";
			//rdi->print();
			//cout << "ldi:\n";
			//ldi->print();
			if(leftOf(rdi->getOrigin(), ldi) > 0){
				//cout << "rdi origin is leftof ldi\n";
				ldi = ldi->lnext();
			}else if(rightOf(ldi->getOrigin(), rdi) > 0){
				//cout << "ldi origin is rightof rdi\n";
				rdi = rdi->rprev();
			}else{
				break;
			}
		}
		// create first cross edge base1 from rdi.Org to ldi.Org
		Edge *basel = Edge::connect(rdi->sym(), ldi); 
		//cout << "basel connected:\n";
		//basel->print();
		if(Vertex::equal(ldi->getOrigin(), ldo->getOrigin())){
			//cout << "ldoldi equal\n";
			ldo = basel->sym();
		}
		if(Vertex::equal(rdi->getOrigin(), rdo->getOrigin())){
			//cout << "rdirdo equal\n";
			rdo = basel;
		}
		// merge loop
		while(true){
			//cout << "in merge loop...\n";
			// locate first L point (lcand.Dest) to be encountered by rising
			// bubble and delete L edges out of basel.Dest that fail circle test
			Edge *lcand = basel->sym()->onext();
			//cout << "lcand:\n";
			//lcand->print();
			if(valid(lcand, basel)){
				double *pa = vertexToDoubleArr(basel->getDest()); 	
				double *pb = vertexToDoubleArr(basel->getOrigin()); 		
				double *pc = vertexToDoubleArr(lcand->getDest()); 		
				double *pd = vertexToDoubleArr(lcand->onext()->getDest()); 
				while(incircle(pa, pb, pc, pd) > 0){
					//cout << "locating first L point (lcand.Dest) to be encountered by rising\n";
					Edge *t = lcand->onext();
					Edge::deleteEdge(lcand);
					lcand = t;
					// update lcand for incircle test
					pc = vertexToDoubleArr(lcand->getDest()); 		
					pd = vertexToDoubleArr(lcand->onext()->getDest()); 
				}
			}
			Edge *rcand = basel->oprev();
			//cout << "rcand:\n";
			//rcand->print();
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
					// update rcand for incircle test
					pc = vertexToDoubleArr(rcand->getDest()); 			
					pd = vertexToDoubleArr(rcand->oprev()->getDest()); 
				}
			}
			// if both lcand and rcand are invalid, then basel is upper common tangent
			if(!valid(lcand, basel) && !valid(rcand, basel)){
				//cout << "lcand and rcand invalid. basel is upper common tangent.\n";
				break;
			}
			// the next cross edge is to be connected to either lcand.Dest or 
			// rcand.Dest. If both are valid then choose the appropriate one
			// using the incircle test.
			double *pa = vertexToDoubleArr(lcand->getDest()); 
			double *pb = vertexToDoubleArr(lcand->getOrigin()); 
			double *pc = vertexToDoubleArr(rcand->getOrigin()); 
			double *pd = vertexToDoubleArr(rcand->getDest()); 	
			if(!valid(lcand, basel) || (valid(rcand, basel) && (incircle(pa, pb, pc, pd) > 0))){
				// add cross edge basel from rcand.Dest to basel.Dest
				//cout << "adding cross edge basel from rcand.Dest to basel.Dest\n";
				basel = Edge::connect(rcand, basel->sym());
				//cout << "new basel:\n";
				//basel->print();
				//cout << endl;
			}else{
				// add cross edge basel from basel.Org to lcand.Dest
				//cout << "adding cross edge basel from basel.Org to lcand.Dest\n";
				basel = Edge::connect(basel->sym(), lcand->sym());	
				//cout << "new basel:\n";
				//basel->print();
				//cout << endl;
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

	// compute delaunay triangulation with standard diviclearde & conquer algorithm
	vector<Edge*> triangles;
	if (argv[2][0] == '-') {
		if(argv[2][1] == 'd'){
			clock_t start_s = clock();
			// sort points lexicographically (break ties with y-coord)
			std::sort(points.begin(), points.end(), lex_compare);
			// run standard divide and conquer delaunay triangulation
			triangles = divideConquerDT(points);
			
			clock_t stop_s = clock();
			cout << "time (s): " << (stop_s-start_s)/double(CLOCKS_PER_SEC) << endl;
		}else if(argv[2][1] == 'a'){
			clock_t start_s=clock();

			bool cutDir = 0;
			// run modified alternating cut delaunay triangulation 
			triangles = altCutDT(points, cutDir);

			clock_t stop_s=clock();
			cout << "time (s): " << (stop_s-start_s)/double(CLOCKS_PER_SEC) << endl;
		}
	}
	
	vector< vector<int> > triList = parseTriangles(points);

	cout << "Writing triangles to file... \n";
	writeEle(argv[1], triList);
	
	return 0;
}
