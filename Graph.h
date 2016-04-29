#pragma once

#include <map>
#include <string>
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
        // NOTE ctor moved to private and Graph classes made friend so this class could be instantiated only by graph
        //      and not everywhere => gives another "hint" to possible user that this class has no "meaning"
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
        public://change to private later, just makes it easier to code, cause IDE suggestions
            friend class AbstractGraph<V,E>;
            friend class Graph<V,E>;
            long id;    // NOTE id might be size_t later as negative values are probably not needed?
            V value;    // NOTE What about non-default constructible items?
            std::map<long, E> edgesTo;
            Vertex() {}
            Vertex(long id, V value) :id(id), value(value) {}
        public:
            long getId() const { return id; }           // NOTE const qualifier added + returning value by const ref to prevent copying
            const V& getValue() const { return value; }
        };

        using vertexMap = std::map<long, Vertex >;

        bool directed;
		vertexMap vertices;
		long total_id = 0;

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
    public:         // NOTE Ctors + dtor moved to protected to make it really abstract
		/**
		* Add vertex
        * @param value value to be inserted
		* @return iterator to inserted vertex
		*/
        long addVertex(const Vertex& vertex)
		{
            auto toReturn = vertices.insert(std::pair<long, Vertex> (total_id, vertex));
			total_id++;
			return toReturn.first->first;
		}

		/**
		* Add and move vertex 
        * @param value value to be inserted
		* @return iterator to inserted vertex
		*/
        long addVertex(Vertex && vertex)
		{
            auto toReturn = vertices.insert(std::pair<long, Vertex >(total_id, std::move(vertex)));
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
            Vertex v(total_id, value);
			return addVertex(v);
		}

		/**
		* Add vertex by moving value
        * @param value value to be inserted
		* @return iterator to inserted vertex
		*/
		long addVertex(V && value)
		{
            Vertex v(total_id, std::move(value));
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
					ss << "Edge from " << m.second.value << " to " << vertices[n.first].value << " with value " << n.second <<"\n";
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
		* Default constructor
		*/
        Graph() : AbstractGraph<V,E>()          // NOTE In clang, referencing to AbstractGraph using only name does not work (needs template params also)
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
            this->vertices[from].edgesTo.insert(std::pair<long, E>(to, value));     // NOTE "this->" must go before the vertices name ( clang complains otherwise, some inheritance problem probably )
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

#undef GRAPH_DEBUG
