#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace Graph
{
	/**
	 * Struct for template specialization
	 */
	struct Unweight
	{
		bool operator==(const Unweight&) const
		{
			return true;
		}

		bool operator!=(const Unweight&) const
		{
			return false;
		}
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
		template<typename TV, typename TE>
		friend bool operator==(const AbstractGraph<TV,TE>&, const AbstractGraph<TV,TE>&);

		/**
		 * Vertex class
		 */
		class Vertex
		{
			friend class AbstractGraph<V,E>;

			template<typename TV, typename TE>
			friend class Graph;

			size_t id;
			V value;
			std::map<size_t, E> outgoingEdges;

			Vertex(size_t id, V value)
				:id(id), value(value)
			{}

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

			const std::map<size_t, E> & getOutgoingEdges() const
			{
				return outgoingEdges;
			}

			bool operator==(const Vertex& rhs) const
			{
				return value == rhs.value &&
				       id == rhs.id &&
				       outgoingEdges == rhs.outgoingEdges;
			}

			bool operator!=(const Vertex& rhs) const
			{
				return !(*this == rhs);
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
		AbstractGraph(bool directed = true)
			:directed(directed)
		{}

		/**
		* Destructor
		*/
		virtual ~AbstractGraph()
		{}

		/**
		* Add vertex
		* @param value value to be inserted
		* @return id of inserted vertex
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

		/**
		 * Method providing core saving functionality
		 * @param filePath path to file
		 * @param f function to be called on output file and edge
		 * @param fv function to be called on output file and vertex
		 * @return true if save was successful, false otherwise
		 */
		template<typename Func, typename FuncVert>
		bool _saveToFileBase(const std::string& filePath, Func f, FuncVert fv) const
		{
			std::ofstream outputFile(filePath);

			if(outputFile.is_open())
			{
				for(const auto& v : vertices)
				{
					outputFile << "id " << v.second.id << " ";
					fv(outputFile, v);
					outputFile << std::endl;

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

		/**
		 * Method extending _saveToFileBase functionality by (thanks to SFINAE) having
		 * different versions for std::string and other types
		 * @param filePath path to output file
		 * @param f function to call on output file and edge
		 */
		template<typename Func, typename TV = V>
		typename std::enable_if_t<!std::is_same<TV, std::string>::value, bool>
		_saveToFile(const std::string& filePath, Func f) const
		{
			return _saveToFileBase(filePath, f, [](auto& outputFile, auto& v)
			{
				outputFile << v.second.value;
			});
		}

		/**
		 * Method extending _saveToFileBase functionality by (thanks to SFINAE) having
		 * different versions for std::string and other types
		 * @param filePath path to output file
		 * @param f function to call on output file and edge
		 */
		template<typename Func, typename TV = V>
		typename std::enable_if_t<std::is_same<TV, std::string>::value, bool>
		_saveToFile(const std::string& filePath, Func f) const
		{
			return _saveToFileBase(filePath, f, [](auto& outputFile, auto& v)
			{
				outputFile << "\"" << v.second.value << "\"";
			});
		}

		/**
		 * Basic method for loading graph from file
		 * @param filePath path to file
		 * @param isWeighted true if graph is weighted
		 * @param f func which will be applied to stringstream and target id
		 * @param fv func which will be applied to stringstream and vertex value
		 * @return true if loading from file was successful
		 */
		template<typename Func, typename FuncVert>
		bool _loadFromFileBase(const std::string& filePath, bool isWeighted, Func f, FuncVert fv)
		{
			static_assert(std::is_default_constructible<V>::value, "Vertex type must be default constructible.");
			
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
						V vertValue;

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

						fv(ss, vertValue);

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

					ss.flush();
				}
			}

			if(!retValue)
			{
				vertices.clear();
			}

			return retValue;
		}

		/**
		 * Loads graph from file (SFINAE helps to choose correct type of loading method)
		 * @param filePath path to file
		 * @param isWeighted true if graph is weighted
		 * @param f func taking stringstream and index of end vertex as params and performs operation on those
		 */
		template<typename Func, typename TV = V>
		typename std::enable_if_t<!std::is_same<TV, std::string>::value, bool>
		_loadFromFile(const std::string& filePath, bool isWeighted, Func f)
		{
			return _loadFromFileBase(filePath, isWeighted, f, [](auto& ss, auto& v)
			{
				ss >> v;
			});
		}

		/**
		 * Loads graph from file (SFINAE helps to choose correct type of loading method)
		 * @param filePath path to file
		 * @param isWeighted true if graph is weighted
		 * @param f func taking stringstream and index of end vertex as params and performs operation on those
		 */
		template<typename Func, typename TV = V>
		typename std::enable_if_t<std::is_same<TV, std::string>::value, bool>
		_loadFromFile(const std::string& filePath, bool isWeighted, Func f)
		{
			return _loadFromFileBase(filePath, isWeighted, f, [](auto& ss, auto& v)
			{
				std::string result;
				getline(ss, result);

				size_t quotePos = result.find_first_of('"');
				size_t quotePosEnd = result.find_last_of('"');
				if(quotePos != std::string::npos && quotePosEnd != quotePos)
				{
					v = result.substr(quotePos + 1, quotePosEnd - quotePos - 1);
				}
			});
		}

		/**
		 * Base method for exporting graph to dot format
		 * @param filePath path to output file
		 * @param f function gets ofstream and edge value as params and (possibly) modifies them
		 * @return true if export was successful
		 */
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

						f(outputFile, e.second, v.second.id, e.first);

						outputFile << ";" << std::endl;
					}
					else
					{
						if(std::find(exportedPairs.begin(), exportedPairs.end(), std::make_pair(e.first, v.second.id)) == exportedPairs.end())
						{
							outputFile << v.second.id << " -- " << e.first;

							f(outputFile, e.second, v.second.id, e.first);

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
		 * Get count of vertices in graph
		 * @return count of vertices
		 */
		size_t getVerticesCount() const
		{
			return vertices.size();
		}

		/**
		 * Checks if graph is directed
		 * @return true if directed, false otherwise
		 */
		bool isDirected() const
		{
			return directed;
		}

		/**
		*  Get pairs of <id, value> of vertices
		* @return map, where key = id and value = value of given vertex
		*/
		std::map<size_t, V> getVerticesMap() const
		{
			std::map<size_t, V> result;
			for (auto& vert : vertices)
			{
				result.insert({vert.first, vert.second.getValue()});
			}
			return result;
		}

		/**
		 * Returns map with vertices ids as keys and default constructed element T as value
		 * @return map, where key = id and value = default constructed template parameter
		 */
		template<typename T>
		std::map<size_t, T> getVerticesMap() const
		{
			std::map<size_t, T> result;
			for (auto& vert: vertices)
			{
				result.insert({ vert.first, T() });
			}
			return result;
		}

		/**
		 * Returns position of edges in format {from, to} vertex id
		 * @param includeUndirEdgesTwice if set to true, each edge in undirected graph will be included twice
		 * @return vector of <source vertex, end vertex> pairs
		 */
		std::vector<std::pair<size_t, size_t>> getEdgesPositions(bool includeUndirEdgesTwice = false) const
		{
			std::vector<std::pair<size_t, size_t>> result;
			for (auto& vert : vertices)
			{
				for(auto& edge : vert.second.outgoingEdges)
				{
					if(!includeUndirEdgesTwice && !directed)
					{
						if(std::find(result.begin(), result.end(), std::make_pair(edge.first, vert.first)) != result.end())
						{
							continue;
						}
					}
					result.push_back({ vert.first, edge.first });
				}
			}
			return result;
		}

		/**
		 * Returns position of edges in format {from, to} vertex id and value
		 * @param includeUndirEdgesTwice if set to true, each edge in undirected graph will be included twice
		 * @return vector of tuples in format { from vertex id, source vertex id, edge value }
		 */
		std::vector<std::tuple<size_t, size_t, E>> getEdgesPositionsAndValues(bool includeUndirEdgesTwice = false) const
		{
			std::vector<std::tuple<size_t, size_t, E>> result;
			for (auto& vert : vertices)
			{
				for(auto& edge : vert.second.outgoingEdges)
				{
					if(!includeUndirEdgesTwice && !directed)
					{
						if(std::find(result.begin(), result.end(), std::make_tuple(edge.first, vert.first, edge.second))!= result.end())
						{
							continue;
						}
					}
					result.push_back(std::make_tuple(vert.first, edge.first, edge.second));
				}
			}
			return result;
		}

		/**
		 * Get edges from vertex
		 * @param source
		 * @return edges
		 */
		std::map<size_t, E> getEdgesFrom(size_t source) const
		{
			auto is_in = vertices.find(source);
			if (is_in == vertices.end())
			{
				throw std::invalid_argument("vertex id not found");
			}
			return vertices.find(source)->second.getOutgoingEdges();
		}

		/**
		 * Get value of given vertex
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
		* Set value of given vertex
		* @param vertex id of vertex
		* @return nothing
		*/
		void setVertexValue(size_t vertex, const V & value)
		{
			auto is_in = vertices.find(vertex);
			if (is_in != vertices.end())
			{
				is_in->second.setValue(value);
			}
		}

		/**
		* Set value of given vertex
		* @param vertex id of vertex
		* @return nothing
		*/
		void setVertexValue(size_t vertex, V && value)
		{
			auto is_in = vertices.find(vertex);
			if (is_in != vertices.end())
			{
				is_in->second.setValue(std::move(value));
			}
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
		bool adjacent(size_t from, size_t to) const
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
		std::string listVertices() const
		{
			std::stringstream ss;
			for (auto & m : vertices)
			{
				ss << m.second.id + 1 << "." << m.second.value << " ";
			}
			return ss.str();
		}

		void listVerticesToStream(std::ostream& stream = std::cout) const
		{
			stream << listVertices() << std::endl;
		}

		size_t getActualId() const
		{
			return total_id;
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

		template<typename TV, typename TE>
		friend class Graph;

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
		Graph(bool directed = true)
			:AbstractGraph<V,E>(directed)
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
			auto is_in_from = vertices.find(from);
			auto is_in_to = vertices.find(to);
			if (is_in_from == vertices.end() || is_in_to == vertices.end())
			{
				return;
			}

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
		const E& getEdgeValue(size_t from, size_t to) const
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
		* Get value of edge
		* @param from vertex from
		* @param to vertex to
		* @throws invalid_argument exception if either id is invalid
		* @return value of edge
		*/
		E& getEdgeValue(size_t from, size_t to)
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
		 * Saves graph to file in custom format
		 * @param filePath file to which the graph will be saved (if file exists, will be overwritten)
		 * @return true if save request was successful, false otherwise
		 */
		template<typename TE = E>
		typename std::enable_if_t<!std::is_same<TE, std::string>::value, bool>
		saveToFile(const std::string& filePath) const
		{
			return this->_saveToFile(filePath, [](auto& outputFile, auto& e)
			{
				outputFile << " " << e.second;
			});
		}

		/**
		 * Saves graph to file in custom format
		 * @param filePath file to which the graph will be saved (if file exists, will be overwritten)
		 * @return true if save request was successful, false otherwise
		 */
		template<typename TE = E>
		typename std::enable_if_t<std::is_same<TE, std::string>::value, bool>
		saveToFile(const std::string& filePath) const
		{
			return this->_saveToFile(filePath, [](auto& outputFile, auto& e)
			{
				outputFile << " " << "\"" << e.second << "\"";
			});
		}

		/**
		* Clears current graph content and loads graph from file in custom format (vertices/edges names will be loaded till first whitespace)
		* @param filePath path to file
		* @return true if loading was successful, false otherwise
		*/
		template<typename TE = E>
		typename std::enable_if_t<!std::is_same<TE, std::string>::value, bool>
		loadFromFile(const std::string& filePath)
		{
			static_assert(std::is_default_constructible<E>::value, "Edge type must be default constructible.");
			
			return this->_loadFromFile(filePath, true, [&,this](auto& ss, auto& targetId)
			{
				E edgeValue;
				ss >> edgeValue;
				vertices.rbegin()->second.outgoingEdges.insert({targetId, edgeValue});
			});
		}

		/**
		* Clears current graph content and loads graph from file in custom format (vertices/edges names will be loaded till first whitespace)
		* @param filePath path to file
		* @return true if loading was successful, false otherwise
		*/
		template<typename TE = E>
		typename std::enable_if_t<std::is_same<TE, std::string>::value, bool>
		loadFromFile(const std::string& filePath)
		{
			return this->_loadFromFile(filePath, true, [&,this](auto& ss, auto& targetId)
			{
				std::string result;
				getline(ss, result);
				size_t quotePos = result.find_first_of('"');
				size_t quotePosEnd = result.find_last_of('"');
				if(quotePos != std::string::npos && quotePosEnd != quotePos)
				{
					result = result.substr(quotePos + 1, quotePosEnd - quotePos - 1);
					vertices.rbegin()->second.outgoingEdges.insert({targetId, result});
				}
			});
		}

		/**
		 * Exports graph to dot format with ids as vertex names
		 * @param filePath path to file
		 * @param colorEdgesBetween path of ids of vertices whose between edges will be coloured (each vertex must be contained only once)
		 * @return true if export was sucessful, false otherwise
		 */
		bool exportToDot(const std::string& filePath, const std::vector<size_t>& colorEdgesBetween = std::vector<size_t>())
		{
			return this->_exportToDot(filePath, [&, this](auto& outputFile, auto& e, auto& startVertex, auto& endVertex)
			{
				std::stringstream ss;
				ss << e;

				auto startVrtxPos = std::find(colorEdgesBetween.begin(), colorEdgesBetween.end(), startVertex);
				std::string color;

				if(startVrtxPos != colorEdgesBetween.end())
				{
					if((startVrtxPos+1 != colorEdgesBetween.end() && *(startVrtxPos+1) == endVertex) ||
					        (!directed && startVrtxPos != colorEdgesBetween.begin() && *(startVrtxPos-1) == endVertex))
					{
						color = ",color=\"red\"";
					}
				}

				outputFile << "[label=\"" << ss.str() << "\",weight=\"" << ss.str()  << "\"" << color << "]";

				ss.flush();
			});
		}

		/**
		 * Exports graph to dot format with ids as vertex names
		 * @param filePath path to file
		 * @param colorEdgesBetween vector of edges pairs which will be coloured
		 * @return true if export was sucessful, false otherwise
		 */
		bool exportToDot(const std::string& filePath, const std::vector<std::pair<size_t, size_t>>& colorEdges)
		{
			return this->_exportToDot(filePath, [&, this](auto& outputFile, auto& e, auto& startVertex, auto& endVertex)
			{
				std::stringstream ss;
				ss << e;

				bool shouldBeColored = false;

				for(auto& edge : colorEdges)
				{
					if((edge.first == startVertex && edge.second == endVertex) ||
					        (!directed && edge.first == endVertex && edge.second == startVertex))
					{
						shouldBeColored = true;
						break;
					}
				}

				std::string color;

				if(shouldBeColored)
				{
					color = ",color=\"red\"";
				}

				outputFile << "[label=\"" << ss.str() << "\",weight=\"" << ss.str()  << "\"" << color << "]";

				ss.flush();
			});
		}

#ifdef GRAPH_DEBUG
		std::string listEdges() const
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

		void listEdgesToStream(std::ostream& stream = std::cout) const
		{
			stream << listEdges();
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
		Graph(bool directed = true)
			:AbstractGraph<V, Unweight>(directed)
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
		 * Saves graph to file in custom format
		 * @param filePath file to which the graph will be saved (if file exists, will be overwritten)
		 * @return true if save request was successful, false otherwise
		 */
		bool saveToFile(const std::string& filePath) const
		{
			return this->_saveToFile(filePath, [](auto&, auto&) { });
		}

		/**
		* Clears current graph content and loads graph from file in custom format (vertices/edges names will be loaded till first whitespace)
		* @param filePath path to file
		* @return true if loading was successful, false otherwise
		*/
		bool loadFromFile(const std::string& filePath)
		{
			return this->_loadFromFile(filePath, false, [&, this](auto&, auto& targetId)
			{
				vertices.rbegin()->second.outgoingEdges.insert({targetId, Unweight()});
			});
		}

		/**
		 * Exports graph to dot format with ids as vertex names
		 * @param filePath path to file
		 * @param colorEdgesBetween path of ids of vertices whose between edges will be coloured (each vertex must be contained only once)
		 * @return true if export was sucessful, false otherwise
		 */
		bool exportToDot(const std::string& filePath, const std::vector<size_t>& colorEdgesBetween = std::vector<size_t>())
		{
			return this->_exportToDot(filePath, [&, this](auto& outputFile, auto& e, auto& startVertex, auto& endVertex)
			{
				auto startVrtxPos = std::find(colorEdgesBetween.begin(), colorEdgesBetween.end(), startVertex);
				std::string color;

				if(startVrtxPos != colorEdgesBetween.end())
				{
					if((startVrtxPos+1 != colorEdgesBetween.end() && *(startVrtxPos+1) == endVertex) ||
					        (!directed && startVrtxPos != colorEdgesBetween.begin() && *(startVrtxPos-1) == endVertex))
					{
						color = "[color=\"red\"]";
					}
				}

				outputFile << color;
			});
		}

		/**
		 * Exports graph to dot format with ids as vertex names
		 * @param filePath path to file
		 * @param colorEdgesBetween vector of edges pairs which will be coloured
		 * @return true if export was sucessful, false otherwise
		 */
		bool exportToDot(const std::string& filePath, const std::vector<std::pair<size_t, size_t>>& colorEdges)
		{
			return this->_exportToDot(filePath, [&, this](auto& outputFile, auto& e, auto& startVertex, auto& endVertex)
			{
				bool shouldBeColored = false;

				for(auto& edge : colorEdges)
				{
					if((edge.first == startVertex && edge.second == endVertex) ||
					        (!directed && edge.first == endVertex && edge.second == startVertex))
					{
						shouldBeColored = true;
						break;
					}
				}

				std::string color;

				if(shouldBeColored)
				{
					color = "[color=\"red\"]";
				}

				outputFile << color;
			});
		}

#ifdef GRAPH_DEBUG
		std::string listEdges() const
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

		void listEdgesToStream(std::ostream& stream = std::cout) const
		{
			stream << listEdges();
		}
#endif
	};

	template<typename V, typename E>
	bool operator==(const AbstractGraph<V,E>& lhs, const AbstractGraph<V,E>& rhs)
	{
		return lhs.directed == rhs.directed && lhs.vertices == rhs.vertices;
	}

	template<typename V, typename E>
	bool operator!=(const AbstractGraph<V,E>& lhs, const AbstractGraph<V,E>& rhs)
	{
		return !(lhs == rhs);
	}
}

#undef GRAPH_DEBUG
