#pragma once

#include "Graph.h"
#include <stack>
#include <queue>
#include <map>

namespace Graph
{
	/**
	 * Depth-first search algorithm
	 * @param graph graph
	 * @param starting_vertex vertex to start search from, must be part of graph
	 * @param f unary function
	 */
	template<typename V, typename E, typename UnaryFunction>
	void DFS(Graph<V, E> graph, size_t starting_vertex, UnaryFunction f)
	{
		std::map<size_t, bool> discovered;
		auto vertmap = graph.getVerticesMap();
		if (vertmap.find(starting_vertex) == vertmap.end())
		{
			return;
		}
		for (auto & ver : graph.getVerticesValues())
		{
			discovered.insert({ ver.first, false });
		}
		std::stack<size_t> vertex_stack;
		vertex_stack.push(starting_vertex);
		while (!vertex_stack.empty())
		{
			size_t v = vertex_stack.top();
			vertex_stack.pop();
			if (!discovered.find(v)->second)
			{
				discovered.find(v)->second = true;
				f(graph.getVertexValue(v));
				for (auto & edd : graph.getNeighbours(v))
				{
					vertex_stack.push(edd);
				}
			}
		}
		return;
	}

	/**
	* Breadth-first search algorithm
	* @param graph graph
	* @param starting_vertex vertex to start search from, must be part of graph
	* @param f unary function
	*/
	template<typename V, typename E, typename UnaryFunction>
	void BFS(Graph<V, E> graph, size_t starting_vertex, UnaryFunction f)
	{
		std::map<size_t, bool> discovered;
		auto vertmap = graph.getVerticesMap();
		if (vertmap.find(starting_vertex) == vertmap.end())
		{
			return;
		}
		for (auto & ver : graph.getVerticesValues())
		{
			discovered.insert({ ver.first, false });
		}
		std::queue<size_t> vertex_queue;
		vertex_queue.push(starting_vertex);
		discovered.find(starting_vertex)->second = true;
		while (!vertex_queue.empty())
		{
			size_t v = vertex_queue.front();
			vertex_queue.pop();
			f(graph.getVertexValue(v));
			for (auto & edd : graph.getNeighbours(v))
			{
				if (!discovered.find(edd)->second)
				{
					vertex_queue.push(edd);
					discovered.find(edd)->second = true;
				}
			}
		}
		return;
	}
	
	/**
	 * @brief Bellman-Ford shortest path algorithm
	 * @param graph graph to find shortest paths in
	 * @param startVertex source vertex
	 * @param infinity infinity value
	 * @return pair of maps of <distances of each vertex from source vertex (infinity if no path exists) AND predecessors>
	 */
	template<typename V, typename E>
	std::pair<std::map<size_t, E>, std::map<size_t, size_t>> bellmanFord(const Graph<V,E>& graph, size_t startVertex, E infinity = std::numeric_limits<E>::max())
	{
		std::map<size_t, E> distance = graph.template getVerticesIdsMap<E>();
		std::map<size_t, size_t> predecessors = graph.template getVerticesIdsMap<size_t>();
		auto graphEdges = graph.getEdgesPositions();
		
		for(auto& d : distance)
		{
			d.second = infinity;
		}
		
		for(auto& p : predecessors)
		{
			p.second = p.first;
		}
		
		distance.at(startVertex) = E();
		
		for(size_t i = 0; i < graph.getVerticesCount() - 1; ++i)
		{
			for(auto& edge : graphEdges)
			{
				if(distance.at(edge.first) != infinity &&
					distance.at(edge.first) + graph.getEdgeValue(edge.first, edge.second) < distance.at(edge.second))
				{
					distance.at(edge.second) = distance.at(edge.first) + graph.getEdgeValue(edge.first, edge.second);
					predecessors.at(edge.second) = edge.first;
				}
			}
		}
		
		for(auto& edge : graphEdges)
		{
			if(distance.at(edge.first) != infinity && 
				distance.at(edge.first) + graph.getEdgeValue(edge.first, edge.second) < distance.at(edge.second))
			{
				throw std::invalid_argument("Graph contains cycle of negative weight!");
			}
		}
		
		return { distance, predecessors };
	}
	
	/**
	 * @brief Bellman-Ford shortest path algorithm
	 * @param graph graph to find shortest path in
	 * @param startVertex source vertex
	 * @param endVertex end vertex
	 * @param infinity infinity value
	 * @return length of shortest path
	 */
	template<typename V, typename E>
	E bellmanFordShortestPath(const Graph<V,E>& graph, size_t startVertex, size_t endVertex, E infinity = std::numeric_limits<E>::max())
	{
		return bellmanFord(graph, startVertex, infinity).first.at(endVertex);
	}
	
	/**
	 * @brief Bellman-Ford shortest path algorithm
	 * @param graph graph to find shortest path in
	 * @param startVertex source vertex
	 * @param endVertex end vertex
	 * @param infinity infinity value
	 * @return vector of vertices ids of path ordered from start vertex to end
	 */
	template<typename V, typename E>
	std::vector<size_t> bellmanFordPathVertices(const Graph<V,E>& graph, size_t startVertex, size_t endVertex, E infinity = std::numeric_limits<E>::max())
	{
		auto destination = bellmanFord(graph, startVertex, infinity);
		std::vector<size_t> result;
		
		if(destination.first.at(endVertex) == infinity) { return result; }
		
		size_t currVertex = endVertex;
		while(currVertex != startVertex)
		{
			result.push_back(currVertex);
			currVertex = destination.second.at(currVertex);
		}
		result.push_back(startVertex);
		
		return std::vector<size_t> { result.rbegin(), result.rend() };
	}
	
	template<typename V, typename>
	std::map<size_t, Unweight> bellmanFord(const Graph<V,Unweight>&, size_t, Unweight = Unweight()) = delete;
	
	template<typename V, typename>
	Unweight bellmanFordShortestPath(const Graph<V,Unweight>&, size_t, size_t, Unweight = Unweight()) = delete;
	
	template<typename V, typename>
	std::vector<size_t> bellmanFordPathVertices(const Graph<V,Unweight>&, size_t, size_t, Unweight = Unweight()) = delete;
}