#pragma once

#include "Graph.h"
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <tuple>
#include <memory>

namespace Graph
{
	/**
	* Namespace for additional helper stuff
	*/
	namespace helper
	{
		/**
		 * Union-find data structure
		 */
		class UnionFind
		{
		private:
			struct Node
			{
				std::map<size_t, std::unique_ptr<Node>>::iterator parent;
				
				Node(const std::map<size_t, std::unique_ptr<Node>>::iterator& iter)
					:parent(iter) 
				{ }
			};
		
			std::map<size_t, std::unique_ptr<Node>> mVertices;
			std::map<size_t, size_t> mSetSizes;
			
			std::map<size_t, std::unique_ptr<Node>>::iterator 
			find(size_t idx, const std::unique_ptr<Node>& node)
			{
				if(node->parent->first != idx)
				{
					node->parent = find(node->parent->first, node->parent->second);
				}
				return node->parent;
			}
		public:
			UnionFind(const std::vector<size_t>& vertices)
			{
				for(auto& a : vertices)
				{
					auto it = mVertices.emplace(a, nullptr);
					it.first->second = std::make_unique<Node>(it.first);
					mSetSizes.emplace(a, 1);
				}
			}

			template<typename T>
			UnionFind(const std::map<size_t, T>& verticesMap)
			{
				for(auto& a : verticesMap)
				{
					auto it = mVertices.emplace(a.first, nullptr);
					it.first->second = std::make_unique<Node>(it.first);
					mSetSizes.emplace(a.first, 1);
				}
			}

			size_t find(size_t item)
			{
				auto it = mVertices.find(item);
				return find(it->first, it->second)->first;
			}

			void unionSets(size_t first, size_t second)
			{
				size_t firstRoot = find(first);
				size_t secondRoot = find(second);

				if(firstRoot == secondRoot) { return; }

				if(mSetSizes.at(first) < mSetSizes.at(second))
				{
					mVertices.find(firstRoot)->second->parent = mVertices.find(secondRoot)->second->parent;
					mSetSizes.at(secondRoot) = mSetSizes.at(firstRoot);
				}
				else
				{
					mVertices.find(secondRoot)->second->parent = mVertices.find(firstRoot)->second->parent;
					mSetSizes.at(firstRoot) = mSetSizes.at(secondRoot);
				}
			}

			size_t size() const
			{
				return mVertices.size();
			}
		};

		
		template<typename E>
		struct CompareSecond
		{
			bool operator()(const std::pair<std::size_t, E>& left, const std::pair<std::size_t, E>& right) const
			{
				return left.second > right.second;
			}
		};

		template<typename E>
		struct CompareThird
		{
			bool operator()(const std::tuple<size_t, size_t, E>& left, const std::tuple<size_t, size_t, E>& right) const
			{
				return (std::get<2>(left) > std::get<2>(right)) &&
				       (
				           ((std::get<0>(left))!=std::get<1>(left)) && ((std::get<0>(right)) != std::get<1>(right))
				       );
			}
		};

		/**
		* Depth-first search algorithm
		* @param graph graph
		* @param starting_vertex vertex to start search from, must be part of graph
		* @param preorder unary function
		* @param postorder unary function
		*/
		template<typename V, typename E, typename UnaryFunction1, typename UnaryFunction2>
		void _dfs(Graph<V, E> & graph, size_t starting_vertex, UnaryFunction1 preorder, UnaryFunction2 postorder, std::map<size_t, bool> & discovered)
		{
			discovered.find(starting_vertex)->second = true;
			preorder(graph.getVertexValue(starting_vertex));
			for (auto & v : graph.getNeighbours(starting_vertex))
			{
				if(!discovered.find(v)->second) _dfs(graph, v, preorder, postorder, discovered);
			}
			postorder(graph.getVertexValue(starting_vertex));
			return;
		}
		

