
#include "vertex.h"

Vertex::Vertex(){
	pt.push_back(0.0);
	pt.push_back(0.0);
	nodeNum = 0;
}

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

bool Vertex::equal(Vertex *v1, Vertex *v2){
	return (v1->pt[0] == v2->pt[0] && v1->pt[1] == v2->pt[1]);
}
