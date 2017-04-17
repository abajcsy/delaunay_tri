// Edge class

#include <iostream>
#include "edge.h"

using namespace std;

//*************** QuadEdge Structure ****************//
class QuadEdge{
	public:
		Edge e[4];	// four edge records containing data and next fields

	public: 
		QuadEdge::QuadEdge(){
			// assign indicies for each edge in quadedge
			this->e[0].idx = 0;
			this->e[1].idx = 1;
			this->e[2].idx = 2;
			this->e[3].idx = 3;
	
			// set up next CCW edge pointers (around origin) for all edges
			this->e[0].next = &(e[0]);
			this->e[1].next = &(e[3]);
			this->e[2].next = &(e[2]);
			this->e[3].next = &(e[1]);
		}
};

//*************** Edge Functions ****************//

/* Returns dual edge (right --> left): (e,r) rot = (e, r+1) */
Edge* Edge::rot(){
	if (idx >= 3) {
		return this - 3;
	}else{
		return this + 1;
	}
}

/* Returns dual edge (left --> right): (e,r) Rot^-1 = (e,r+3) */
Edge* Edge::invrot(){
	if (idx > 0) {
		return this - 1;
	} else {
		return this + 3;
	}
}

/* Returns edge from dest --> origin: (e,r) Sym = (e,r+2) */
Edge* Edge::sym(){
	if (idx < 2) {
		return this + 2;
	} else {
		return this - 2;
	}
}

/* Returns next CCW edge from origin of edge: (e,r) Onext = e[r].next */
Edge* Edge::onext(){
	return next;
}

/* Returns next CW edge from origin of edge: (e,r) Oprev = (e[r+1].next) Rot */
Edge* Edge::oprev(){
	return rot()->onext()->rot();
}

/* Returns next CCW edge into dest of edge */
Edge* Edge::dnext(){
	return sym()->onext()->sym();
}

/* Returns next CW edge into dest of edge */
Edge* Edge::dprev(){
	return invrot()->onext()->invrot();
}

/* Returns CCW edge around left face following edge */
Edge* Edge::lnext(){
	return invrot()->onext()->rot();
}

/* Returns CCW edge around left face before edge */
Edge* Edge::lprev(){
	return onext()->sym();
}

/* Returns CCW edge around right face following edge */
Edge* Edge::rnext(){
	return rot()->onext()->invrot();
}

/* Returns CCW edge around right face before edge */
Edge* Edge::rprev(){
	return sym()->onext();
}

/* Returns the origin of this edge */
double* origin(){
	return origin;
}

/* Returns destination of this edge */
// TODO IS THIS RIGHT OR SHOULD BE sym->origin ?
double* dest(){
	return dest;
}

/* Returns empty new edge */
static Edge* makeEdge(){
	return (new QuadEdge())->e;
}

/* Splices two edges together:
	- if aOrg and bOrg are distinct, combines into one
	- if aOrg and bOrg are exactly the same ring, breaks into 2 separate pieces
	- if aOrg and bOrg are same ring with opposite orientations, flip (and
	  reverse the order) of a segment of that ring.  
 */
static void splice(Edge *a, Edge *b){
	// sanity check
	if(a == NULL || b == NULL){
		cout << "Error: splice() has null parameters." << endl;
		return ;
	}

	Edge* alpha = a->onext()->rot(); 
	Edge *beta = b->onext()->rot();

	// save old onext values of a,b,alpha,beta
	Edge *tmpa = a->onext();
	Edge *tmpb = b->onext();
	Edge *tmpalpha = alpha->onext();
	Edge *tmpbeta = beta->onext();

	// interchange values of aOnext with bOnext and alphaOnext with betaOnext
	a->next = tmpb;
	b->next = tmpa;
	alpha->next = tmpbeta;
	beta->next = tmpalpha;
}

/* Adds new edge e connecting the dest of a to the origin of b so that
 * aLeft = eLeft = bLeft, and sets e.origin = a.dest and e.dest = b.origin
 */
static Edge* connect(Edge *a, Edge *b){
	Edge *e = makeEdge();
	e->origin = a->dest();
	splice(e, a->lnext());
	splice(e->sym(), b);
	return e;
}

/* Disconnects edge e from the rest of the structure */
static void deleteEdge(Edge *e){
	splice(e, e.oprev());
	splice(e->sym(), e->sym()->oprev());
}