		/**
		*  Get pairs of <id, value> of vertices
		* @return map, where key = id and value = value of given vertex
		*/
		template<typename V, typename E>
		std::map<size_t, V> getVerticesMap(const GraphBase<V,E>& graph)
		{
			auto ids = graph.getVerticesIds();
			std::map<size_t, V> result;
			for (auto& vert : ids)
			{
				result.emplace(vert, graph.getVertexValue(vert));
			}
			return result;
		}

		/**
		 * Returns map with vertices ids as keys and default constructed element T as value
		 * @return map, where key = id and value = default constructed template parameter
		 */
		template<typename T, typename V, typename E>
		std::map<size_t, T> getVerticesMap(const GraphBase<V,E>& graph)
		{
			auto ids = graph.getVerticesIds();
			std::map<size_t, T> result;
			for (auto& vert: ids)
			{
				result.emplace(vert, T());
			}
			return result;
		}
	}

	/**
	* Depth-first search algorithm
	* @param graph graph
	* @param starting_vertex vertex to start search from, must be part of graph
	* @param preorder unary function
	* @param postorder unary function
	*/
	template<typename V, typename E, typename UnaryFunction1, typename UnaryFunction2>
	void DFS(Graph<V, E> & graph, size_t starting_vertex, UnaryFunction1 preorder, UnaryFunction2 postorder)
	{
		std::map<size_t, bool> discovered;
		auto vertmap = helper::getVerticesMap(graph);
		if (vertmap.find(starting_vertex) == vertmap.end())
		{
			return;
		}
		for (auto & ver : vertmap)
		{
			discovered.emplace(ver.first, false);
		}
		helper::_dfs(graph, starting_vertex, preorder, postorder, discovered);

		return;
	}

	/**
	* Breadth-first search algorithm
	* @param graph graph
	* @param starting_vertex vertex to start search from, must be part of graph
	* @param f unary function
	* @return distances and paths to discovered vertices(if graph is enweighted, then shortest paths)
	*/
	template<typename V, typename E, typename UnaryFunction>
	std::pair<std::map<size_t, size_t>, std::map<size_t, size_t>>
	BFS(Graph<V, E> & graph, size_t starting_vertex, UnaryFunction f)
	{
		std::map<size_t, size_t> distance;
		std::map<size_t, size_t> parent;
		auto vertmap = helper::getVerticesMap(graph);
		if (vertmap.find(starting_vertex) == vertmap.end())
		{
			return {distance, parent};
		}
		for (auto & ver : vertmap)
		{
			distance.emplace(ver.first, std::numeric_limits<size_t>::max());
			parent.emplace(ver.first, ver.first);
		}
		std::queue<size_t> vertex_queue;
		vertex_queue.push(starting_vertex);
		distance.find(starting_vertex)->second = 0;
		while (!vertex_queue.empty())
		{
			size_t v = vertex_queue.front();
			vertex_queue.pop();
			f(graph.getVertexValue(v));
			for (auto & edd : graph.getNeighbours(v))
			{
				if (distance.find(edd)->second == std::numeric_limits<size_t>::max())
				{
					distance.find(edd)->second = distance.find(v)->second + 1;
					parent.find(edd)->second = v;
					vertex_queue.push(edd);
				}
			}
		}
		return {distance, parent};
	}

