#ifndef VERTEX_H_
#define VERTEX_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;

class Vertex {
	public:
		vector<double> pt;
		int nodeNum;
	
	public:
		Vertex();
		//Vertex(double x, double y, int nodeNum);
		void setPt(vector<double> newPt);
		void setPtX(double x);
		void setPtY(double y);
		void setNodeNum(int num);
		vector<double> getPt();
		int getNodeNum();
};

#endif


