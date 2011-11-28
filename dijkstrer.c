/*
 *
 *  Created on: Nov 13, 2011
 *        Author : konmpar
 *       Contact : konmpar at gmail dot com
 *
 */

#include "mpich2/mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node node;
typedef struct _acne acne;
int myid, numprocs;

/*
 * There are two structures.
 * 'Node' represent each node having childrens acnes represent each connected to him nodes.
 * 'Acne' represent each acne connecting two Nodes.
 */

struct _node {
	int distance; // distance from the first node
	struct _acne *children[100]; // going to nodes
	int edgeno; // how many nodes are connected to
	int name;
};

struct _acne {
	int value; // distance between these edges
	int from; // acne connecting from
	int to; // to
	struct _node *edge;
};

// Get the node with the minimum distance so we can go on #Dijkstra's Algorithm
int getMinDistanceNode(struct _acne *array[], int size){
	int minnum=array[0]->edge->distance, minnode=0, j;

	for(j=0; j< size;j++){
		if(array[j]->edge->distance<minnum){
			minnode = j;
			minnum = array[j]->edge->distance;
		}
	}


	return minnode;
}

// Get the maximum number of childs connented to one node
int getMaxEdgeNo(struct _node array[], int size){
	int maxnum=array[0].edgeno, j;

	for(j=0; j< size;j++){
		if(array[j].edgeno > maxnum){
			maxnum = array[j].edgeno;
		}
	}


	return maxnum;
}

/*
 * For every node we are going in (the parameter), we search its connected childs nodes and
 * calculate their distances. then we are going in again( recursion ) in the node with the
 * minimum distance
 *
 * */

int traverse(struct _node *node){

	// the node with edgeno=0 is the last node
	if(node->edgeno<=0)
		return 0;

	if (myid == 0) {
		printf("start of node %d\n", node->name);
		fflush(stdout);

		printf("\tchildrens: %d\n", node->edgeno);
		fflush(stdout);

	}

	int i;

	// Since in every node, every process calculates each child, we don't want more processes that the number of childs of each node to be executed
	if(myid<node->edgeno){

		// Dijkstra's algorithm
		if(node->distance + node->children[myid]->value < node->children[myid]->edge->distance ){
			node->children[myid]->edge->distance = node->distance + node->children[myid]->value;
		}

		// Every process should broadcast her results to the others, so they can collaborate peacefully
		// Let the world know my results
		for (i = 0; i < node->edgeno; i++) {
			MPI_Bcast( &node->children[i]->edge->distance, 1, MPI_INT, i, MPI_COMM_WORLD );
		}

		printf("\tprocess id %d calculates node %d with distance %d\n", myid, node->children[myid]->edge->name, node->children[myid]->edge->distance);
		fflush(stdout);

	}

	if (myid == 0) {
		printf("end of node\n-----------------------\n");
		fflush(stdout);

		printf("\tminnode :%d\n", node->children[getMinDistanceNode(node->children, node->edgeno)]->edge->name);
		fflush(stdout);

	}

	// Called recursion
	traverse(node->children[getMinDistanceNode(node->children, node->edgeno)]->edge);

	return 0;
}

int main(int argc, char *argv[]){

	// MPI initialization
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	FILE *ifp;

	int from, to, distance;
	int nodesNo = 0;
	int edgesNo = 0;
	int acnesCtr = 0; // count the acnes
	int previous = 0;
	int childsCtr = 0; // count childs for every node
	char *file;

	if(argc==2)
		file = argv[argc-1];
	else
		file = "dijkstra.dat";

	ifp = fopen(file, "r");
	if (ifp == NULL) {
		fprintf(stderr, "Can't open input file!\n");
		MPI_Finalize();
		exit(1);
	}

	/*
	 * Files content should be in the format
	 * n N
	 * startNode endNode distance
	 * startNode endNode distance
	 * .
	 * .
	 * .
	 * startNode endNode distance
	 *
	 * where n is the total number of Nodes and N is the total number of Acnes
	 *
	 */

	// read the first line
	fscanf(ifp, "%d %d", &nodesNo, &edgesNo);

	struct _node nodes[nodesNo];
	struct _acne acnes[edgesNo];

	while (fscanf(ifp, "%d %d %d", &from, &to, &distance) == 3) { /* read from file */

		/*
		 * Then for every line we create a node and also
		 * an acne to place the connected edges.
		 *
		 */
		if (previous != from)
			childsCtr = 0;

		nodes[from].distance = 65000; // infinite
		nodes[from].name = from;

		acnes[acnesCtr].from = from;
		acnes[acnesCtr].to = to;
		acnes[acnesCtr].value = distance;
		acnes[acnesCtr].edge = &nodes[to];

		nodes[from].children[childsCtr] = &acnes[acnesCtr];

		nodes[from].edgeno = childsCtr + 1;

		acnesCtr++;
		childsCtr++;
		previous = from;
	}

	// set firsts node's distance
	nodes[0].distance = 0;

	// set last node
	nodes[nodesNo - 1].distance = 65000;
	nodes[nodesNo - 1].edgeno = 0;
	nodes[nodesNo - 1].name = nodesNo - 1;

	//
	int maxnum = getMaxEdgeNo(nodes, sizeof(nodes)/sizeof(*nodes));
	if(numprocs<maxnum){
		fprintf(stderr, "Number of processes should be greater that the maximum number of edges: %d!\n", maxnum);
		MPI_Finalize();
		exit(1);
	}

	// get in calculation
	traverse(&nodes[0]);

	// print out the results
	if(myid == 0){
		int j;
		for(j=0; j< (sizeof(nodes)/sizeof(*nodes));j++){
			printf("Node: %d, Distance from the root node: %d, Number of edges: %d\n", nodes[j].name, nodes[j].distance, nodes[j].edgeno);
			fflush(stdout);
		}
	}


	MPI_Finalize();

	return 0;
}

