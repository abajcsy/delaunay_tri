// QuadEdge class

#include "edge.h"
#include "quadedge.h"

using namespace std;

//*************** Edge Functions ****************//

Edge::Edge(){
	origin = new Vertex();
	dest = new Vertex();
}

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
Vertex* Edge::getOrigin(){
	return origin;
}

/* Returns destination of this edge */
Vertex* Edge::getDest(){
	return dest;
}

void Edge::setOrigin(Vertex *pt){
	origin = pt;
	sym()->dest = pt;
}

void Edge::setDest(Vertex *pt){
	sym()->origin = pt;
	dest = pt;
}

void Edge::print(){
	cout << "Edge idx: " << idx << endl;
	cout << "--> Origin [node# " << origin->getNodeNum() << "]: (" << origin->getPt()[0] << ", " << origin->getPt()[1] << ")\n";
	cout << "--> Dest [node# " << dest->getNodeNum() << "]: (" << dest->getPt()[0] << ", " << dest->getPt()[1] << ")\n";	
	/*if(!origin->getPt().empty() && !dest->getPt().empty()){
		cout << "--> Origin: (" << origin->getPt()[0] << ", " << origin->getPt()[1] << ")\n";
		cout << "--> Dest: (" << dest->getPt()[0] << ", " << dest->getPt()[1] << ")\n";	
	}else if(!origin->getPt().empty() && dest->getPt().empty()){
		cout << "--> Origin: (" << origin->getPt()[0] << ", " << origin->getPt()[1] << ")\n";
		cout << "--> Dest: (---)\n";
	}else if(origin->getPt().empty() && !dest->getPt().empty()){
		cout << "--> Origin: (---)\n";
		cout << "--> Dest: (" << dest->getPt()[0] << ", " << dest->getPt()[1] << ")\n";
	}else{ //origin->getPt().empty() && dest->getPt().empty()
		cout << "--> Origin: (---)\n";
		cout << "--> Dest: (---)\n";	
	}*/
}

/* Returns empty new edge */
Edge* Edge::makeEdge(){
	return (new QuadEdge())->e;
}

/* Splices two edges together:
	- if aOrg and bOrg are distinct, combines into one
	- if aOrg and bOrg are exactly the same ring, breaks into 2 separate pieces
	- if aOrg and bOrg are same ring with opposite orientations, flip (and
	  reverse the order) of a segment of that ring.  
 */
void Edge::splice(Edge *a, Edge *b){
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
Edge* Edge::connect(Edge *a, Edge *b){
	Edge *e = makeEdge();
	e->setOrigin(a->getDest()); //TODO CHECK ALIASING HERE
	e->setDest(b->getOrigin());
	splice(e, a->lnext());
	splice(e->sym(), b);
	return e;
}

/* Disconnects edge e from the rest of the structure */
void Edge::deleteEdge(Edge *e){
	splice(e, e->oprev());
	splice(e->sym(), e->sym()->oprev());
}


