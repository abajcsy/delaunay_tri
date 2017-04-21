#ifndef VERTEX_H_
#define VERTEX_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "edge.h"

using namespace std;

class Vertex {
	public:
		vector<double> pt;
		int nodeNum;
		Edge *edge;
	
	public:
		Vertex();
		void setPt(vector<double> newPt);
		void setPtX(double x);
		void setPtY(double y);
		void setNodeNum(int num);
		void setEdge(Edge *e);
		void deleteEdge(Edge *e);

		vector<double> getPt();
		int getNodeNum();
		Edge* getEdge();
		void print();

		static bool equal(Vertex *v1, Vertex *v2);
};

#endif