	/**
	 * Bellman-Ford shortest path algorithm
	 * @param graph graph to find shortest paths in
	 * @param startVertex source vertex
	 * @param infinity infinity value
	 * @return pair of maps of <distances of each vertex from source vertex (infinity if no path exists) AND predecessors>
	 */
	template<typename V, typename E>
	std::pair<std::map<size_t, E>, std::map<size_t, size_t>>
	bellmanFord(const Graph<V,E>& graph, size_t startVertex, E infinity = std::numeric_limits<E>::max())
	{
		static_assert(std::is_default_constructible<E>::value, "Edge type must be default constructible.");
		
		std::map<size_t, E> distance = helper::getVerticesMap<E>(graph);
		std::map<size_t, size_t> predecessors = helper::getVerticesMap<size_t>(graph);
		auto graphEdges = graph.getEdgesPositions(true);

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
	 * Bellman-Ford shortest path algorithm
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
	 * Bellman-Ford shortest path algorithm
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
	 * Kruskal algorithm for computing minimum spanning tree (only for undirected weighted graphs)
	 * @param graph
	 * @return vector of source/end vertices of MST edges
	 */
	template<typename V, typename E>
	std::vector<std::pair<size_t, size_t>> kruskalMST(const Graph<V, E>& graph)
	{
		if(graph.isDirected())
		{
			throw std::invalid_argument("Kruskal algorithm is defined only for undirected graphs.");
		}

		std::vector<std::pair<size_t, size_t>> result;
		auto edges = graph.getEdgesPositionsAndValues();
		std::sort(edges.begin(), edges.end(), [](auto a, auto b)
		{
			return std::get<2>(a) < std::get<2>(b);
		} );

		helper::UnionFind uf(helper::getVerticesMap(graph));

		for(auto& edge : edges)
		{
			if(uf.find(std::get<0>(edge)) != uf.find(std::get<1>(edge)))
			{
				result.emplace_back(std::get<0>(edge), std::get<1>(edge));
				uf.unionSets(std::get<0>(edge), std::get<1>(edge));
			}
		}

		return result;
	}

	template<typename V>
	std::vector<std::pair<size_t, size_t>> kruskalMST(const Graph<V, Unweight>& graph) = delete;

	/**
	* Dijkstra algorithm
	* @param graph graph
	* @param source source vertex
	* @param infinity max value of E
	* @return map of distances and predecessors for shortest paths
	*/
	template<typename V, typename E>
	std::pair<std::map<size_t, E>, std::map<size_t, size_t>>
	dijkstraAll(const Graph<V, E>& graph, size_t source, E infinity = std::numeric_limits<E>::max())
	{
		static_assert(std::is_default_constructible<E>::value, "Edge type must be default constructible.");
		
		auto vertices = helper::getVerticesMap(graph);
		if (vertices.find(source) == vertices.end())
		{
			throw std::invalid_argument("source vertex id not found");
		}
		std::set<size_t> vertex_queue;
		std::map<size_t, E> distance = helper::getVerticesMap<E>(graph);
		std::map<size_t, size_t> predecessors = helper::getVerticesMap<size_t>(graph);
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
			std::priority_queue<std::pair<size_t, E>, std::vector<std::pair<size_t, E>>, helper::CompareSecond<E>> vqm;
			for (auto & y : vertex_queue)
			{
				vqm.emplace(y, distance.at(y));
			}
			size_t u = vqm.top().first;
			vertex_queue.erase(u);
			for (auto & w : graph.getNeighbours(u))
			{
				if (vertex_queue.find(w) != vertex_queue.end())
				{
					E alt = (distance.at(u) != infinity)? (distance.at(u) + graph.getEdgeValue(u, w)) : infinity;
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
	* Dijkstra algorithm
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
		auto dAll = dijkstraAll(graph, source, infinity);
		std::vector<size_t> result;
		size_t u = target;
		while (dAll.second.at(u) != u)
		{
			result.push_back(u);
			u = dAll.second.at(u);
		}
		result.push_back(u);
		std::reverse(result.begin(), result.end());
		return { dAll.first.at(target), result };
	}

	/**
	* Prim's algorithm for computing minimum spanning tree (only for connected undirected weighted graphs)
	* @param graph
	* @param source vertex
	* @return set of source/end vertices of MST edges
	*/
	template<typename V, typename E>
	std::set<std::pair<size_t, size_t>> prim(const Graph<V, E>& graph, size_t source)
	{
		if (graph.isDirected())
		{
			throw std::invalid_argument("graph must be undirected");
		}

		std::set<std::pair<size_t, size_t>> result;

		std::set<size_t> vertices;
		vertices.insert(source);
		size_t v = source;
		//std::set<std::tuple<size_t, size_t, E>> edges;
		std::priority_queue<std::tuple<size_t, size_t, E>, std::vector<std::tuple<size_t, size_t, E>>, helper::CompareThird<E>> edges;
		while (vertices.size() < graph.getVerticesCount())
		{
			auto vedges = graph.getEdgesFrom(v);
			for (auto & w : vedges)
			{
				if (vertices.find(w.first) == vertices.end()) edges.emplace(v, w.first, w.second);
			}
			//auto edge = (*std::min_element(edges.begin(), edges.end(), helper::CompareThird<E>()));
			auto edge = edges.top();
			if (result.find({ std::get<1>(edge), std::get<0>(edge) }) == result.end()) result.insert({ std::get<0>(edge), std::get<1>(edge) });
			vertices.insert(std::get<1>(edge));
			edges.pop();
			v = std::get<1>(edge);
		}

		return result;
	}

	/**
	* Prim's algorithm for computing minimum spanning tree (only for connected undirected weighted graphs)
	* @param graph
	* @return set of source/end vertices of MST edges
	*/
	template<typename V, typename E>
	std::set<std::pair<size_t, size_t>> prim(const Graph<V, E>& graph)
	{
		auto graphvertices = helper::getVerticesMap(graph);
		size_t source = (*(graphvertices.begin())).first;
		return prim(graph, source);
	}

	template<typename V>
	std::vector<std::pair<size_t, size_t>> prim(const Graph<V, Unweight>& graph) = delete;

	template<typename V>
	std::vector<std::pair<size_t, size_t>> prim(const Graph<V, Unweight>& graph, size_t source) = delete;

	/**
	 * Edmonds-Karp algorithm
	 * @param graph graph
	 * @param source source vertex
	 * @param sink sink vertex
	 * @return maximum flow, graph with edges values equal to their flow
	 */
	template<typename V, typename E>
	std::pair<E, Graph<V,E>> edmondsKarpMaxFlow(Graph<V, E> graph, size_t source, size_t sink)
	{
		static_assert(std::is_default_constructible<E>::value, "Edge type must be default constructible.");
		
		// Capacity graph
		auto edges = graph.getEdgesPositions();

		for(auto& e : edges)
		{
			if(!graph.adjacent(e.second,e.first))
			{
				graph.addEdge(e.second, e.first, E());
			}
		}

		// Flow graph
		Graph<V,E> graphFlow = graph;

		for(auto& e : edges)
		{
			graphFlow.updateEdgeValue(e.first, e.second, E());
			graphFlow.updateEdgeValue(e.second, e.first, E());
		}

		E maxFlow {};

		while(true)
		{
			std::queue<size_t> q;
			q.push(source);
			// key = destination vertex; value = source vertex of edge going to key
			std::map<size_t, size_t> pred;

			while(!q.empty())
			{
				size_t curr = q.front();
				q.pop();
				auto outEdges = graph.getEdgesFrom(curr);
				for(auto& e : outEdges)
				{
					if(pred.find(e.first) == pred.end() && e.first != source &&
					        graph.getEdgeValue(curr, e.first) > graphFlow.getEdgeValue(curr, e.first))
					{
						pred.emplace(e.first, curr);
						q.push(e.first);
					}
				}
			}

			if(pred.find(sink) == pred.end())
			{
				break;
			}

			E df = std::numeric_limits<E>::max();

			for(auto e = sink; e != source; e = pred.at(e))
			{
				df = std::min(df, graph.getEdgeValue(pred.at(e), e) - graphFlow.getEdgeValue(pred.at(e), e));
			}

			for(auto e = sink; e != source; e = pred.at(e))
			{
				graphFlow.updateEdgeValue(pred.at(e), e,
				                          graphFlow.getEdgeValue(pred.at(e), e) + df);
				graphFlow.updateEdgeValue(e, pred.at(e),
				                          graphFlow.getEdgeValue(e, pred.at(e)) - df);
			}

			maxFlow += df;
		}

		return { maxFlow, graphFlow };
	}

	template<typename V>
	std::pair<Unweight, Graph<V,Unweight>> edmondsKarpMaxFlow(Graph<V, Unweight> graph, size_t source, size_t sink) = delete;
}