#pragma once

#include "Graph.h"
#include <stack>
#include <queue>
#include <map>
#include <set>

namespace Graph
{
	/**
	 * Union-find data structure
	 */
	class UnionFind
	{
	private:
		std::map<size_t, size_t> mVertices;
		std::map<size_t, size_t> mSetSizes;
	public:
		UnionFind(const std::vector<size_t>& vertices)
		{
			for(auto& a : vertices)
			{
				mVertices.insert({a, a});
				mSetSizes.insert({a, 1});
			}
		}

		template<typename T>
		UnionFind(const std::map<size_t, T>& verticesMap)
		{
			std::vector<size_t> vertices;

			for(auto& v : verticesMap)
			{
				vertices.push_back(v.first);
			}

			for(auto& a : vertices)
			{
				mVertices.insert({a, a});
				mSetSizes.insert({a, 1});
			}
		}

		size_t find(size_t item)
		{
			size_t newSize = 1;
			auto it = mVertices.find(item);

			if(it == mVertices.end())
			{
				throw std::invalid_argument("Requested item does not exist.");
			}

			size_t parent = it->second;

			while(parent != mVertices.find(parent)->second)
			{
				parent = mVertices.find(parent)->second;
				++newSize;
			}

			mSetSizes.at(item) = newSize;
			return parent;
		}

		void unionSets(size_t first, size_t second)
		{
			size_t firstRoot = find(first);
			size_t secondRoot = find(second);

			if(mSetSizes.at(first) < mSetSizes.at(second))
			{
				mVertices.find(firstRoot)->second = secondRoot;
				mSetSizes.at(secondRoot) = mSetSizes.at(firstRoot);
			}
			else
			{
				mVertices.find(secondRoot)->second = firstRoot;
				mSetSizes.at(firstRoot) = mSetSizes.at(secondRoot);
			}
		}

		size_t size() const
		{
			return mVertices.size();
		}
	};


	/**
	* Depth-first search algorithm
	* @param graph graph
	* @param starting_vertex vertex to start search from, must be part of graph
	* @param f unary function
	*/
	template<typename V, typename E, typename UnaryFunction>
	void DFS(Graph<V, E> & graph, size_t starting_vertex, UnaryFunction f)
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
	void BFS(Graph<V, E> & graph, size_t starting_vertex, UnaryFunction f)
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

		if(destination.first.at(endVertex) == infinity)
		{
			return result;
		}

		size_t currVertex = endVertex;
		while(currVertex != startVertex)
		{
			result.push_back(currVertex);
			currVertex = destination.second.at(currVertex);
		}
		result.push_back(startVertex);

