#ifndef EDGE_H_
#define EDGE_H_

class Edge {
	public:
		int idx;			// index of edge in quadedge data struct
		Edge *next;			// holds reference to onext edge
		const void *data; 	// holds geometric and nontopological info
 
		Edge* rot();
		Edge* invrot();
		Edge* onext();
		Edge* oprev();
		Edge* sym();
		void splice(Edge*, Edge*);
		//origin();
		//dest();
};

#endif


