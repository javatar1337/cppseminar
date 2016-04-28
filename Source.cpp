#include <iostream>
#include <string>
#include "Graph.h"
using namespace std;

int main()
{
	cout << "Hello world!" << endl;
	Graph::Graph<int, int> graph;
	int a = 5;
	graph.addVertex(a);
	graph.addVertex(13);
	graph.addVertex(Graph::Vertex<int, int> (10));
	Graph::Vertex<int, int> v(15);
	graph.addVertex(v);
	graph.addVertex(13);
	cout << graph.listvertices() << endl;
	return 0;
}