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
	//auto all = graph.getVertices();
	/*Graph::Vertex<std::string, int> breclav;
	for (auto & i : all) {
		if (i.second.getValue() != "Brno" && i.second.getValue() != "Praha") breclav = i.second;
	}*/
	cout << graph.listvertices() << endl;
	graph.addEdge(brno, praha, 33);
	graph.addEdge(praha, brno, 40);
	//graph.addEdge(brno, breclav.getId(), 50);
	cout << graph.listedges();
	cout << "removing edge from Brno to Praha" << endl;
	graph.removeEdge(brno, praha);
	cout << graph.listedges();
	/*cout << "adding edge from Breclav to Praha and back" << endl;
	graph.addEdge(breclav.getId(), praha, 100);
	graph.addEdge(praha, breclav.getId(), 120);
	cout << graph.listedges();
	cout << "removing Breclav" << endl;
	graph.removeVertex(breclav.getId());
	cout << graph.listedges();*/

	std::cout << "Edge value from Praha to Brno: " << graph.getEdgeValue(praha, brno) << std::endl;

	auto vals = graph.getVerticesValues();

	for(auto& val : vals)
	{
		std::cout << (val.first + 1) << ". " << val.second << std::endl;
	}

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

	std::cout << "--FILE SAVE/LOAD--" << std::endl;

	Graph::Graph<std::string, int> triangleGraph;

	auto first = triangleGraph.addVertex("pointA");
	auto second = triangleGraph.addVertex("pointB");
	auto third = triangleGraph.addVertex("pointC");

	triangleGraph.addEdge(first, second, 5);
	triangleGraph.addEdge(second, third, 4);
	triangleGraph.addEdge(third, first, 3);

	std::cout << "Original graph: " << std::endl;
	std::cout << triangleGraph.listvertices() << std::endl;
	std::cout << triangleGraph.listedges() << std::endl;

	triangleGraph.saveToFile("triangleGraph.txt");

	Graph::Graph<std::string, int> triangleGraphLoaded;

	bool loadedFromFile = triangleGraphLoaded.loadFromFile("triangleGraph.txt");

	std::cout << "Loaded graph: " << loadedFromFile << std::endl;
	std::cout << triangleGraphLoaded.listvertices() << std::endl;
	std::cout << triangleGraphLoaded.listedges() << std::endl;

	std::cout << "TRIANGLE GRAPH EDITED" << std::endl;

	auto fourth = triangleGraph.addVertex("pointD");

	triangleGraph.addEdge(fourth, first, 2);
	triangleGraph.addEdge(fourth, second, 2);
	triangleGraph.addEdge(fourth, third, 2);

	std::cout << "Original graph: " << std::endl;
	std::cout << triangleGraph.listvertices() << std::endl;
	std::cout << triangleGraph.listedges() << std::endl;

	triangleGraph.saveToFile("triangleGraph.txt");
	triangleGraphLoaded.loadFromFile("triangleGraph.txt");

	std::cout << "Loaded graph: " << loadedFromFile << std::endl;
	std::cout << triangleGraphLoaded.listvertices() << std::endl;
	std::cout << triangleGraphLoaded.listedges() << std::endl;

	Graph::Graph<std::string> squareGraph(false);
	auto sqVertex1 = squareGraph.addVertex("vrtx1");
	auto sqVertex2 = squareGraph.addVertex("vrtx2");
	auto sqVertex3 = squareGraph.addVertex("vrtx3");
	auto sqVertex4 = squareGraph.addVertex("vrtx4");

	squareGraph.addEdge(sqVertex1, sqVertex2);
	squareGraph.addEdge(sqVertex2, sqVertex3);
	squareGraph.addEdge(sqVertex3, sqVertex4);
	squareGraph.addEdge(sqVertex4, sqVertex1);
	squareGraph.addEdge(sqVertex1, sqVertex3);
	squareGraph.addEdge(sqVertex2, sqVertex4);

	std::cout << "Original graph: " << std::endl;
	std::cout << squareGraph.listvertices() << std::endl;
	std::cout << squareGraph.listedges() << std::endl;

	squareGraph.saveToFile("squareGraph.txt");

	Graph::Graph<std::string> squareGraphLoaded(false);
	squareGraphLoaded.loadFromFile("squareGraph.txt");

	std::cout << "Loaded graph: " << std::endl;
	std::cout << squareGraphLoaded.listvertices() << std::endl;
	std::cout << squareGraphLoaded.listedges() << std::endl;

	squareGraphLoaded.exportToDot("squareDot.txt");

	Graph::Graph<std::string> dirUnweightedGraph;

	first = dirUnweightedGraph.addVertex("first");
	second = dirUnweightedGraph.addVertex("second");
	third = dirUnweightedGraph.addVertex("third");

	dirUnweightedGraph.addEdge(first, second);
	dirUnweightedGraph.addEdge(first, third);
	dirUnweightedGraph.addEdge(second, third);

	dirUnweightedGraph.exportToDot("triangleDot.txt");

	triangleGraph.exportToDot("triangleWeightedDot.txt");

	Graph::Graph<std::string, int> undirWeightedGraph(false);

	first = undirWeightedGraph.addVertex("oneVertex");
	second = undirWeightedGraph.addVertex("anotherVertex");

	undirWeightedGraph.addEdge(first, second, 5);

	undirWeightedGraph.exportToDot("undirWeighted.txt");

	Graph::Graph<std::string, int> smallMapGraph;

	sqVertex1 = smallMapGraph.addVertex("Prague");
	sqVertex2 = smallMapGraph.addVertex("Brno");
	sqVertex3 = smallMapGraph.addVertex("Pardubice");

	smallMapGraph.addEdge(sqVertex1, sqVertex2, 205);
	smallMapGraph.addEdge(sqVertex1, sqVertex3, 124);
	smallMapGraph.addEdge(sqVertex2, sqVertex3, 145);

	smallMapGraph.exportToDot("smallMapDot.txt");

	return 0;
}
