#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>

#ifndef GRAPH_DEBUG
#define GRAPH_DEBUG
#endif

namespace Graph
{
	/**
	 * Class for template specialization
	 */
	class Unweight
	{
	private:
		Unweight();
	public:
		template<typename V, typename E>
		friend class Graph;
	};

	/**
	 * Classes declarations
	 */
	template<typename V, typename E>
	class AbstractGraph;

	template<typename V, typename E = Unweight>
	class Graph;

	template<typename U>
	class Graph<U, Unweight>;

	/**
	 * Abstract Graph class
	 */
	template<typename V, typename E>
	class AbstractGraph
	{
	protected:
		/**
		* Vertex class
		*/
		class Vertex
		{
		public:     // TODO change to private later, just makes it easier to code, cause IDE suggestions
			friend class AbstractGraph<V,E>;
			friend class Graph<V,E>;
			size_t id;
			V value;
			std::map<size_t, E> edgesTo;
			//Vertex() {}  NOTE read comment at Source.cpp line 59
			Vertex(size_t id, V value) :id(id), value(value) {}
		public:
			size_t getId() const
			{
				return id;
			}
			const V& getValue() const
			{
				return value;
			}
		};

		using vertexMap = std::map<size_t, Vertex >;

		bool directed;
		vertexMap vertices;
		size_t total_id = 0;

		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
		AbstractGraph(bool directed = true) :directed(directed)
		{}

		/**
		* Destructor
		*/
		virtual ~AbstractGraph()
		{}

		/**
		* Add vertex
		* @param value value to be inserted
		* @return iterator to inserted vertex
		*/
		size_t addVertex(const Vertex& vertex)
		{
			auto toReturn = vertices.insert(std::pair<size_t, Vertex> (total_id, vertex));
			total_id++;
			return toReturn.first->first;
		}

		/**
		* Add and move vertex
		* @param value value to be inserted
		* @return iterator to inserted vertex
		*/
		size_t addVertex(Vertex && vertex)
		{
			auto toReturn = vertices.insert(std::pair<size_t, Vertex >(total_id, std::move(vertex)));
			total_id++;
			return toReturn.first->first;
		}
	public:
		/**
		 * Add vertex by value reference
		 * @param value value to be inserted
		 * @return iterator to inserted vertex
		 */
		size_t addVertex(const V & value)
		{
			Vertex v(total_id, value);
			return addVertex(v);
		}

		/**
		* Add vertex by moving value
		* @param value value to be inserted
		* @return iterator to inserted vertex
		*/
		size_t addVertex(V && value)
		{
			Vertex v(total_id, std::move(value));
			return addVertex(v);
		}

		/**
		* @brief Get pair of <id, value> of vertices
		* @return vector of pairs containing id and value of given vertex
		*/
		std::vector<std::pair<size_t, V>> getVerticesValues() const
		{
			std::vector<std::pair<size_t, V>> result;
			for(auto& vert : vertices)
			{
				result.push_back({ vert.first, vert.second.getValue() });
			}
			return result;
		}

		/**
		 * @brief Get value of given vertex
		 * @param vertex id of vertex
		 * @return value of this vertex
		 */
		const V& getVertexValue(size_t vertex) const
		{
			return vertices.find(vertex)->second.getValue();
		}

		/**
		* Remove vertex and all edges adjacent to it
		* @param vertex id of vertex to remove
		* @return number vertices removed - at most 1
		*/
		size_t removeVertex(size_t vertex)
		{
			for (auto & i : vertices)
			{
				i.second.edgesTo.erase(vertex);
			}
			return vertices.erase(vertex);
		}

		/**
		* Remove edge
		* @param from vertex from
		* @param to vertex to
		* @return number edges removed - at most 1
		*/
		size_t removeEdge(size_t from, size_t to)
		{
			return vertices.find(from)->second.edgesTo.erase(to);
		}

#ifdef GRAPH_DEBUG
		std::string listvertices() const
		{
			std::stringstream ss;
			for (auto & m : vertices)
			{
				ss << m.second.id + 1 << "." << m.second.value << " ";
			}
			return ss.str();
		}

		std::string listedges()
		{
			std::stringstream ss;
			for (auto & m : vertices)
			{
				for (auto & n : m.second.edgesTo)
				{
					ss << "Edge from " << m.second.value << " to " << vertices.find(n.first)->second.value << " with value " << n.second <<"\n";
				}
			}
			return ss.str();
		}
#endif
	};


	/**
	 * Graph class
	 */
	template<typename V, typename E>
	class Graph : public AbstractGraph<V,E>
	{
	public:
		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
		Graph(bool directed = true) :AbstractGraph<V,E>(directed)
		{}

		/**
		* Destructor
		*/
		virtual ~Graph()
		{}

		/**
		* Add edge by value reference
		* @param from vertex from edge is leaving, must be in graph
		* @param to vertex to edge is coming, must be in graph
		* @param value value of edge
		* @return nothing
		*/
		void addEdge(size_t from, size_t to, const E & value)
		{
			this->vertices.find(from)->second.edgesTo.insert({to, value});     // NOTE "this->" must go before the vertices name ( clang complains otherwise, some inheritance problem probably )
			if(!(this->directed))
			{
				this->vertices.find(to)->second.edgesTo.insert({from, value});
			}
		}

		/**
		* Add edge by moving value
		* @param from vertex from edge is leaving, must be in graph
		* @param to vertex to edge is coming, must be in graph
		* @param value value of edge
		* @return nothing
		*/
		void addEdge(size_t from, size_t to, E && value)
		{
			this->vertices.find(from)->second.edgesTo.insert({to, std::move(value)});
			if (!(this->directed))
			{
				this->vertices.find(to)->second.edgesTo.insert({from, std::move(value)});
			}
		}

		// NOTE It seems that all addEdge, listedges, .. methods are reversed
		// E.g. addEdge(from, to) - we find "from edge" and add "to edge" to their "edgesTo"
		// but it probably should be in pseudocode: vertices.find(to).edgesTo.insert(from)
		const E& getEdgeValue(size_t from, size_t to)
		{
			return this->vertices.find(from)->second.edgesTo.find(to)->second;
		}
	};

	/**
	 * Graph class specialization for unweighted graphs
	 */
	template<typename V>
	class Graph<V, Unweight> : public AbstractGraph<V, Unweight>
	{
	public:
		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
		Graph(bool directed = true) :AbstractGraph<V, Unweight>(directed)
		{}

		/**
		* Destructor
		*/
		virtual ~Graph()
		{}

		/**
		* Add edge by value reference
		* @param from vertex from edge is leaving, must be in graph
		* @param to vertex to edge is coming, must be in graph
		* @return nothing
		*/
		void addEdge(size_t from, size_t to)
		{
			Unweight u;
			this->vertices.find(from)->second.edgesTo.insert({to, u});
			if (!(this->directed))
			{
				this->vertices.find(to)->second.edgesTo.insert({from, u});
			}
		}
	};
}

#undef GRAPH_DEBUG
