#include <iostream>
#include <string>

// NOTE By defining GRAPH_DEBUG before including Graph.h we also allow compilation of
//		custom debug functions like listVertices etc. => might be useful if some problems appear
//		and user wants to get as many informations as possible.
//		Also those codes are available only if define is here, otherwise no debug stuff is compiled
#define GRAPH_DEBUG
#include "Graph.h"

#include "Graph_algorithms.h"

Graph::Graph<std::string, size_t> getGraph()
{
	Graph::Graph<std::string, size_t> graph(false);
	auto praha = graph.addVertex("Praha");
	auto brno = graph.addVertex("Brno");
	auto ostrava = graph.addVertex("Ostrava");
	auto pardubice = graph.addVertex("Pardubice");
	auto karlovyVary = graph.addVertex("Karlovy Vary");
	auto plzen = graph.addVertex("Plzen");
	auto most = graph.addVertex("Most");
	
	graph.addEdge(praha, brno, 205);
	graph.addEdge(brno, ostrava, 170);
	graph.addEdge(ostrava, pardubice, 233);
	graph.addEdge(praha, pardubice, 124);
	graph.addEdge(pardubice, brno, 147);
	graph.addEdge(praha, karlovyVary, 127);
	graph.addEdge(brno, karlovyVary, 334);
	graph.addEdge(plzen, brno, 395);
	graph.addEdge(plzen, praha, 94);
	graph.addEdge(most, praha, 100);
	graph.addEdge(most, pardubice, 205);
	
	return graph;
}

Graph::Graph<std::string, size_t> getDigraph()
{
	Graph::Graph<std::string, size_t> digraph;
	auto praha = digraph.addVertex("Praha");
	auto brno = digraph.addVertex("Brno");
	auto ostrava = digraph.addVertex("Ostrava");
	auto pardubice = digraph.addVertex("Pardubice");
	
	digraph.addEdge(praha, brno, 205);
	digraph.addEdge(brno, ostrava, 170);
	digraph.addEdge(ostrava, pardubice, 233);
	digraph.addEdge(praha, pardubice, 124);
	digraph.addEdge(pardubice, brno, 147);
	
	return digraph;
}

void basicGraphFunctions()
{
	Graph::Graph<std::string, size_t> graph(false);
	auto praha = graph.addVertex("Praha");
	auto brno = graph.addVertex("Brno");
	auto ostrava = graph.addVertex("Ostrava");
	auto pardubice = graph.addVertex("Pardubice");
	
	graph.addEdge(praha, brno, 205);
	graph.addEdge(brno, ostrava, 170);
	graph.addEdge(ostrava, pardubice, 233);
	graph.addEdge(praha, pardubice, 124);
	graph.addEdge(pardubice, brno, 147);
	
	std::cout << "-- UNDIR GRAPH --" << std::endl;
	graph.listVerticesToStream();
	graph.listEdgesToStream();
	
	graph.removeEdge(brno, praha);
	std::cout << "-- UNDIR GRAPH (edge Praha-Brno removed) --" << std::endl;
	graph.listVerticesToStream();
	graph.listEdgesToStream();
	
	Graph::Graph<std::string, size_t> digraph;
	praha = digraph.addVertex("Praha");
	brno = digraph.addVertex("Brno");
	ostrava = digraph.addVertex("Ostrava");
	pardubice = digraph.addVertex("Pardubice");
	
	digraph.addEdge(praha, brno, 205);
	digraph.addEdge(brno, ostrava, 170);
	digraph.addEdge(ostrava, pardubice, 233);
	digraph.addEdge(praha, pardubice, 124);
	digraph.addEdge(pardubice, brno, 147);
	
	std::cout << "-- DIGRAPH --" << std::endl;
	digraph.listVerticesToStream();
	digraph.listEdgesToStream();
	
	digraph.removeVertex(pardubice);
	std::cout << "-- DIGRAPH (Pardubice removed) --" << std::endl;
	digraph.listVerticesToStream();
	digraph.listEdgesToStream();
	
	std::cout << "Value between Praha and Brno: " << digraph.getEdgeValue(praha, brno) << std::endl;
	digraph.updateEdgeValue(praha, brno, 206);
	std::cout << "Value between Praha and Brno updated to 206." << std::endl;
	std::cout << "Value between Praha and Brno: " << digraph.getEdgeValue(praha, brno) << std::endl;
}

