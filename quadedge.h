#ifndef QUADEDGE_H_
#define QUADEDGE_H_

#include "edge.h"

class QuadEdge{
	public:
		Edge e[4];	// four edge records containing data and next fields

	public: 
		QuadEdge();
};

#endif
