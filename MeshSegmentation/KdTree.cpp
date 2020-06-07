// Title:   kdTree.cpp
// Created: Thu Sep 25 14:19:18 2003
// Authors: Richard Keiser, Oliver Knoll, Mark Pauly, Matthias Zwicker
//
// Copyright (c) 2001, 2002, 2003 Computer Graphics Lab, ETH Zurich
//
// This file is part of the Pointshop3D system.
// See http://www.pointshop3d.com/ for more information.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.
//
// Contact info@pointshop3d.com if any conditions of this
// licensing are not clear to you.
//
#include "StdAfx.h"
#include "kdTree.h"
#include <stdlib.h>

#define SWAP_POINTS(a,b) \
			KdTreePoint tmp = points[a];\
		    points[a] = points[b];\
		    points[b] = tmp;


// ******************
// global definitions
// ******************
float						g_queryOffsets[3];
Vector3D					g_queryPosition;

KdTree::KdTree(const Vector3D *positions, const unsigned int nOfPositions, const unsigned int maxBucketSize) {
	m_bucketSize			= maxBucketSize;
	m_positions				= positions;
	m_nOfPositions			= nOfPositions;
	m_points				= new KdTreePoint[nOfPositions];
	m_nOfFoundNeighbours	= 0;
	m_nOfNeighbours			= 0;
	m_queryPriorityQueue	= new PQueue();
	for (unsigned int i=0; i<nOfPositions; i++) {
		m_points[i].pos = positions[i];
		m_points[i].index = i;
	}
	computeEnclosingBoundingBox(m_boundingBoxLowCorner, m_boundingBoxHighCorner);
	m_root = new KdNode();
	
	Vector3D maximum, minimum;
	getSpread(m_points, nOfPositions, maximum, minimum);
	createTree(*m_root, 0, nOfPositions, maximum, minimum);
	setNOfNeighbours(1);
}


KdTree::~KdTree() {
	delete m_root;
	delete[] m_points;
	delete m_queryPriorityQueue;
}

void KdTree::computeEnclosingBoundingBox(Vector3D &lowCorner, Vector3D &hiCorner) {
	Vector3D tmp;
	hiCorner = lowCorner = m_points[0].pos;	
	for (unsigned int i=1; i<m_nOfPositions; i++) {
		tmp = m_positions[i];
		if (hiCorner[0] < tmp[0]) {
			hiCorner[0] = tmp[0];
		}
		else if (lowCorner[0] > tmp[0]) {
			lowCorner[0] = tmp[0];
		}
		if (hiCorner[1] < tmp[1]) {
			hiCorner[1] = tmp[1];
		}
		else if (lowCorner[1] > tmp[1]) {
			lowCorner[1] = tmp[1];
		}
		if (hiCorner[2] < tmp[2]) {
			hiCorner[2] = tmp[2];
		}
		else if (lowCorner[2] > tmp[2]) {
			lowCorner[2] = tmp[2];
		}
	}		 
}

float KdTree::computeBoxDistance(const Vector3D &q, const Vector3D &lo, const Vector3D &hi) {
	register float dist = 0.0;
	register float t;

	if (q[0] < lo[0]) {
		t = lo[0] - q[0];
		dist = t*t;
	}
	else if (q[0] > hi[0]) {
		t = q[0] - hi[0];
		dist = t*t;
	}
	if (q[1] < lo[1]) {
		t = lo[1] - q[1];
		dist += t*t;
	}
	else if (q[1] > hi[1]) {
		t = q[1] - hi[1];
		dist += t*t;
	}
	if (q[2] < lo[2]) {
		t = lo[2] - q[2];
		dist += t*t;
	}
	else if (q[2] > hi[2]) {
		t = q[2] - hi[2];
		dist += t*t;
	}

	return dist;
}

void KdTree::queryPosition(const Vector3D &position) {
	if (m_neighbours.size() == 0) {
		return;
	}
	g_queryOffsets[0] = g_queryOffsets[1] = g_queryOffsets[2] = 0.0;
	m_queryPriorityQueue->init();
	m_queryPriorityQueue->insert(-1, FLT_MAX);
	g_queryPosition = position;
	float dist = computeBoxDistance(position, m_boundingBoxLowCorner, m_boundingBoxHighCorner);
	m_root->queryNode(dist, m_queryPriorityQueue);
	
	if (m_queryPriorityQueue->getMax().index == -1) {
		m_queryPriorityQueue->removeMax();
	}

	m_nOfFoundNeighbours = m_queryPriorityQueue->getNofElements();

	for(int i=m_nOfFoundNeighbours-1; i>=0; i--) {
		m_neighbours[i] = m_queryPriorityQueue->getMax();
		m_queryPriorityQueue->removeMax();
	}
}

void KdTree::queryRange(const Vector3D &position, const float maxSqrDistance) {
	if (m_neighbours.size() == 0) {
		return;
	}
	g_queryOffsets[0] = g_queryOffsets[1] = g_queryOffsets[2] = 0.0;
	m_queryPriorityQueue->init();
	m_queryPriorityQueue->insert(-1, maxSqrDistance);
	g_queryPosition = position;

	float dist = computeBoxDistance(position, m_boundingBoxLowCorner, m_boundingBoxHighCorner);	
	m_root->queryNode(dist, m_queryPriorityQueue);

	if (m_queryPriorityQueue->getMax().index == -1) {
		m_queryPriorityQueue->removeMax();
	}

	m_nOfFoundNeighbours = m_queryPriorityQueue->getNofElements();

	for(int i=m_nOfFoundNeighbours-1; i>=0; i--) {
		m_neighbours[i] = m_queryPriorityQueue->getMax();
		m_queryPriorityQueue->removeMax();
	}
}

void KdTree::setNOfNeighbours (const unsigned int newNOfNeighbours) {
	if (newNOfNeighbours != m_nOfNeighbours) {
		m_nOfNeighbours = newNOfNeighbours;
		m_queryPriorityQueue->setSize(m_nOfNeighbours);
		m_nOfNeighbours = newNOfNeighbours;
		m_neighbours.resize(m_nOfNeighbours);
		m_nOfFoundNeighbours = 0;
	}
}


void KdTree::createTree(KdNode &node, int start, int end, Vector3D maximum, Vector3D minimum) {
	int	mid;

	int n = end-start;
	Vector3D diff;
	short dim;

	diff = maximum - minimum;
	// get longest axe
	if (diff[0] > diff[1]) {
		if (diff[0] > diff[2]) {
			dim = 0;	//x-axe is longest axe
		}
		else {
			dim = 2;	// z-axe is longest axe
		}
	}
	else {
		if (diff[1] > diff[2]) {
			dim = 1;	// y-axe is longest axe
		}
		else {
			dim = 2;	// z-axe is longest axe
		}
	}
	
	node.m_dim = dim;
	float bestCut = (maximum[dim]+minimum[dim])/2.0;
	float min, max;
	getMinMax(m_points+start, n, dim, min, max);	// find min/max coordinates
	if (bestCut < min)		// slide to min or max as needed
		node.m_cutVal = min;
    else if (bestCut > max)
		node.m_cutVal = max;
    else
		node.m_cutVal = bestCut;

    int br1, br2;
	splitAtMid(m_points+start, n, dim, node.m_cutVal, br1, br2);	// permute points accordingly

	if (bestCut < min) mid = start+1;
    else if (bestCut > max) mid = end-1;
    else if (br1 > n/2.0) mid = start+br1;
    else if (br2 < n/2.0) mid = start+br2;
    else mid = start + (n>>1);

	Node** childNodes = new Node*[2];					//memory leak
	node.m_children = childNodes;
	if (mid-start <= m_bucketSize) {
		// new leaf
		KdLeaf* leaf = new KdLeaf();					//memory leak
		node.m_children[0] = leaf;
		leaf->m_points = (m_points+start);
		leaf->m_nOfElements = mid-start;
	}
	else {
		// new node
		KdNode* childNode = new KdNode();				//memory leak
		node.m_children[0] = childNode;
		float oldMax = maximum[dim];
		maximum[dim] = node.m_cutVal;
		createTree(*childNode, start, mid, maximum, minimum);
		maximum[dim] = oldMax;
	}
	
	if (end-mid <= m_bucketSize) {
		// new leaf
		KdLeaf* leaf = new KdLeaf();					//memory leak
		node.m_children[1] = leaf;
		leaf->m_points = (m_points+mid);
		leaf->m_nOfElements = end-mid;
	}
	else {
		// new node
		minimum[dim] = node.m_cutVal;
		KdNode* childNode = new KdNode();				//memory leak
		node.m_children[1] = childNode;
		createTree(*childNode, mid, end, maximum, minimum);
	}
}

