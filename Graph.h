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
		V value;
		std::map<Vertex<V, E>, E> edgesFrom;
	public:
		Vertex() {}
		Vertex(V value) :value(value) {}
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
		long id = 0;
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
		* @param value value
		* @return iterator to inserted vertex
		*/
		typename vertexMap::iterator addVertex(Vertex<V,E> & vertex)
		{
			std::pair<vertexMap::iterator, bool> toReturn = vertices.insert(std::pair<long, Vertex<V, E> > (id, vertex));
			id++;
			return toReturn.first;
		}

		/**
		* Add and move vertex 
		* @param value value
		* @return iterator to inserted vertex
		*/
		typename vertexMap::iterator addVertex(Vertex<V, E> && vertex)
		{
			std::pair<vertexMap::iterator, bool> toReturn = vertices.insert(std::pair<long, Vertex<V, E> >(id, std::move(vertex)));
			id++;
			return toReturn.first;
		}

		/** 
		 * Add vertex by value reference
		 * @param value value
		 * @return iterator to inserted vertex
		 */
		typename vertexMap::iterator addVertex(V & value)
		{
			Vertex<V,E> v(value);
			return addVertex(v);
		}

		/**
		* Add vertex by moving value
		* @param value value
		* @return iterator to inserted vertex
		*/
		typename vertexMap::iterator addVertex(V && value)
		{
			Vertex<V, E> v(std::move(value));
			return addVertex(v);
		}

		//todelete
		std::string listvertices()
		{
			std::stringstream ss;
			for (auto m : vertices)
			{
				ss << m.second.value << " ";
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
		Graph() : AbstractGraph()
		{}

		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
		Graph(bool directed) :AbstractGraph(directed)
		{}

		/**
		* Destructor
		*/
		virtual ~Graph()
		{}
	};

	/**
	 * Graph class specialization
	 */
	template<typename U>
	class Graph<U, Unweight> : public AbstractGraph<U, Unweight>
	{
	public:
		/**
		* Default constructor
		*/
		Graph() : AbstractGraph()
		{}

		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
		Graph(bool directed) :AbstractGraph(directed)
		{}

		/**
		* Destructor
		*/
		virtual ~Graph()
		{}
	};
}
