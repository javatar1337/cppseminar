#include <iostream>
#include <string>

// NOTE By defining GRAPH_DEBUG before including Graph.h we also allow compilation of
//		custom debug functions like listVertices etc. => might be useful if some problems appear
//		and user wants to get as many informations as possible.
//		Also those codes are available only if define is here, otherwise no debug stuff is compiled
#define GRAPH_DEBUG
#include "Graph.h"

#include "Graph_algorithms.h"
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
	auto all = graph.getVerticesMap();
	size_t breclav;
	for (auto & i : all)
	{
		if (i.second != "Brno" && i.second != "Praha") breclav = i.first;
	}
	cout << graph.listVertices() << endl;
	graph.addEdge(brno, praha, 33);
	graph.addEdge(praha, brno, 40);
	graph.addEdge(brno, breclav, 50);
	cout << graph.listEdges();
	cout << "removing edge from Brno to Praha" << endl;
	graph.removeEdge(brno, praha);
	graph.removeEdge(brno, (size_t)10);
	cout << graph.listEdges();
	cout << "adding edge from Breclav to Praha and back" << endl;
	graph.addEdge(breclav, praha, 100);
	graph.addEdge(praha, breclav, 120);
	cout << graph.listEdges();
	cout << "Is there edge from Brno to Breclav: " << (graph.adjacent(brno, breclav) ? "true" : "false") << endl;;
	cout << "Is there edge from Brno to Praha: " << (graph.adjacent(brno, praha) ? "true" : "false") << endl;;
	cout << "Praha neighbours: ";
	auto gn = graph.getNeighbours(praha);
	for (auto & gni : gn)
	{
		cout << graph.getVertexValue(gni) << " ";
	}
	cout << endl;
	cout << "removing Breclav" << endl;
	graph.removeVertex(breclav);
	cout << graph.listEdges();
	cout << endl << "updating edge from praha to brno" << endl;
	graph.updateEdgeValue(praha, brno, 200);
	std::cout << "Edge value from Praha to Brno: " << graph.getEdgeValue(praha, brno) << std::endl;
	cout << endl << "updating edge from praha to brno" << endl;
	int upd = 222;
	graph.updateEdgeValue(praha, brno, upd);
	std::cout << "Edge value from Praha to Brno: " << graph.getEdgeValue(praha, brno) << std::endl;

	graph.updateEdgeValue(brno, praha, 313);
	cout << graph.listEdges() << endl;

	std::string newbrno = "BRNO";
	graph.setVertexValue(praha, "PRAHA");
	graph.setVertexValue(brno, newbrno);

	auto vals = graph.getVerticesMap();

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

	auto destVals = destroyerGraph.getVerticesMap();

	for(auto& a : destVals)
	{
		std::cout << (a.first+1) << "." << a.second;
	}

	std::cout << std::endl << destroyerGraph.listVertices() << std::endl;

	destroyerGraph.removeVertex(d4);

	std::cout << std::endl << destroyerGraph.listVertices() << std::endl;

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
	std::cout << triangleGraph.listVertices() << std::endl;
	std::cout << triangleGraph.listEdges() << std::endl;

	triangleGraph.saveToFile("triangleGraph.txt");

	Graph::Graph<std::string, int> triangleGraphLoaded;

	bool loadedFromFile = triangleGraphLoaded.loadFromFile("triangleGraph.txt");

	std::cout << "Loaded graph: " << loadedFromFile << std::endl;
	std::cout << triangleGraphLoaded.listVertices() << std::endl;
	std::cout << triangleGraphLoaded.listEdges() << std::endl;

	std::cout << "TRIANGLE GRAPH EDITED" << std::endl;

	auto fourth = triangleGraph.addVertex("pointD");

	triangleGraph.addEdge(fourth, first, 2);
	triangleGraph.addEdge(fourth, second, 2);
	triangleGraph.addEdge(fourth, third, 2);

	std::cout << "Original graph: " << std::endl;
	std::cout << triangleGraph.listVertices() << std::endl;
	std::cout << triangleGraph.listEdges() << std::endl;

	triangleGraph.saveToFile("triangleGraph.txt");
	triangleGraphLoaded.loadFromFile("triangleGraph.txt");

	std::cout << "Loaded graph: " << loadedFromFile << std::endl;
	std::cout << triangleGraphLoaded.listVertices() << std::endl;
	std::cout << triangleGraphLoaded.listEdges() << std::endl;

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
	std::cout << squareGraph.listVertices() << std::endl;
	std::cout << squareGraph.listEdges() << std::endl;

	squareGraph.saveToFile("squareGraph.txt");

	Graph::Graph<std::string> squareGraphLoaded(false);
	squareGraphLoaded.loadFromFile("squareGraph.txt");

	std::cout << "Loaded graph: " << std::endl;
	std::cout << squareGraphLoaded.listVertices() << std::endl;
	std::cout << squareGraphLoaded.listEdges() << std::endl;

	squareGraphLoaded.exportToDot("squareDot.txt");
	squareGraphLoaded.exportToDot("squareDotColoured.txt", { sqVertex2, sqVertex1, sqVertex3 });
	squareGraphLoaded.exportToDot("squareDotColoured2.txt", { { sqVertex2, sqVertex1 },  { sqVertex3, sqVertex4 }});

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

	for(auto a : undirWeightedGraph.getEdgesPositions())
	{
		std::cout << "EDGE " << a.first << ", " << a.second << std::endl;
	}

	for(auto a : undirWeightedGraph.getEdgesPositionsAndValues())
	{
		std::cout << "EDGE " << std::get<0>(a) << ", " << std::get<1>(a) << ", " << std::get<2>(a) <<  std::endl;
	}

	undirWeightedGraph.exportToDot("undirWeighted.txt");

	Graph::Graph<std::string, int> smallMapGraph;

	sqVertex1 = smallMapGraph.addVertex("Prague");
	sqVertex2 = smallMapGraph.addVertex("Brno");
	sqVertex3 = smallMapGraph.addVertex("Pardubice");

	smallMapGraph.addEdge(sqVertex1, sqVertex2, 205);
	smallMapGraph.addEdge(sqVertex1, sqVertex3, 124);
	smallMapGraph.addEdge(sqVertex2, sqVertex3, 145);

	smallMapGraph.exportToDot("smallMapDot.txt");


	//pokus
	Graph::Graph<int> example;

	for (int i = 0; i < 10; i++)
	{
		example.addVertex(i);
	}

	example.addEdge((size_t)0, (size_t)2);
	example.addEdge((size_t)0, (size_t)3);
	example.addEdge((size_t)0, (size_t)7);
	example.addEdge((size_t)2, (size_t)5);
	example.addEdge((size_t)2, (size_t)6);
	example.addEdge((size_t)2, (size_t)8);
	example.addEdge((size_t)3, (size_t)4);
	example.addEdge((size_t)3, (size_t)8);
	example.addEdge((size_t)4, (size_t)8);
	example.addEdge((size_t)5, (size_t)1);
	example.addEdge((size_t)6, (size_t)7);
	example.addEdge((size_t)7, (size_t)9);
	example.addEdge((size_t)8, (size_t)9);
	example.addEdge((size_t)9, (size_t)5);
	cout << "DFS" << endl;
	Graph::DFS(example, size_t(0), [](const int & a)
	{
		std::cout << a << std::endl;
	});
	cout << "BFS" << endl;
	Graph::BFS(example, size_t(0), [](const int & a)
	{
		std::cout << a << std::endl;
	});

	auto exampleEdges = example.getEdgesPositions(true);

	for(auto& edge : exampleEdges)
	{
		std::cout << edge.first << ", " << edge.second << std::endl;
	}

	std::cout << "BellmanFord" << std::endl;

	Graph::Graph<size_t, size_t> someGraphUnDir(false);
	std::vector<size_t> someGraphVertices;

	for(size_t i = 0; i < 10; ++i)
	{
		someGraphVertices.push_back(someGraphUnDir.addVertex(i));
	}

	for(size_t i = 0; i<10; ++i)
	{
		for(size_t j = 0; j<10; ++j)
		{
			if((i+j) % 5 == 0)
			{
				someGraphUnDir.addEdge(i, j, i+j);
			}
		}
	}

	someGraphUnDir.addEdge(someGraphVertices[0], someGraphVertices[2], 10);
	someGraphUnDir.addEdge(someGraphVertices[8], someGraphVertices[3], 9);

	someGraphUnDir.exportToDot("biggerGraph.txt");
	std::cout << bellmanFordShortestPath(someGraphUnDir, someGraphVertices[0], someGraphVertices[1]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphUnDir, someGraphVertices[0], someGraphVertices[8]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphUnDir, someGraphVertices[0], someGraphVertices[0]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphUnDir, someGraphVertices[6], someGraphVertices[1]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphUnDir, someGraphVertices[3], someGraphVertices[2]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphUnDir, someGraphVertices[7], someGraphVertices[3]) << std::endl;

	someGraphVertices.clear();

	Graph::Graph<size_t, size_t> someGraphDir;

	for(size_t i = 0; i < 10; ++i)
	{
		someGraphVertices.push_back(someGraphDir.addVertex(i));
	}

	for(size_t i = 0; i<10; ++i)
	{
		for(size_t j = 0; j<10; ++j)
		{
			if(i < j && (i+j) % 5 == 0)
			{
				someGraphDir.addEdge(i, j, i+j);
			}
		}
	}

	someGraphDir.addEdge(someGraphVertices[0], someGraphVertices[2], 10);
	someGraphDir.addEdge(someGraphVertices[8], someGraphVertices[3], 9);
	someGraphDir.addEdge(someGraphVertices[0], someGraphVertices[1], 8);
	someGraphDir.addEdge(someGraphVertices[1], someGraphVertices[0], 13);


	someGraphDir.exportToDot("biggerGraphDir.txt");
	std::cout << "dir" << std::endl;
	std::cout << bellmanFordShortestPath(someGraphDir, someGraphVertices[0], someGraphVertices[1]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphDir, someGraphVertices[0], someGraphVertices[8]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphDir, someGraphVertices[0], someGraphVertices[0]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphDir, someGraphVertices[6], someGraphVertices[1]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphDir, someGraphVertices[3], someGraphVertices[2]) << std::endl;
	std::cout << bellmanFordShortestPath(someGraphDir, someGraphVertices[7], someGraphVertices[3]) << std::endl;

	std::cout << "Shortest path vertices: " << std::endl;
	auto pathVerts = bellmanFordPathVertices(someGraphDir, someGraphVertices[0], someGraphVertices[7]);

	for(auto& v : pathVerts)
		std::cout << v << std::endl;
	std::cout << std::endl;

	std::cout << "dijkstra: " << std::endl;
	auto dykestra = Graph::dijkstra(someGraphDir, someGraphVertices[0], someGraphVertices[7]);
	std::cout << "value: "<< dykestra.first << std::endl;
	std::cout << "path: ";
	for (auto& v : dykestra.second)
		std::cout << v << std::endl;
	std::cout << "end dijkstra" << std::endl;

	std::cout << "dir" << std::endl;
	std::cout << Graph::dijkstra(someGraphDir, someGraphVertices[0], someGraphVertices[1]).first << std::endl;
	std::cout << Graph::dijkstra(someGraphDir, someGraphVertices[0], someGraphVertices[8]).first << std::endl;
	std::cout << Graph::dijkstra(someGraphDir, someGraphVertices[0], someGraphVertices[0]).first << std::endl;
	std::cout << Graph::dijkstra(someGraphDir, someGraphVertices[6], someGraphVertices[1]).first << std::endl;
	std::cout << Graph::dijkstra(someGraphDir, someGraphVertices[3], someGraphVertices[2]).first << std::endl;
	std::cout << "path begin: ";
	for (auto & x : Graph::dijkstra(someGraphDir, someGraphVertices[3], someGraphVertices[2]).second)
	{
		std::cout << x << std::endl;
	}
	std::cout << "path end" << std::endl;
	std::cout << Graph::dijkstra(someGraphDir, someGraphVertices[7], someGraphVertices[3]).first << std::endl;
	std::cout << "end" << std::endl;
	// Exports dot file with colored path from source to destination

	someGraphDir.exportToDot("biggerGraphDirColored.txt", pathVerts);

	pathVerts = bellmanFordPathVertices(someGraphUnDir, someGraphVertices[8], someGraphVertices[5]);
	for(auto& v : pathVerts)
		std::cout << v << std::endl;

	someGraphUnDir.exportToDot("biggerGraphUnDirColored.txt", pathVerts);

	example.exportToDot("exampleDot.txt");

	std::cout << "Union" << std::endl;
	Graph::UnionFind fnd({1, 2, 3, 4, 5, 6, 7, 8});

	for(size_t i = 1; i <= fnd.size(); ++i)
	{
		std::cout << i << ": " << fnd.find(i) << " | ";
	}
	std::cout << std::endl;

	std::cout << "Merge 1 & 3, then 2 & 4, then 4 & 3: " << std::endl;
	fnd.unionSets(1, 3);
	fnd.unionSets(2, 4);
	fnd.unionSets(4, 3);

	for(size_t i = 1; i <= fnd.size(); ++i)
	{
		std::cout << i << ": " << fnd.find(i) << " | ";
	}
	std::cout << std::endl;

	std::cout << "Merge 5 & 7, then 7 & 1, then 7 & 7: " << std::endl;
	fnd.unionSets(5, 7);
	fnd.unionSets(7, 1);
	fnd.unionSets(7, 7);

	for(size_t i = 1; i <= fnd.size(); ++i)
	{
		std::cout << i << ": " << fnd.find(i) << " | ";
	}
	std::cout << std::endl;

	std::cout << "Merge 6 & 8, then 7 & 6:" << std::endl;
	fnd.unionSets(6, 8);
	fnd.unionSets(7, 6);

	for(size_t i = 1; i <= fnd.size(); ++i)
	{
		std::cout << i << ": " << fnd.find(i) << " | ";
	}
	std::cout << std::endl;

	std::cout << "Kruskal" << std::endl;


	someGraphUnDir.addEdge(someGraphVertices[4], someGraphVertices[5], 5);
	auto kruskalOutput = kruskalMST(someGraphUnDir);

	for(auto& edge : kruskalOutput)
	{
		std::cout << edge.first << "-" << edge.second << std::endl;
	}

	someGraphUnDir.exportToDot("kruskalDefault.txt");
	someGraphUnDir.exportToDot("kruskalColoured.txt", kruskalOutput);

	// Separate block to prevent variable naming collision (and because I am lazy to create a new function for it)
	{
		Graph::Graph<std::string, size_t> kruskalTest(false);
		size_t a = kruskalTest.addVertex("A");
		size_t b = kruskalTest.addVertex("B");
		size_t c = kruskalTest.addVertex("C");
		size_t d = kruskalTest.addVertex("D");
		size_t e = kruskalTest.addVertex("E");
		size_t f = kruskalTest.addVertex("F");
		size_t g = kruskalTest.addVertex("G");

		kruskalTest.addEdge(a, b, 7);
		kruskalTest.addEdge(a, d, 5);
		kruskalTest.addEdge(b, d, 9);
		kruskalTest.addEdge(b, c, 8);
		kruskalTest.addEdge(c, e, 5);
		kruskalTest.addEdge(b, e, 7);
		kruskalTest.addEdge(d, e, 15);
		kruskalTest.addEdge(d, f, 6);
		kruskalTest.addEdge(f, e, 8);
		kruskalTest.addEdge(f, g, 11);
		kruskalTest.addEdge(e, g, 9);

		kruskalTest.exportToDot("kruskalTestNonColoured.txt");
		kruskalTest.exportToDot("kruskalTestColoured.txt", kruskalMST(kruskalTest));
	}
	return 0;
}
