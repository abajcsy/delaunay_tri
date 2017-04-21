
#include "vertex.h"
#include "edge.h"

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

// sets an outgoing edge from this vertex
void Vertex::setEdge(Edge *e){
	edge = e;
}

void Vertex::deleteEdge(Edge *e){
	Edge *onextEdge = e->onext();
	if(!Edge::equal(onextEdge, e)){
		e = onextEdge;
	}else{
		e = NULL;
	}
}

vector<double> Vertex::getPt(){
	return pt;
}

int Vertex::getNodeNum(){
	return nodeNum;
}

Edge* Vertex::getEdge(){
	return edge;
}

bool Vertex::equal(Vertex *v1, Vertex *v2){
	return (v1->pt[0] == v2->pt[0] && v1->pt[1] == v2->pt[1]);
}

void Vertex::print(){
	cout << "v " << nodeNum << ": (" << pt[0] << ", " << pt[1] << ")\n";
}
