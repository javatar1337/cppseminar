#pragma once

#include <map>
#include <string>
#include <sstream>

namespace Graph
{
	/**
	 * Class for template specialization
	 */
	class Unweight {};

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
	* Vertex class
	*/
	template<typename V, typename E>
	class Vertex
	{
	public://change to private later, just makes it easier to code, cause IDE suggestions
		friend class AbstractGraph<V,E>;
		friend class Graph<V,E>;
		long id;
		V value;
		std::map<long, E> edgesTo;
		Vertex() {}
		Vertex(long id, V value) :id(id), value(value) {}
	public:
		long getId() { return id; }
		V getValue() { return value; }
	};

	/**
	 * Abstract Graph class
	 */
	template<typename V, typename E>
	class AbstractGraph
	{
	protected:
		bool directed;
		using vertexMap = std::map<long, Vertex<V, E> >;
		vertexMap vertices;
		long total_id = 0;
	public:
		/**
		* Default constructor
		*/
		AbstractGraph() :directed(true)
		{}

		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
		AbstractGraph(bool directed) :directed(directed)
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
        long addVertex(const Vertex<V,E> & vertex)
		{
            auto toReturn = vertices.insert(std::pair<long, Vertex<V, E> > (total_id, vertex));
			total_id++;
			return toReturn.first->first;
		}

		/**
		* Add and move vertex 
        * @param value value to be inserted
		* @return iterator to inserted vertex
		*/
		long addVertex(Vertex<V, E> && vertex)
		{
            auto toReturn = vertices.insert(std::pair<long, Vertex<V, E> >(total_id, std::move(vertex)));
			total_id++;
			return toReturn.first->first;
		}

		/** 
		 * Add vertex by value reference
         * @param value value to be inserted
		 * @return iterator to inserted vertex
		 */
        long addVertex(const V & value)
		{
			Vertex<V,E> v(total_id, value);
			return addVertex(v);
		}

		/**
		* Add vertex by moving value
        * @param value value to be inserted
		* @return iterator to inserted vertex
		*/
		long addVertex(V && value)
		{
			Vertex<V, E> v(total_id, std::move(value));
			return addVertex(v);
		}

		/**
		* Get map of vertices containing, where key = id, value = value stored in vertex
		* @return map of vertices
		*/
		const vertexMap & getVertices() const
		{
			return vertices;
		}

		/**
		* Remove vertex and all edges adjacent to it
		* @param vertex id of vertex to remove
		* @return number vertices removed - at most 1
		*/
        size_t removeVertex(long vertex)
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
        size_t removeEdge(long from, long to)
		{
			return vertices[from].edgesTo.erase(to);
		}

		//todelete
		std::string listvertices() const
		{
			std::stringstream ss;
			for (auto & m : vertices)
			{
				ss << m.second.id + 1 << "." << m.second.value << " ";
			}
			return ss.str();
		}

		//todelete
		std::string listedges()
		{
			std::stringstream ss;
			for (auto & m : vertices)
			{
				for (auto & n : m.second.edgesTo)
				{
					ss << "Edge from " << m.second.value << " to " << vertices[n.first].value << " with value " << n.second <<"\n";
				}
			}
			return ss.str();
		}
	};


	/**
	 * Graph class
	 */
	template<typename V, typename E>
	class Graph : public AbstractGraph<V,E>
	{
	public:
		/**
		* Default constructor
		*/
        Graph() : AbstractGraph<V,E>()          // In clang, referencing to AbstractGraph using only name does not work (needs template params also)
		{}

		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
        Graph(bool directed) :AbstractGraph<V,E>(directed)
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
        void addEdge(long from, long to, const E & value)
		{
            this->vertices[from].edgesTo.insert(std::pair<long, E>(to, value));     // "this->" must go before the vertices name ( clang complains otherwise, some inheritance problem probably )
            if(!(this->directed))
            {
                this->vertices[to].edgesTo.insert(std::pair<long, E>(from, value));
            }
		}

		/**
		* Add edge by moving value
		* @param from vertex from edge is leaving, must be in graph
		* @param to vertex to edge is coming, must be in graph
		* @param value value of edge
		* @return nothing
		*/
        void addEdge(long from, long to, E && value)
		{
            this->vertices[from].edgesTo.insert(std::pair<long, E>(to, std::move(value)));
            if (!(this->directed))
            {
                this->vertices[to].edgesTo.insert(std::pair<long, E>(from, std::move(value)));
            }
		}
	};

	/**
	 * Graph class specialization
	 */
	template<typename V>
	class Graph<V, Unweight> : public AbstractGraph<V, Unweight>
	{
	public:
		/**
		* Default constructor
		*/
        Graph() : AbstractGraph<V, Unweight>()
		{}

		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
        Graph(bool directed) :AbstractGraph<V, Unweight>(directed)
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
        void addEdge(long from, long to)
		{
			Unweight u;
            this->vertices[from].edgesTo.insert(std::make_pair(to, u));
            if (!(this->directed))
            {
                this->vertices[to].edgesTo.insert(std::pair<long, Unweight>(from, u));
            }
		}
	};
}
