#include <functional> // less
#include <initializer_list>
#include <vector>
#include <memory>
#include <cmath>

#ifndef CPP14_HEAP
#define CPP14_HEAP

namespace Graph {
	template<typename V, typename E>
	std::pair<std::map<size_t, E>, std::map<size_t, size_t>> dijkstraAll(const Graph<V, E>& graph, size_t source, E infinity = std::numeric_limits<E>::max());
}


	/*
	The heap type, parametrized by the type of elements and by the type that
	defines a comparator. The comparator is expected to be stateless and default
	constructible, so it does not need to be stored in the Heap. The ordering of
	the heap depends on the type supplied to Compare, for example, with std::less<
	T > it creates a max-heap (this behaviour mirrors the one of
	std::priority_queue), see the end of this file for some pre-defined heaps with
	comparator.

	Unlike std::priority_queue, this Heap type supports in-place update of a value.
	For this reason it defines the Handle member type which represents a handle to
	an element of the heap. This handle needs to remain valid until the element is
	removed from the heap, or the heap is destructed. The handle can be used to
	update or remove the value from the heap, and to read the value. However, Heap
	does not define iterators.

	You should implement Heap as standard binary heap in an array
	<https://en.wikipedia.org/wiki/Binary_heap>, using std::vector as the
	underlying container (for the purpose of complexity analysis, you can consider
	push_back to have constant complexity, although in fact it is only amortized
	constant). For each of the operations there is a complexity requirement that you
	have to meet. You are free to choose the type of the elements stored in the vector
	and you can use any additional data structures that you wish.
	 */
	template< typename T, typename Compare >
	struct Heap {

		
		template<typename V, typename E>
		friend std::pair<std::map<size_t, E>, std::map<size_t, size_t>> Graph::dijkstraAll(const Graph<V, E>& graph, size_t source, E infinity);
		
		using value_type = T;

		struct Node {
			value_type data;
			int pos;
		};

		/*
		Handle has to be default constructible, copy and move constructible, copy
		and move assignable, and support equality. Otherwise it is an opaque data
		structure (it does not define any public member functions or data).
		 */
		struct Handle {
		private:
			friend struct Heap;
			Node * _node;
		public:
			Handle()
			{}

			Handle(const Handle& other) // copy constructor
			{
				_node = other._node;
			}

			Handle(Handle&& other) : _node(std::move(other._node)) // move constructor
			{}

			Handle& operator=(Handle other)
			{
				std::swap(_node, other._node);
				return *this;
			}
			bool operator==(const Handle &o) const {
				return _node == o._node;
			}

			bool operator!=(const Handle &o) const {
				return !(*this == o);
			}

			~Handle() {
			}
		};

		// O(1). Heap is default constructible in constant time.
		Heap()
		{}

		// O(n) where n is the size of other. Heap is copy constructible. Copy does
		// not affect other (and its handles) in any way, the handles from other
		// should not be used with this.
		Heap(const Heap & other)
		{
			int i = 0;
			for (auto it = other.heap_imp.begin(); it != other.heap_imp.end(); it++)
			{
				Handle h;
				h._node = new Node();
				h._node->data = (*it)._node->data;
				h._node->pos = i;
				heap_imp.push_back(h);
				i++;
			}
		}

		// O(1). Heap is move constructible. After the move, no operations other
		// than destruction or assignment should be done with other and all handles
		// for other should now be valid handles for this.
		Heap(Heap && other) :heap_imp(std::move(other.heap_imp))
		{}

		// O(n) where n is the distance from begin to end. Heap can be created from
		// an iterator range in linear time (provided that the iterator has
		// constant time dereference and increment).
		template< typename Iterator >
		Heap(Iterator begin, Iterator end)
		{
			int i = 0;
			for (auto it = begin; it != end; it++)
			{
				Handle h;
				h._node = new Node();
				h._node->data = (*it);
				h._node->pos = i;
				heap_imp.push_back(h);
				i++;
			}
			for (int i = int((heap_imp.size()) / 2) - 1; i >= 0; i--)
			{
				heapify(i);
			}
		}

		// O(n) where n is the number of elements in list.
		Heap(std::initializer_list< T > list) :Heap(list.begin(), list.end())
		{}

		// O(n) where n is the size of other. Heap is copy assignable. Assignment
		// does not affect other in any way, the handles from other should not be
		// used with this.
		Heap &operator=(const Heap & other)
		{
			Heap temp(other);
			swap(temp);
			return *this;
		}

		// O(1). Heap is move assignable. After the move-assign, no operations
		// other than destruction or assignment should be done with other and all
		// handles for other should now be valid handles for this.
		Heap &operator=(Heap && other)
		{
			swap(other);
			return *this;
		}

		// O(n). Invalidates all handles to this.
		~Heap()
		{
			for (int i = 0; i < int(heap_imp.size()); i++)
			{
				delete (heap_imp[i])._node;
			}
			heap_imp.clear();
		}

		// O(1). Heap is swappable. After the swap all handles for this should be
		// valid handles for other and vice versa.
		void swap(Heap &other)
		{
			std::swap(heap_imp, other.heap_imp);
		}

		// O(1). Get the top (e.g. maximal for max-heap) element of the heap.
		const T &top() const
		{
			return ((heap_imp[0])._node->data);
		}

		// O(1). Get handle to the top element of the heap.
		Handle topHandle() const
		{
			return heap_imp[0];
		}

		// O(log n). Remove the top element from the heap. This invalidates handle
		// to the removed element, handles to other elements must remain valid.
		void pop()
		{
			if (heap_imp.size() > 0)
			{
				if (heap_imp.size() > 1) std::swap(heap_imp[0], heap_imp[heap_imp.size() - 1]);//
				heap_imp[heap_imp.size() - 1]._node->pos = -1;
				heap_imp[0]._node->pos = 0;
				delete (heap_imp[heap_imp.size() - 1])._node;
				heap_imp.pop_back();
				heapify(0);
			}
		}

		// O(log n). Insert an element and return a handle for it. No handles are
		// invalidated.
		Handle insert(const T & value)
		{
			return _insert(value);
		}

		// O(log n). A version of insert which moves the element into the
		// underlying container instead of copying it.
		Handle insert(T && value)
		{
			return _insert(std::move(value));
		}

		// O(1). Get value of an element represented by the given handle.
		// Precondition: h must be a valid handle for this.
		const T &get(const Handle &h) const
		{
			return (h._node->data);
		}

		// O(log n). Update the value represented by the given handle (replace it
		// by the new value).
		// Precondition: h must be a valid handle for this.
		void update(const Handle &h, const T &value)
		{
			_update(h, value);
		}

		// O(log n). A version of update which uses move assign instead of copy
		// assign to replace the value.
		void update(const Handle &h, T &&value)
		{
			_update(h, std::move(value));
		}

		// O(log n). Erase the value represented by the given handle from the heap.
		// Invalidates h, but does not invalidate handles to other elements.
		void erase(const Handle &h)
		{
			int replacement = h._node->pos;
			if (int(heap_imp.size() - 1) != replacement) std::swap(heap_imp[h._node->pos], heap_imp[heap_imp.size() - 1]);//
			heap_imp[h._node->pos]._node->pos = replacement;
			delete (heap_imp[heap_imp.size() - 1])._node;
			heap_imp.pop_back();
			Compare comparator;
			if (replacement < int(heap_imp.size()))
			{
				if (comparator(heap_imp[(replacement - 1) / 2]._node->data, heap_imp[replacement]._node->data))
				{
					bubble_up(replacement);
				}
				else
				{
					heapify(replacement);
				}
			}
		}

		// O(1). Get size (number of elements) of the heap.
		size_t size() const
		{
			return heap_imp.size();
		}

		// O(1). Is the heap empty?
		bool empty() const
		{
			return heap_imp.empty();
		}

	private:
		std::vector<Handle> heap_imp;

		void bubble_up(int node)
		{
			Compare comparator;
			int child = node;
			int parent = int(std::floor((child - 1) / 2));
			while (comparator(heap_imp[parent]._node->data, heap_imp[child]._node->data) && child > 0)
			{
				std::swap(heap_imp[parent], heap_imp[child]);
				std::swap((heap_imp[parent])._node->pos, (heap_imp[child])._node->pos);
				child = parent;
				parent = int(std::floor((child - 1) / 2));
			}
		}

		void heapify(int node)
		{
			Compare comparator;
			int left = 2 * node + 1;
			int right = 2 * node + 2;
			int m = node;
			if (left < int(heap_imp.size()) && comparator((heap_imp[m])._node->data, (heap_imp[left])._node->data)) { m = left; }
			if (right < int(heap_imp.size()) && comparator((heap_imp[m])._node->data, (heap_imp[right])._node->data)) { m = right; }
			if (m != node)
			{
				std::swap(heap_imp[node], heap_imp[m]);
				std::swap((heap_imp[node])._node->pos, (heap_imp[m])._node->pos);
				heapify(m);
			}
		}

		template <typename X> Handle _insert(X && value)
		{
			Handle tr;
			tr._node = new Node();
			tr._node->data = std::forward<X>(value);
			tr._node->pos = heap_imp.size();
			heap_imp.push_back(tr);
			bubble_up(heap_imp.size() - 1);
			return tr;
		}

		template <typename X> void _update(const Handle &h, X && value)
		{
			(heap_imp[h._node->pos])._node->data = std::forward<X>(value);
			Compare comparator;
			if (comparator((heap_imp[(h._node->pos - 1) / 2])._node->data, h._node->data))
			{
				bubble_up(h._node->pos);
			}
			else
			{
				heapify(h._node->pos);
			}
		}
	};

	// O(n log n). Assigns values of the heap in the sorted order (top first) to the output
	// iterator. The complexity should hold if both increment and assignment to o
	// can be done in constant time.
	template< typename OutputIterator, typename T, typename Cmp >
	void copySorted(Heap< T, Cmp > heap, OutputIterator o)
	{
		while (heap.size() > 0)
		{
			(*o) = heap.top();
			heap.pop();
			o++;
		}
	}

	// O(n log n). Create sorted vector form the given heap.
	template< typename T, typename Cmp >
	std::vector< T > toSortedVector(Heap< T, Cmp > heap)
	{
		std::vector<T> tr;
		while (heap.size() > 0)
		{
			tr.push_back(heap.top());
			heap.pop();
		}
		return tr;
	}

	// O(1). Swaps two heaps. See Heap::swap for more.
	template< typename T, typename Cmp >
	void swap(Heap< T, Cmp > & a, Heap< T, Cmp > & b) { a.swap(b); }

	// examples of concrete heaps

	template< typename T >
	using MaxHeap = Heap< T, std::less< T > >;

	template< typename T >
	using MinHeap = Heap< T, std::greater< T > >;

	template< typename A, typename B, typename Cmp >
	struct PairCompare {
		bool operator()(const std::pair< A, B > & a, const std::pair< A, B > & b) const {
			return Cmp()(a.first, b.first);
		}
	};

	template< typename P, typename V, typename Cmp = std::less< P > >
	using PriorityQueue = Heap< std::pair< P, V >, PairCompare< P, V, Cmp > >;


#endif // CPP14_HEAP

