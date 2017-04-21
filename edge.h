#ifndef EDGE_H_
#define EDGE_H_

class QuadEdge;
class Vertex;

using namespace std;

class Edge {
	public:
		int idx;				// index of edge in quadedge data struct
		Edge *next;				// holds reference to onext edge
		Vertex *origin;			// holds ID and (x,y) coord of origin vertex
		Vertex *dest;			// holds ID and (x,y) coord of dest vertex

		Edge();
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
		Vertex* getOrigin();		// returns origin vertex of this edge 
		Vertex* getDest();			// returns destination vertex of this edge
		void setOrigin(Vertex* pt);			// sets origin of edge
		void setDest(Vertex* pt);			// sets dest of edge

		/* creates a new edge that is disconnected */
		static Edge* makeEdge();
		/* splices two edges a and b */
		static void splice(Edge *a, Edge *b);
		/* adds new edge e connecting the dest of a to the origin of b */
		static Edge* connect(Edge *a, Edge *b);
		/* disconnects edge e from the rest of the structure */
		static void deleteEdge(Edge *e);
		/* tests if two edges are same */
		static bool equal(Edge *a, Edge *b);

		void print();
};

#endif


