#include "quadedge.h"

QuadEdge::QuadEdge(){
	// assign indicies for each edge in quadedge
	e[0].idx = 0;
	e[1].idx = 1;
	e[2].idx = 2;
	e[3].idx = 3;

	// set up next CCW edge pointers (around origin) for all edges
	e[0].next = &(e[0]);
	e[1].next = &(e[3]);
	e[2].next = &(e[2]);
	e[3].next = &(e[1]);
}

