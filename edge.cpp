// Edge class
#include <iostream>
#include "edge.h"

using namespace std;

//*************** Edge Functions ****************//

Edge::Edge(int idx, Edge *next){
	this.idx = idx;
	this.next = next;
}

/* Returns dual of edge in CCW order.
 *  (e,r) rot = (e, r+1)
 */
Edge* Edge::rot(){
	nextIdx = (this.idx+1)%4;
	if (nextIdx == 0) {
		return this - 3;
	}else{
		return this + 1;
	}
}

/* (e,r) Rot^-1 = (e,r+3)
 */
Edge* Edge::invrot(){
	nextIdx = (this.idx-1)%4;
	if (nextIdx == 3) {
		return this + 3;
	} else {
		return this - 1;
	}
}

/* Returns next clockwise (CW) edge from origin of this edge
 * (e,r) Onext = e[r].next
 */
Edge* Edge::onext(){
	return next;
}

/* Returns next counter-clockwise (CCW) edge from origin of this edge
 * (e,r) Oprev = (e[r+1].next) Rot
 */
Edge* Edge::oprev(){
	return this.rot()->onext()->rot();
}

/* (e,r) Sym = (e,r+2)
 */
Edge* Edge::sym(){
	if (this.idx < 2) {
		return this + 2;
	} else {
		return this - 2;
	}
}