void basicFileOperations()
{
	std::cout << "\n-- FILE OPERATIONS --" << std::endl;
	
	auto graph = getGraph();
	bool result = graph.saveToFile("baseGraph.txt");
	std::cout << "Graph was saved to file 'baseGraph.txt'? " << result << std::endl;
	
	Graph::Graph<std::string, size_t> graphLoaded(false);
	result = graphLoaded.loadFromFile("baseGraph.txt");
	std::cout << "Graph was loaded from file 'baseGraph.txt'? " << result << std::endl;
	
	std::cout << "Graphs are equal? " << (graph == graphLoaded) << std::endl;
	
	auto digraph = getDigraph();
	result = digraph.saveToFile("baseDiGraph.txt");
	std::cout << "Digraph was saved to file 'baseDiGraph.txt'? " << result << std::endl;
	
	Graph::Graph<std::string, size_t> digraphLoaded;
	result = digraphLoaded.loadFromFile("baseDiGraph.txt");
	std::cout << "Digraph was loaded from file 'baseDiGraph.txt'? " << result << std::endl;
	
	std::cout << "Digraphs are equal? " << (digraph == digraphLoaded) << std::endl;
	
	result = graph.exportToDot("baseDot.txt");
	std::cout << "Graph was exported to 'baseDot.txt'? " << result << std::endl;
	
	result = digraph.exportToDot("baseDiDot.txt");
	std::cout << "Digraph was exported to 'baseDiDot.txt'? " << result << std::endl;
}

void bfsDfsOperations()
{
	Graph::Graph<std::string, size_t> graph(false);
	auto praha = graph.addVertex("Praha");
	auto brno = graph.addVertex("Brno");
	auto ostrava = graph.addVertex("Ostrava");
	auto pardubice = graph.addVertex("Pardubice");
	auto karlovyVary = graph.addVertex("Karlovy Vary");
	
	graph.addEdge(praha, brno, 205);
	graph.addEdge(brno, ostrava, 170);
	graph.addEdge(ostrava, pardubice, 233);
	graph.addEdge(praha, pardubice, 124);
	graph.addEdge(pardubice, brno, 147);
	graph.addEdge(praha, karlovyVary, 127);
	
	std::cout << "-- BFS --" << std::endl;
	BFS(graph, pardubice, [](auto vertex)
	{
		std::cout << "Vertex: " << vertex << std::endl;
	});
	
	std::cout << "-- DFS --" << std::endl;
	DFS(graph, pardubice, [](auto vertex)
	{
		std::cout << "Vertex: " << vertex << std::endl;
	}, [](auto vertex)
	{
		return;
	});
	std::cout << "hh" << std::endl;
	Graph::Graph<std::string, size_t> graph2(false);
	auto vv1 = graph2.addVertex("0");
	auto vv2 = graph2.addVertex("1");
	auto vv3 = graph2.addVertex("2");
	auto vv4 = graph2.addVertex("3");
	auto vv5 = graph2.addVertex("4");
	auto vv6 = graph2.addVertex("5");
	auto vv7 = graph2.addVertex("6");
	graph2.addEdge(vv1, vv2, 1);
	graph2.addEdge(vv1, vv3, 1);
	graph2.addEdge(vv1, vv4, 1);
	graph2.addEdge(vv2, vv7, 1);
	graph2.addEdge(vv4, vv5, 1);
	graph2.addEdge(vv5, vv6, 1);
	graph2.addEdge(vv7, vv6, 1);
	DFS(graph2, vv1
		, [](auto vertex)
	{
		return;
	}
		, [](auto vertex)
	{
		std::cout << "Vertex: " << vertex << std::endl;
	}	
	);
}

