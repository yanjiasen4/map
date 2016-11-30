#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"
#include <climits>
#include <cstddef>
#include <memory>

using namespace std;

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator {
	private:
		T * t;
		size_t index;
	public:
		explicit iterator() { }
		explicit iterator(T *tt, const size_t &ind) :t(tt) { index = ind; }
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			iterator it = *this;
			it.t += n;
			it.index += n;
			return it;
		}
		iterator operator-(const int &n) const {
			iterator it = *this;
			it.t -= n;
			it.index = n;
			return it;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invalid_iterator.
		int operator-(const iterator &rhs) const {
			if (this->t - this->index != rhs.t - rhs.index)
				throw invalid_iterator();
			else
				return this->t - rhs.t;
		}
		iterator operator+=(const int &n) {
			return this + n;
		}
		iterator operator-=(const int &n) {
			return this - n;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator it = *this;
			it->t++;
			it->index++;
			return it;
		}
		iterator& operator++() {
			this->t++;
			this->index++;
			return *this;
		}
		iterator operator--(int) {
			iterator it = *this;
			it->t--;
			it->index--;
			return it;
		}
		iterator& operator--() {
			this->t--;
			this->index--;
			return *this;
		}

		T& operator*() const{
			return *t;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return t == rhs.t;
		}
		bool operator==(const const_iterator &rhs) const {
			return t = rhs.t;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return t != rhs.t;
		}
		bool operator!=(const const_iterator &rhs) const {
			return t != rhs.t;
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator {
	public:
		const T * t;
		size_t index;
	public:
		const_iterator() {}
		const_iterator(T *tt, size_t ind) :t(tt) { index = ind; }
		/**
		* return a new iterator which pointer n-next elements
		*   even if there are not enough elements, just return the answer.
		* as well as operator-
		*/
		const_iterator operator+(const int &n) const {
			const_iterator it = *this;
			it.t += n;
			it.index += n;
			return it;
		}
		const_iterator operator-(const int &n) const {
			const_iterator it = *this;
			it.t -= n;
			it.index -= n;
			return it;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invalid_iterator.
		int operator-(const const_iterator &rhs) const {
			if (this->t - this->index != rhs.t - rhs.index)
				throw invalid_iterator();
			else
				return this->t - rhs.t;
		}
		const_iterator operator+=(const int &n) {
			return this + n;
		}
		const_iterator operator-=(const int &n) {
			return this - n;
		}
		/**
		* TODO iter++
		*/
		const_iterator operator++(int) {
			const_iterator it = *this;
			it->t++;
			it->index++;
			return it;
		}
		const_iterator& operator++() {
			this->t++;
			this->index++;
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator it = *this;
			it->t--;
			it->index--;
			return it;
		}
		const_iterator& operator--() {
			this->t--;
			this->index--;
			return *this;
		}

		const T& operator*() const {
			return *t;
		}
		/**
		* a operator to check whether two iterators are same (pointing to the same memory).
		*/
		bool operator==(const iterator &rhs) const {
			return t == rhs.t;
		}
		bool operator==(const const_iterator &rhs) const {
			return t == rhs.t;
		}
		/**
		* some other operator for iterator.
		*/
		bool operator!=(const iterator &rhs) const {
			return t != rhs.t;
		}
		bool operator!=(const const_iterator &rhs) const {
			return t != rhs.t;
		}
	};
	/**
	 * TODO Constructs
	 * Atleast three: default constructor, copy constructor and a constructor for std::vector
	 */
	vector() {
		_size = 0;
		_capacity = DEFAULT_CAPACITY;
		_data = al.allocate(_capacity);
	}
	vector(const vector &other) {
		_size = other.size();
		_capacity = other.capacity();
		_data = al.allocate(_capacity);
		for (int i = 0; i < _size; i++)
		{
			al.construct(_data+i, other[i]);
		}
	}
	/**
	 * TODO Destructor
	 */
	~vector() {
		if (_data != NULL)
		{
			destroy();
			al.deallocate(_data, _capacity);
			_data = NULL;
		}
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other) {
		_size = other.size();
		_capacity = other.capacity();
		if (_data == NULL)
		{
			_data = al.allocate(_capacity);
		}
		for (int i = 0; i < _size; i++)
		{
			al.construct(_data+i, other[i]);
		}
		return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos){
		if (pos >= _size || pos < 0)
			throw index_out_of_bound();
		else
			return *(_data + pos);
	}
	const T & at(const size_t &pos) const {
		if (pos >= _size || pos < 0)
			throw index_out_of_bound();
		else
			return *(_data + pos);
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
		if (pos >= _size || pos < 0)
			throw index_out_of_bound();
		else
			return _data[pos];
	}
	const T & operator[](const size_t &pos) const {
		if (pos >= _size || pos < 0)
			throw index_out_of_bound();
		else
			return _data[pos];
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
		if (_size == 0)
			throw container_is_empty();
		else
			return *_data;
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
		if (_size == 0)
			throw container_is_empty();
		else
			return _data[_size-1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		iterator it(_data, 0);
		return it;
	}
	const_iterator cbegin() const {
		const_iterator cit(_data, 0);
		return cit;
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		iterator it(_data + _size, _size);
		return it;
	}
	const_iterator cend() const {
		const_iterator cit(_data + _size, _size);
		return cit;
	}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const { return _size == 0; }
	/**
	 * returns the number of elements
	 */
	size_t size() const { return _size; }
	/**
	 * returns the number of elements that can be held in currently allocated storage.
	 */
	size_t capacity() const { return _capacity; }
	/**
	 * clears the contents
	 */
	void clear() {
		if (_data != NULL)
		{
			destroy();
			al.deallocate(_data, _capacity);
		}
		_size = 0;
		_capacity = DEFAULT_CAPACITY;
		_data = al.allocate(_capacity);
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
		size_t dpos = pos - begin();
		_size++;
		if (_size > _capacity)
		{
			T * oldData = _data;
			_data = al.allocate(2*_capacity);
			for (int i = 0; i < dpos; i++)
			{
				al.construct(_data+i, oldData[i]);
			}
			al.construct(_data+dpos, value);
			for (int i = dpos + 1; i < _size; i++)
			{
				al.construct(_data+i+1, oldData[i]);
			}
			destroy(oldData, _size - 1);
			al.deallocate(oldData, _capacity);
			_capacity *= 2;
		}
		else
		{
			al.construct(_data+_size, value);
			for (int i = _size - 1; i >= dpos; i--)
			{
				_data[i + 1] = _data[i];
			}
			_data[dpos] = value;
		}
		iterator it(_data + dpos, dpos);
		return it;
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value is true
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {
		if (ind > _size)
			throw index_out_of_bound();
		if (_size == 0 && _data == NULL) {
			_data = new T(value);
		}
		else
		{
			_size++;
			// need resize vector
			if (_size > _capacity)
			{
				T * oldData = _data;
				_data = al.allocate(2*_capacity);
				for (int i = 0; i < ind; i++)
				{
					al.construct(_data+i, oldData[i]);
				}
				_data[ind] = value;
				for (int i = ind + 1; i < _size; i++)
				{
					al.construct(_data+i+1, oldData[i]);
				}
				destroy(oldData, _size - 1);
				al.deallocate(oldData, _capacity);
				_capacity *= 2;
			}
			else
			{
				al.construct(_data+_size, value);
				for (int i = _size - 1; i > ind; i--)
				{
					_data[i + 1] = _data[i];
				}
				al.construct(_data + ind, value);
			}
		}
		iterator it(_data + ind, ind);
		return it;
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
		size_t dpos = pos - begin();
		iterator it;
		for (int i = dpos; i < _size - 1; i++)
		{
			_data[i] = _data[i + 1];
		}
		if (dpos == _size - 1)
			iterator it = end();
		else
			iterator it(_data + dpos, dpos);
		_size--;
		al.destroy(_data + _size);
		return it;
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) {
		iterator it;
		if (ind >= _size)
			throw index_out_of_bound();
		else {
			for (int i = ind; i < _size - 1; i++)
			{
				_data[i] = _data[i + 1];
			}
			if (ind == _size)
				iterator it = end();
			else
				iterator it(_data + ind, ind);
			_size--;
			al.destroy(_data + _size);
			return it;
		}
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
		if (_size >= _capacity)
			reallocate();
		al.construct(_data + _size, value);
		_size++;
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
		if (_size == 0)
			throw container_is_empty();
		else
		{
			size_t ind = _size;
			erase(ind-1);
		}
	}
private:
	void destroy() {
		if (_size == 0) return;
		T *buff = _data;
		for (int i = 0; i < _size; i++)
		{
			al.destroy(buff++);
		}
	}

	void destroy(T *p, size_t s) {
		if (s == 0 || s > _size) return;
		T *buff = p;
		for (int i = 0; i < s; i++)
		{
			al.destroy(buff++);
		}
	}

	void reallocate() {
		size_t newCapacity = 2 * _capacity;
		T* newData = al.allocate(newCapacity);
		for (int i = 0; i < _size; i++)
		{
			al.construct(newData + i, _data[i]);
		}
		for (T *p = _data + _size; p != _data; p--)
			al.destroy(--p);
		if (_data != NULL)
			al.deallocate(_data, _capacity);
		_data = newData;
		_capacity = newCapacity;
	}

protected:
	std::allocator<T> al;
private:
	T * _data;
	int _capacity;
	size_t _size;
	static const size_t DEFAULT_CAPACITY = 16;
};


}

#endif
