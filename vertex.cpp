
#include "vertex.h"

Vertex::Vertex(){
	pt.push_back(0.0);
	pt.push_back(0.0);
	nodeNum = 0;
}

//Vertex::Vertex(double x, double y, int num){
//	pt.push_back(x);
//	pt.push_back(y);
//	nodeNum = num;
//}

void Vertex::setPt(vector<double> newPt){
	pt = newPt;
}

void Vertex::setPtX(double x){
	pt[0] = x;
}

void Vertex::setPtY(double y){
	pt[1] = y;
}


void Vertex::setNodeNum(int num){
	nodeNum = num;
}

vector<double> Vertex::getPt(){
	return pt;
}

int Vertex::getNodeNum(){
	return nodeNum;
}
