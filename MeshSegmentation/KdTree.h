
#ifndef __KDTREE_H_
#define __KDTREE_H_


#include "PriorityQueue.h"
#include "Vector3D.h"
#include "float.h"
#include <vector>

#define SQR(x) (x*x)

typedef MaxPriorityQueue<int, float> PQueue;
typedef PQueue::Element Neighbour;

typedef struct kdTreePoint {
	Vector3D			pos;
	int			index;
} KdTreePoint;


/**
 * abstract node class
 * base class for leaves and nodes
 *
 * @author Richard Keiser
 * @version 2.0
 */ 
class Node {
public:
	/**
	 * look for the nearest neighbours
	 * @param rd 
	 *		  the distance of the query position to the node box
	 * @param queryPriorityQueue
	 *		  a priority queue
	 */
	virtual void queryNode(float rd, PQueue* queryPriorityQueue) = 0;
};

/**
 * Node of a kd tree. The node stands for a box in space.
 *
 * @author Richard Keiser
 * @version 2.0
 */
class KdNode : public Node {
public:
	/**
	 * deletes the children of the node
	 */
	virtual ~KdNode() {
		delete[] m_children[0];
		delete[] m_children[1];
	}

	/**
	 * array of child nodes
	 */
	Node**			m_children;		
	/**
	 * cut value of the splitting plane
	 */
	float			m_cutVal;		
	/*
	 * actual dimension of this node
	 */
	unsigned char	m_dim;			

	/**
	 * look for the nearest neighbours
	 * @param rd 
	 *		  the distance of the query position to the node box
	 * @param queryPriorityQueue
	 *		  a priority queue
	 */
	void queryNode(float rd, PQueue* queryPriorityQueue);
};


/**
 * A leaf node of the tree (bucket node) containing the primitives
 *
 * @author Richard Keiser
 * @version 2.0
 */
class KdLeaf : public Node {

public:
	/**
	 * the primitives of this leaf
	 */
	KdTreePoint*	m_points;		
	/**
	 * the number of elements in this leaf
	 */
	unsigned int	m_nOfElements;

	/**
	 * look for the nearest neighbours
	 * @param rd 
	 *		  the distance of the query position to the node box
	 * @param queryPriorityQueue
	 *		  a priority queue
	 */
	void queryNode(float rd, PQueue* queryPriorityQueue);
};




/**
 * An efficient k-d tree for 3 dimensions
 * It is very similar to the k-d tree 
 * described by Arya and Mount in ``Algorithms
 *	for fast vector quantization,'' Proc.  of DCC '93: Data Compression
 *	Conference, eds. J. A. Storer and M. Cohn, IEEE Press, 1993, 381-390
 *  and their ANN software library
 *
 * @author Richard Keiser
 * @version 2.0
 */
class KdTree {

public:
	/**
	 * Creates a k-d tree from the positions
	 *
	 * @param positions
	 *			point positions
	 * @param nOfPositions
	 *			number of points
	 * @param maxBucketSize
	 *			number of points per bucket
	 */
	KdTree(const Vector3D *positions, const unsigned int nOfPositions, const unsigned int maxBucketSize);
	/**
	 * Destructor
	 */
	virtual ~KdTree();

	/**
	 * look for the nearest neighbours at <code>position</code>
	 *
	 * @param position
	 *			the position of the point to query with
	 */
	void queryPosition(const Vector3D &position);