void KdTree::getSpread(KdTreePoint* points, int nOfPoints, Vector3D &maximum, Vector3D &minimum) {
	Vector3D pos = points->pos;
	maximum = Vector3D(pos[0], pos[1], pos[2]);
	minimum = Vector3D(pos[0], pos[1], pos[2]);
	points++;
	for (int i = 1; i < nOfPoints; i++) {
		pos = points->pos;
		if (pos[0] < minimum[0]) {
			minimum[0] = pos[0];
		}
		if (pos[0] > maximum[0]) {
			maximum[0] = pos[0];
		}
		if (pos[1] < minimum[1]) {
			minimum[1] = pos[1];
		}
		if (pos[1] > maximum[1]) {
			maximum[1] = pos[1];
		}
		if (pos[2] < minimum[2]) {
			minimum[2] = pos[2];
		}
		if (pos[2] > maximum[2]) {
			maximum[2] = pos[2];
		}
		points++;
	}
}

void KdTree::getMinMax(KdTreePoint *points, int nOfPoints, int dim, float &min, float &max) {
	min = points->pos[dim];
	max = points->pos[dim];
	points++;
	for (int i=1; i<nOfPoints; i++) {
		if (points->pos[dim] < min) {
			min = points->pos[dim];
		}
		else if (points->pos[dim] > max) {
			max = points->pos[dim];
		}
		points++;
	}
}


void KdTree::splitAtMid(KdTreePoint *points, int nOfPoints, int dim, float cutVal, int &br1, int &br2) {
    int l = 0;
    int r = nOfPoints-1;
    for(;;) {				// partition points[0..n-1] about the cut value
		while (l < nOfPoints && points[l].pos[dim] < cutVal) {
			l++;
		}
		while (r >= 0 && points[r].pos[dim] >= cutVal) {
			r--;
		}
		if (l > r) 
			break;
		SWAP_POINTS(l,r);
		l++; 
		r--;
    }
    br1 = l;			// now: points[0..br1-1] < cutVal <= points[br1..n-1]
    r = nOfPoints-1;
    for(;;) {				// partition points[br1..n-1] about cutVal
		while (l < nOfPoints && points[l].pos[dim] <= cutVal) { 
			l++;
		}
		while (r >= br1 && points[r].pos[dim] > cutVal) {
			r--;
		}
		if (l > r) 
			break;
		SWAP_POINTS(l,r);
		l++; 
		r--;
    }
    br2 = l;			// now: points[br1..br2-1] == cutVal < points[br2..n-1]
}

void KdNode::queryNode(float rd, PQueue* queryPriorityQueue) {
	register float old_off = g_queryOffsets[m_dim];
	register float new_off = g_queryPosition[m_dim] - m_cutVal;
	if (new_off < 0) {
		m_children[0]->queryNode(rd, queryPriorityQueue);
		rd = rd - SQR(old_off) + SQR(new_off);
		if (rd < queryPriorityQueue->getMaxWeight()) {
			g_queryOffsets[m_dim] = new_off;
			m_children[1]->queryNode(rd, queryPriorityQueue);
			g_queryOffsets[m_dim] = old_off;
		}
	}
	else {
		m_children[1]->queryNode(rd, queryPriorityQueue);
		rd = rd - SQR(old_off) + SQR(new_off);
		if (rd < queryPriorityQueue->getMaxWeight()) {
			g_queryOffsets[m_dim] = new_off;
			m_children[0]->queryNode(rd, queryPriorityQueue);
			g_queryOffsets[m_dim] = old_off;
		}
	}

}

void KdLeaf::queryNode(float rd, PQueue* queryPriorityQueue) {
	float sqrDist;
	//use pointer arithmetic to speed up the linear traversing
	KdTreePoint* point = m_points;
	for (register unsigned int i=0; i<m_nOfElements; i++) {
		sqrDist = (point->pos - g_queryPosition).getSquaredLength();
		if (sqrDist < queryPriorityQueue->getMaxWeight()) {
			queryPriorityQueue->insert(point->index, sqrDist);
		}
		point++;
	}		
}

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:





















