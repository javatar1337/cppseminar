#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

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
		Unweight() { }
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
			std::map<size_t, E> outgoingEdges;
			//Vertex() {}  NOTE read comment at Source.cpp line 59
			Vertex(size_t id, V value) :id(id), value(value) {}
			void setValue(const V & val)
			{
				value = val;
			}
			void setValue(V && val)
			{
				value = std::move(val);
			}
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

		// Func takes outputFile and outgoingEdges element as params and performs operation on those
		template<typename Func>
		bool _saveToFile(const std::string& filePath, Func f) const
		{
			std::ofstream outputFile(filePath);

			if(outputFile.is_open())
			{
				for(const auto& v : vertices)
				{
					outputFile << "id " << v.second.id << " "
					           << v.second.value << std::endl;
					for(const auto& e : v.second.outgoingEdges)
					{
						outputFile << e.first;

						f(outputFile, e);

						outputFile << std::endl;
					}
				}

				return true;
			}

			return false;
		}

		// Func takes stringstream and index of end vertex as params and performs operation on those
		template<typename Func>
		bool _loadFromFile(const std::string& filePath, bool isWeighted, Func f)
		{
			vertices.clear();
			std::ifstream inputFile(filePath);
			std::string line;
			bool retValue = false;

			if(inputFile.is_open())
			{
				retValue = true;

				while(getline(inputFile, line))
				{
					std::stringstream ss(line);

					if(line.substr(0, 3) == "id ")
					{
						ss.ignore(3);
						size_t vertId;
						V vertValue; // WARNING V must be default constructible in this case!

						if(!ss.good())
						{
							retValue = false;
							break;
						}

						ss >> vertId;
						ss.ignore(1);

						if(!ss.good())
						{
							retValue = false;
							break;
						}

						ss >> vertValue;

						vertices.insert({vertId, Vertex(vertId, std::move(vertValue))});

						if(vertId > total_id)
						{
							total_id = vertId;
						}
					}
					else
					{
						if((isWeighted && line.size() < 3) || (!isWeighted && line.size() < 1))
						{
							retValue = false;
							break;
						}

						size_t targetId;

						ss >> targetId;
						ss.ignore(1);

						if(isWeighted && !ss.good())
						{
							retValue = false;
							break;
						}

						f(ss, targetId);
					}

					ss.flush(); // just to be safe
				}
			}

			if(!retValue)
			{
				vertices.clear();
			}

			return retValue;
		}

		// Func gets ofstream and edge value as params and (possibly) modifies them
		template<typename Func>
		bool _exportToDot(const std::string& filePath, Func f)
		{
			std::ofstream outputFile(filePath);
			std::vector<std::pair<size_t, size_t>> exportedPairs;

			if(!outputFile.is_open())
			{
				return false;
			}

			outputFile << (directed ? "digraph {" : "graph {") << std::endl;

			// First we create labels and output them to file
			// Loop is seperate from the one below to have all labels defined at the start
			for(auto& v : vertices)
			{
				outputFile << v.second.id << " [label=\"";

				std::stringstream ss;
				ss << v.second.value;

				outputFile << ss.str();

				ss.flush();

				outputFile << "\"];" << std::endl;
			}

			// Now we create edges between vertices
			for(auto& v : vertices)
			{
				for(auto& e : v.second.outgoingEdges)
				{
					if(directed)
					{
						outputFile << v.second.id << " -> " << e.first;

						f(outputFile, e.second);

						outputFile << ";" << std::endl;
					}
					else
					{
						if(std::find(exportedPairs.begin(), exportedPairs.end(), std::make_pair(e.first, v.second.id)) == exportedPairs.end())
						{
							outputFile << v.second.id << " -- " << e.first;

							f(outputFile, e.second);

							exportedPairs.push_back({v.second.id, e.first});
							outputFile << ";" << std::endl;
						}
					}
				}
			}

			outputFile << "}";

			return true;
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
		 * @throws invalid_argument exception if id is invalid
		 * @return value of this vertex
		 */
		const V& getVertexValue(size_t vertex) const
		{
			auto is_in = vertices.find(vertex);
			if (is_in == vertices.end())
			{
				throw std::invalid_argument("vertex id not found");
			}
			return vertices.find(vertex)->second.getValue();
		}

		/**
		* @brief Set value of given vertex
		* @param vertex id of vertex
		* @return nothing
		*/
		void setVertexValue(size_t vertex, const V & value)
		{
			auto is_in = vertices.find(vertex);
			if (is_in != vertices.end()) {
				is_in->second.setValue(value);
			}
			return;
		}
		/**
		* @brief Set value of given vertex
		* @param vertex id of vertex
		* @return nothing
		*/
		void setVertexValue(size_t vertex, V && value)
		{
			auto is_in = vertices.find(vertex);
			if (is_in != vertices.end()) {
				is_in->second.setValue(std::move(value));
			}
			return;
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
				i.second.outgoingEdges.erase(vertex);
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
			return vertices.find(from)->second.outgoingEdges.erase(to);
		}

		/**
		* Test adjacency of 2 vertices
		* @param from vertex from
		* @param to vertex to
		* @return true if vertices are adjacent, false if not or if they are invalid
		*/
		bool adjacent(size_t from, size_t to)
		{
			bool result = false;
			if (vertices.find(from) != vertices.end())
			{
				if (vertices.find(from)->second.outgoingEdges.find(to) != vertices.find(from)->second.outgoingEdges.end())
					result = true;
			}
			return result;
		}

		/**
		* Find neighbours of vertex
		* @param vertex
		* @return vector of ids of neighbours, if invalid vertex - empty
		*/
		std::vector<size_t> getNeighbours(size_t vertex) const
		{
			std::vector<size_t> result;
			auto vertex_found = vertices.find(vertex);
			if (vertex_found != vertices.end())
			{
				for (auto & v : vertex_found->second.outgoingEdges)
				{
					result.push_back(v.first);
				}
			}
			return result;
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
#endif
	};


	/**
	 * Graph class
	 */
	template<typename V, typename E>
	class Graph : public AbstractGraph<V,E>
	{
	private:
		// As vars derived from AbstractGraph have dependent names,
		// usings are utilized here to avoid writing this->... every time
		using AbstractGraph<V,E>::vertices;
		using AbstractGraph<V,E>::directed;
		/**
		* Template update value of edge
		* @param from vertex from
		* @param to vertex to
		* @param value new value of edge
		* @return nothing
		*/
		template <typename X> void _updateEdgeValue(const size_t & from, const size_t & to, X && value)
		{
			auto vertex_from = vertices.find(from);
			if (vertex_from != vertices.end())
			{
				if (vertices.find(to) != vertices.end())
				{
					auto edgeto = vertex_from->second.outgoingEdges.find(to);
					if (edgeto != vertex_from->second.outgoingEdges.end())
					{
						vertex_from->second.outgoingEdges.find(to)->second = std::forward<X>(value);
					}
				}
			}
			return;
		}
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
			vertices.find(from)->second.outgoingEdges.insert({to, value});
			if(!directed)
			{
				vertices.find(to)->second.outgoingEdges.insert({from, value});
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
			auto is_in_from = vertices.find(from);
			auto is_in_to = vertices.find(to);
			if (is_in_from == vertices.end() || is_in_to == vertices.end())
			{
				return;
			}
			vertices.find(from)->second.outgoingEdges.insert({to, std::move(value)});
			if (!directed)
			{
				vertices.find(to)->second.outgoingEdges.insert({from, std::move(value)});
			}
		}

		/**
		* Get value of edge
		* @param from vertex from
		* @param to vertex to
		* @throws invalid_argument exception if either id is invalid
		* @return value of edge
		*/
		const E& getEdgeValue(size_t from, size_t to)
		{
			auto is_in_from = vertices.find(from);
			auto is_in_to = vertices.find(to);
			if (is_in_from == vertices.end())
			{
				throw std::invalid_argument("\"from\" vertex id not found");
			}
			if (is_in_to == vertices.end())
			{
				throw std::invalid_argument("\"to\" vertex id not found");
			}
			return vertices.find(from)->second.outgoingEdges.find(to)->second;
		}

		/**
		* Update value of edge
		* @param from vertex from
		* @param to vertex to
		* @param value new value of edge
		* @return nothing
		*/
		void updateEdgeValue(size_t from, size_t to, const E & value)
		{
			_updateEdgeValue(from, to, value);
		}

		/**
		* Update value of edge
		* @param from vertex from
		* @param to vertex to
		* @param move value new value of edge
		* @return nothing
		*/
		void updateEdgeValue(size_t from, size_t to, E && value)
		{
			_updateEdgeValue(from, to, std::move(value));
		}

		/**
		 * @brief Saves graph to file in custom format
		 * @param filePath file to which the graph will be saved (if file exists, will be overwritten)
		 * @return true if save request was successful, false otherwise
		 */
		bool saveToFile(const std::string& filePath) const
		{
			return this->_saveToFile(filePath, [](auto& outputFile, auto& e)
			{
				outputFile << " " << e.second;
			});
		}

		/**
		* @brief Clears current graph content and loads graph from file in custom format (vertices/edges names will be loaded till first whitespace)
		* @param filePath path to file
		* @return true if loading was successful, false otherwise
		*/
		bool loadFromFile(const std::string& filePath)
		{
			return this->_loadFromFile(filePath, true, [&,this](auto& ss, auto& targetId)
			{
				E edgeValue; // WARNING E must be default constructible in this case!
				ss >> edgeValue;
				vertices.rbegin()->second.outgoingEdges.insert({targetId, edgeValue});
			});
		}

		/**
		 * @brief Exports graph to dot format with ids as vertex names
		 * @param filePath path to file
		 * @return true if export was sucessful, false otherwise
		 */
		bool exportToDot(const std::string& filePath)
		{
			return this->_exportToDot(filePath, [](auto& outputFile, auto& e)
			{
				std::stringstream ss;
				ss << e;

				outputFile << "[label=\"" << ss.str() << "\",weight=\"" << ss.str() << "\"]";

				ss.flush();
			});
		}

#ifdef GRAPH_DEBUG
		std::string listedges()
		{
			std::stringstream ss;
			for (auto & m : vertices)
			{
				for (auto & n : m.second.outgoingEdges)
				{
					ss << "Edge from " << m.second.value << " to " << vertices.find(n.first)->second.value << " with value " << n.second <<"\n";
				}
			}
			return ss.str();
		}
#endif
	};

	/**
	 * Graph class specialization for unweighted graphs
	 */
	template<typename V>
	class Graph<V, Unweight> : public AbstractGraph<V, Unweight>
	{
	private:
		using AbstractGraph<V,Unweight>::vertices;
		using AbstractGraph<V,Unweight>::directed;
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
			auto is_in_from = vertices.find(from);
			auto is_in_to = vertices.find(to);
			if (is_in_from == vertices.end() || is_in_to == vertices.end())
			{
				return;
			}
			vertices.find(from)->second.outgoingEdges.insert({to, u});
			if (!directed)
			{
				vertices.find(to)->second.outgoingEdges.insert({from, u});
			}
		}

		/**
		 * @brief Saves graph to file in custom format
		 * @param filePath file to which the graph will be saved (if file exists, will be overwritten)
		 * @return true if save request was successful, false otherwise
		 */
		bool saveToFile(const std::string& filePath) const
		{
			return this->_saveToFile(filePath, [](auto&, auto&) { });
		}

		/**
		* @brief Clears current graph content and loads graph from file in custom format (vertices/edges names will be loaded till first whitespace)
		* @param filePath path to file
		* @return true if loading was successful, false otherwise
		*/
		bool loadFromFile(const std::string& filePath)
		{
			return this->_loadFromFile(filePath, false, [&, this](auto& ss, auto& targetId)
			{
				vertices.rbegin()->second.outgoingEdges.insert({targetId, Unweight()});
			});
		}

		/**
		 * @brief Exports graph to dot format with ids as vertex names
		 * @param filePath path to file
		 * @return true if export was sucessful, false otherwise
		 */
		bool exportToDot(const std::string& filePath)
		{
			return this->_exportToDot(filePath, [](auto& outputFile, auto& ss) { });
		}

#ifdef GRAPH_DEBUG
		std::string listedges()
		{
			std::stringstream ss;
			for (auto & m : vertices)
			{
				for (auto & n : m.second.outgoingEdges)
				{
					ss << "Edge from " << m.second.value << " to " << vertices.find(n.first)->second.value << "\n";
				}
			}
			return ss.str();
		}
#endif
	};
}

#undef GRAPH_DEBUG