	void visitKdTreeAndDeleteNodeInIt();
	/**
	 * look for the nearest neighbours with a maximal squared distance <code>maxSqrDistance</code>. 
	 * If the set number of neighbours is smaller than the number of neighbours at this maximum distance, 
	 * this call is equal to queryPosition but may be faster if the maximum distance is close to the distance 
	 * of the nearest neighbour with the largest distance.
	 *
	 * @param position
	 *			the position of the point to query with
	 * @param maxSqrDistance
	 *			the maximal squared distance of a nearest neighbour
	 *			
	 */
	void queryRange(const Vector3D &position, const float maxSqrDistance);
	/**
	 * set the number of nearest neighbours which have to be looked at for a query
	 *
	 * @params newNOfNeighbours
	 *			the number of nearest neighbours
	 */
	void setNOfNeighbours (const unsigned int newNOfNeighbours);
	/**
	 * get the index of the i-th nearest neighbour to the query point
	 * i must be smaller than the number of found neighbours
	 *
	 * @param i
	 *			index of the nearest neighbour
	 * @return the index of the i-th nearest neighbour
	 */
	inline unsigned int getNeighbourPositionIndex (const unsigned int i);
	/** 
	 * get the position of the i-th nearest neighbour
	 * i must be smaller than the number of found neighbours
	 *
	 * @param i
	 *			index of the nearest neighbour
	 * @return the position of the i-th nearest neighbour
	 */
	inline Vector3D getNeighbourPosition(const unsigned int i);
	/**
	 * get the squared distance of the query point and its i-th nearest neighbour
	 * i must be smaller than the number of found neighbours
	 *
	 * @param i
	 *			index of the nearest neighbour
	 * @return the squared distance to the i-th nearest neighbour
	 */
	inline float getSquaredDistance (const unsigned int i);
	/**
	 * get the number of found neighbours
	 * Generally, this is equal to the number of query neighbours
	 * except for range queries, where this number may be smaller than the number of query neigbhbours
	 *
	 * @return the number of found neighbours
	 */
	inline unsigned int getNOfFoundNeighbours();

	/**
	 * get the number of query neighbors
	 * Generally, this is equal to the number of found neighbours
	 * except for range queries, where this number may be larger than the number of found neigbhbours
	 *
	 * @return the number of querye neighbours
	 */
	inline unsigned int getNOfQueryNeighbours();

protected:
	/**
	 * compute distance from point to box
	 *
	 * @param q 
	 *		the point position
	 * @param lo
	 *		low point of box
	 * @param hi
	 *		high point of box
	 * @return the distance to the box
	 */
	float computeBoxDistance(const Vector3D &q, const Vector3D &lo, const Vector3D &hi);
	/**
	 * computes the enclosing box of the points
	 *
	 * @param lowCorner
	 *			the low corner of the enclosing box is returned
	 * @param hiCorner
	 *			the high corner of the enclosing box is returned
	 */
	void computeEnclosingBoundingBox(Vector3D &lowCorner, Vector3D &hiCorner);
	/** 
	 * creates the tree using the sliding midpoint splitting rule
	 * 
	 * @param node
	 *		  the node to split
	 * @param start
	 *		  first index in the data array
	 * @param end
	 *		  last index in the data array
	 * @param maximum
	 *		  maximum coordinates of the data points
	 * @param minimum
	 *		  minimum coordinates of the data points
	 */
	void createTree(KdNode &node, int start, int end, Vector3D maximum, Vector3D minimum);

	
private:
	
	KdTreePoint*				m_points;
	const Vector3D*				m_positions;
    std::vector<Neighbour>  	m_neighbours;
	int							m_bucketSize;
	KdNode*						m_root;
	unsigned int				m_nOfFoundNeighbours,
								m_nOfNeighbours,
								m_nOfPositions;
	PQueue*						m_queryPriorityQueue;
	Vector3D					m_boundingBoxLowCorner;
	Vector3D					m_boundingBoxHighCorner;

	// gets the minimum and maximum value of all points at dimension dim
	void getMinMax(KdTreePoint *points, int nOfPoints, int dim, float &min, float &max);
	// splits the points such that on return for all points:
	//		points[0..br1-1] < cutVal
	//		points[br1-1..br2-1] == cutVal
	//		points[br2..nOfPoints-1] > cutVal
	void splitAtMid(KdTreePoint *points, int nOfPoints, int dim, float cutVal, int &br1, int &br2);
	// get the axis aligned bounding box of points
	void getSpread(KdTreePoint* points, int nOfPoints, Vector3D &maximum, Vector3D &minimum);
};

inline unsigned int KdTree::getNOfFoundNeighbours() {
	return m_nOfFoundNeighbours;
}

inline unsigned int KdTree::getNOfQueryNeighbours() {
	return m_nOfNeighbours;
}

inline unsigned int KdTree::getNeighbourPositionIndex(const unsigned int neighbourIndex) {
	return m_neighbours[neighbourIndex].index;
}

inline Vector3D KdTree::getNeighbourPosition(const unsigned int neighbourIndex) {
	return m_positions[m_neighbours[neighbourIndex].index];
}

inline float KdTree::getSquaredDistance (const unsigned int neighbourIndex) {
	return m_neighbours[neighbourIndex].weight;
}

#endif

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End: