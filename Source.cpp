#include <iostream>
#include <string>
#include "Graph.h"
using namespace std;

int main()
{
	Graph::Graph<std::string, int> graph;
	std::string a = "Brno";
	auto brno = graph.addVertex(a);
	auto praha = graph.addVertex("Praha");
	graph.addVertex("Breclav");
	auto all = graph.getVertices();
	Graph::Vertex<std::string, int> breclav;
	for (auto & i : all) {
		if (i.second.getValue() != "Brno" && i.second.getValue() != "Praha") breclav = i.second;
	}
	cout << graph.listvertices() << endl;
	graph.addEdge(brno, praha, 33);
	graph.addEdge(praha, brno, 40);
	graph.addEdge(brno, breclav.getId(), 50);
	cout << graph.listedges();
	cout << "removing edge from Brno to Praha" << endl;
	graph.removeEdge(brno, praha);
	cout << graph.listedges();
	cout << "adding edge from Breclav to Praha and back" << endl;
	graph.addEdge(breclav.getId(), praha, 100);
	graph.addEdge(praha, breclav.getId(), 120);
	cout << graph.listedges();
	cout << "removing breclav" << endl;
	graph.removeVertex(breclav.getId());
    cout << graph.listedges();

	return 0;
}
