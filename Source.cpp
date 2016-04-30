#include <iostream>
#include <string>
#include "Graph.h"
using namespace std;

class Destroyer
{
private:
	int mVal;
public:
	Destroyer(int val)
		:mVal(val)
	{ }

	friend std::ostream& operator<<(std::ostream&, const Destroyer&);
};

class Hola
{
public:
	int ha;
};

std::ostream& operator<<(std::ostream& stream, const Destroyer& dst)
{
	stream << dst.mVal << " | ";
	return stream;
}

int main()
{
	Graph::Graph<std::string, int> graph;
	std::string a = "Brno";
	auto brno = graph.addVertex(a);
	auto praha = graph.addVertex("Praha");
	graph.addVertex("Breclav");
	auto all = graph.getVerticesValues();
	size_t breclav;
	for (auto & i : all) {
		if (i.second != "Brno" && i.second != "Praha") breclav = i.first;
	}
	cout << graph.listvertices() << endl;
	graph.addEdge(brno, praha, 33);
	graph.addEdge(praha, brno, 40);
	graph.addEdge(brno, breclav, 50);
	cout << graph.listedges();
	cout << "removing edge from Brno to Praha" << endl;
	graph.removeEdge(brno, praha);
	graph.removeEdge(brno, (size_t)10);
	cout << graph.listedges();
	cout << "adding edge from Breclav to Praha and back" << endl;
	graph.addEdge(breclav, praha, 100);
	graph.addEdge(praha, breclav, 120);
	cout << graph.listedges();
	cout << "Is there edge from Brno to Breclav: " << (graph.adjacent(brno, breclav) ? "true" : "false") << endl;;
	cout << "Is there edge from Brno to Praha: " << (graph.adjacent(brno, praha) ? "true" : "false") << endl;;
	cout << "Praha neighbours: ";
	auto gn = graph.getNeighbours(praha);
	for (auto & gni : gn) {
		cout << graph.getVertexValue(gni) << " ";
	}
	cout << endl;
	cout << "removing Breclav" << endl;
	graph.removeVertex(breclav);
	cout << graph.listedges();

	std::cout << "Edge value from Praha to Brno: " << graph.getEdgeValue(praha, brno) << std::endl;

	std::string newbrno = "BRNO";
	graph.setVertexValue(praha, "PRAHA");
	graph.setVertexValue(brno, newbrno);

	auto vals = graph.getVerticesValues();

	for(auto& val : vals)
	{
		std::cout << (val.first + 1) << ". " << val.second << std::endl;
	}

	cout << endl << endl;
	//test

	Graph::Graph<Hola> holag;
	holag.addVertex(Hola());
	holag.addVertex(Hola());

	// This actually works even with default constructible Vertex
	// but if using operator[idx] and item at idx does not exist, map will return default constructed value
	// but as it cannot be default constructed, error will be thrown
	// so I have tried to prevent this problem by moving from operator[] to insert/find
	// Ĺater we will need to check if index exist etc
	Graph::Graph<Destroyer> destroyerGraph(false);
	auto d4 = destroyerGraph.addVertex(Destroyer(4));
	auto d2 = destroyerGraph.addVertex(Destroyer(2));

	auto destVals = destroyerGraph.getVerticesValues();

	for(auto& a : destVals)
	{
		std::cout << (a.first+1) << "." << a.second;
	}

	std::cout << std::endl << destroyerGraph.listvertices() << std::endl;

	destroyerGraph.removeVertex(d4);

	std::cout << std::endl << destroyerGraph.listvertices() << std::endl;

	std::cout << "Destroyer(2) value: " << destroyerGraph.getVertexValue(d2) << std::endl;

	return 0;
}