void shortestPaths()
{
	std::cout << "-- SHORTEST PATH --" << std::endl;
	
	Graph::Graph<std::string, size_t> graph(false);
	auto praha = graph.addVertex("Praha");
	auto brno = graph.addVertex("Brno");
	auto ostrava = graph.addVertex("Ostrava");
	auto pardubice = graph.addVertex("Pardubice");
	auto karlovyVary = graph.addVertex("Karlovy Vary");
	auto plzen = graph.addVertex("Plzen");
	auto most = graph.addVertex("Most");
	
	graph.addEdge(praha, brno, 205);
	graph.addEdge(brno, ostrava, 170);
	graph.addEdge(ostrava, pardubice, 233);
	graph.addEdge(praha, pardubice, 124);
	graph.addEdge(pardubice, brno, 147);
	graph.addEdge(praha, karlovyVary, 127);
	graph.addEdge(brno, karlovyVary, 334);
	graph.addEdge(plzen, brno, 395);
	graph.addEdge(plzen, praha, 94);
	graph.addEdge(most, praha, 100);
	graph.addEdge(most, pardubice, 205);
	
	Graph::Graph<std::string, size_t> graph2(false);
	auto vvv1 = graph2.addVertex("1");
	auto vvv2 = graph2.addVertex("2");
	auto vvv3 = graph2.addVertex("3");
	auto vvv4 = graph2.addVertex("4");

	graph2.addEdge(vvv1, vvv2, 3);
	graph2.addEdge(vvv1, vvv3, 1);
	graph2.addEdge(vvv2, vvv4, 1);
	graph2.addEdge(vvv3, vvv4, 2);

	std::cout << "Default graph exported to 'shortestPathDot.txt'? " << graph.exportToDot("shortestPathDot.txt") << std::endl;
	
	std::cout << "Dijkstra: Most to Plzen? " << dijkstra(graph, most, plzen).first << std::endl;
	std::cout << "Dijkstra: 1 to 4? " << dijkstra(graph2, vvv1, vvv4).first << std::endl;
	std::cout << "Bellman-Ford: Most to Plzen? " << bellmanFordShortestPath(graph, most, plzen) << std::endl;
	
	std::cout << "Colored graph exported to 'shortestPathColoredDot.txt'? " << 
	graph.exportToDot("shortestPathColoredDot.txt", bellmanFordPathVertices(graph, most, plzen)) << std::endl;
}

void mstOperations()
{
	std::cout << "-- MST --" << std::endl;
	
	auto graph = getGraph();
	
	std::cout << "Base graph is in 'baseDot.txt'." << std::endl;
	auto kruskalVct = kruskalMST(graph);
	auto primSet = prim(graph);
	
	std::cout << "Kruskal: " << std::endl;
	size_t sum = 0;
	for(auto& a : kruskalVct)
	{
		std::cout << a.first << " - " << a.second << std::endl;
		sum += graph.getEdgeValue(a.first, a.second);
	}
	std::cout << sum << std::endl;
	
	std::cout << "Prim: " << std::endl;
	sum = 0;
	for(auto& a : primSet)
	{
		std::cout << a.first << " - " << a.second << std::endl;
		sum += graph.getEdgeValue(a.first, a.second);
	}
	std::cout << sum << std::endl;
	
	std::cout << "Colored graph saved in 'baseColoredDot.txt'? " << graph.exportToDot("baseColoredDot.txt", kruskalVct) << std::endl;
}

void flowOperation()
{
	std::cout << "-- MAX FLOW --" << std::endl;
	Graph::Graph<char, size_t> flowGraph;
	auto a = flowGraph.addVertex('A');
	auto b = flowGraph.addVertex('B');
	auto c = flowGraph.addVertex('C');
	auto d = flowGraph.addVertex('D');
	auto s = flowGraph.addVertex('S');
	auto t = flowGraph.addVertex('T');

	flowGraph.addEdge(s,a,10);
	flowGraph.addEdge(s,b,10);
	flowGraph.addEdge(a,b,2);
	flowGraph.addEdge(a,d,8);
	flowGraph.addEdge(b,d,9);
	flowGraph.addEdge(a,c,4);
	flowGraph.addEdge(d,c,6);
	flowGraph.addEdge(d,t,10);
	flowGraph.addEdge(c,t,10);

	std::cout << "Max flow (should be 19): " << edmondsKarpMaxFlow(flowGraph, s, t).first << std::endl;
}

// Just a simple main to show some of the core functionality
int main()
{
	basicGraphFunctions();
	basicFileOperations();
	bfsDfsOperations();
	shortestPaths();
	mstOperations();
	flowOperation();
	
	return 0;
}