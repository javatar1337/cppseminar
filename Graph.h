#pragma once
namespace Graph
{
	/**
	 * Class for template specialization
	 */
	class Unweight {};

	/**
	* Graph class
	*/
	template<typename V, typename E = Unweight>
	class Graph
	{
	private:
		bool directed; //for now, may switch for enum
	public:
		/**
		* Default constructor
		*/
		Graph() :directed(true)
		{

		}

		/**
		 * Orientation constructor
		 * @param directed true for directed, false undirected
		 */
		Graph(bool directed) :directed(directed)
		{

		}

		/**
		* Destructor
		*/
		~Graph()
		{
		}
	};

	/**
	 * Graph class specialization
	 */
	template<typename U>
	class Graph<U, Unweight>
	{
		bool directed;
	public:
		/**
		* Default constructor
		*/
		Graph() :directed(true)
		{

		}

		/**
		* Orientation constructor
		* @param directed true for directed, false undirected
		*/
		Graph(bool directed) :directed(directed)
		{

		}

		/**
		* Destructor
		*/
		~Graph()
		{
		}
	};
}