		return std::vector<size_t> { result.rbegin(), result.rend() };
	}

	template<typename V>
	std::map<size_t, Unweight> bellmanFord(const Graph<V,Unweight>&, size_t, Unweight = Unweight()) = delete;

	template<typename V>
	Unweight bellmanFordShortestPath(const Graph<V,Unweight>&, size_t, size_t, Unweight = Unweight()) = delete;

	template<typename V>
	std::vector<size_t> bellmanFordPathVertices(const Graph<V,Unweight>&, size_t, size_t, Unweight = Unweight()) = delete;

	/**
	 * @brief Kruskal algorithm for computing minimum spanning tree (only for undirected weighted graphs)
	 * @param graph
	 * @return vector of source/end vertices of MST edges
	 */
	template<typename V, typename E>
	std::vector<std::pair<size_t, size_t>> kruskalMST(const Graph<V, E>& graph)
	{
		if(graph.isDirected())
		{
			throw std::invalid_argument("Kruskal algorithm is defined only for undirected graphs. For directed use Edmond/Karps.");
		}

		std::vector<std::pair<size_t, size_t>> result;
		auto edges = graph.getEdgesPositionsAndValues();
		std::sort(edges.begin(), edges.end(), [](auto a, auto b)
		{
			return std::get<2>(a) < std::get<2>(b);
		} );

		UnionFind uf(graph.getVerticesMap());

		for(auto& edge : edges)
		{
			if(uf.find(std::get<0>(edge)) != uf.find(std::get<1>(edge)))
			{
				result.push_back({std::get<0>(edge), std::get<1>(edge)});
				uf.unionSets(std::get<0>(edge), std::get<1>(edge));
			}
		}

		return result;
	}

	template<typename V>
	std::vector<std::pair<size_t, size_t>> kruskalMST(const Graph<V, Unweight>& graph) = delete;

	/**
	* Djikstra algorithm
	* @param graph graph
	* @param source source vertex
	* @param infinity max value of E
	* @return map of distances and predecessors for shortest paths
	*/
	template<typename V, typename E>
	std::pair<std::map<size_t, E>, std::map<size_t, size_t>> 
	dijkstraAll(const Graph<V, E>& graph, size_t source, E infinity = std::numeric_limits<E>::max())
	{
		auto vertices = graph.getVerticesMap();
		if (vertices.find(source) == vertices.end())
		{
			throw std::invalid_argument("source vertex id not found");
		}
		std::set<size_t> vertex_queue;
		std::map<size_t, E> distance = graph.template getVerticesIdsMap<E>();
		std::map<size_t, size_t> predecessors = graph.template getVerticesIdsMap<size_t>();
		auto graphEdges = graph.getEdgesPositions();

		for (auto & d : distance)
		{
			d.second = infinity;
		}

		for (auto & p : predecessors)
		{
			p.second = p.first;
		}
		for (auto & v : vertices)
		{
			vertex_queue.insert(v.first);
		}
		distance.at(source) = E();
		
		while (!vertex_queue.empty())
		{
			size_t u = *(vertex_queue.begin());
			for (auto & v : vertex_queue)
			{
				if (distance.at(v) < distance.at(u)) u = v;
			}
			vertex_queue.erase(u);
			for (auto & w : graph.getNeighbours(u))
			{
				if (vertex_queue.find(w) != vertex_queue.end())
				{
					E alt = distance.at(u) + graph.getEdgeValue(u, w);
					if (alt < distance.at(w))
					{
						distance.at(w) = alt;
						predecessors.at(w) = u;
					}
				}
			}
		}
		return {distance, predecessors};
	}

	/**
	* Djikstra algorithm
	* @param graph graph
	* @param source source vertex
	* @param target target vertex
	* @param infinity max value of E
	* @return distance and shortest path
	*/
	template<typename V, typename E>
	std::pair<E, std::vector<size_t>>
	dijkstra(const Graph<V, E>& graph, size_t source, size_t target, E infinity = std::numeric_limits<E>::max())
	{
		auto vertices = graph.getVerticesMap();
		if (vertices.find(source) == vertices.end())
		{
			throw std::invalid_argument("source vertex id not found");
		}
		if (vertices.find(target) == vertices.end())
		{
			throw std::invalid_argument("target vertex id not found");
		}
		std::set<size_t> vertex_queue;
		std::map<size_t, E> distance = graph.template getVerticesIdsMap<E>();
		std::map<size_t, size_t> predecessors = graph.template getVerticesIdsMap<size_t>();
		auto graphEdges = graph.getEdgesPositions();

		for (auto & d : distance)
		{
			d.second = infinity;
		}

		for (auto & p : predecessors)
		{
			p.second = p.first;
		}
		for (auto & v : vertices)
		{
			vertex_queue.insert(v.first);
		}
		distance.at(source) = E();

		size_t u;
		while (!vertex_queue.empty())
		{
			u = *(vertex_queue.begin());
			for (auto & v : vertex_queue)
			{
				if (distance.at(v) < distance.at(u)) u = v;
			}
			vertex_queue.erase(u);
			if (u == target) break;
			for (auto & w : graph.getNeighbours(u))
			{
				if (vertex_queue.find(w) != vertex_queue.end())
				{
					E alt = distance.at(u) + graph.getEdgeValue(u, w);
					if (alt < distance.at(w))
					{
						distance.at(w) = alt;
						predecessors.at(w) = u;
					}
				}
			}
		}
		vector<size_t> result;
		u = target;
		while (predecessors.at(u) != u)
		{
			result.push_back(u);
			u = predecessors.at(u);
		}
		result.push_back(u);
		std::reverse(result.begin(), result.end());
		return{ distance.at(target), result };
	}
}