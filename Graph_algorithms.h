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
}