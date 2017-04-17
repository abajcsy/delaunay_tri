#ifndef EDGE_H_
#define EDGE_H_

class QuadEdge;

class Edge {
	public:
		int idx;				// index of edge in quadedge data struct
		Edge *next;				// holds reference to onext edge
		double origin[2]; 		// holds (x,y) coord of origin vertex
 		double dest[2]; 		// holds (x,y) coord of destination vertex

		Edge* rot();			// returns dual edge (right --> left)
		Edge* invrot();			// returns dual edge (left --> right)
		Edge* sym();			// returns edge from dest --> origin
		Edge* onext();			// returns next CCW edge from origin of edge
		Edge* oprev();			// returns next CW edge from origin of edge
		Edge* dnext();			// returns next CCW edge into dest of edge
		Edge* dprev();			// returns next CW edge into dest of edge
		Edge* lnext();			// returns CCW edge around left face following edge
		Edge* lprev();			// returns CCW edge around left face before edge
		Edge* rnext();			// returns CCW edge around right face following edge
		Edge* rprev();			// returns CCW edge around right face before edge
		//Face* left();
		//Face* right();
		double* origin();		// returns origin vertex of this edge 
		double* dest();			// returns destination vertex of this edge

		/* creates a new edge that is disconnected */
		static Edge* makeEdge();
		/* splices two edges a and b */
		static void splice(Edge *a, Edge *b);
		/* adds new edge e connecting the dest of a to the origin of b */
		static Edge* connect(Edge *a, Edge *b);
		/* disconnects edge e from the rest of the structure */
		static void deleteEdge(Edge *e);
};

#endif


