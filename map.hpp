/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <iostream>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

#define RED   0
#define BLACK 1
#define NIL   2

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */

	class Node {
	public:
		Node() { left = right = NULL; }
		Node(const int &color) { type = color; }
		Node(const value_type &value) 
		{ 
			key = value.first; 
			this->value = value.second; 
			parent = NULL; 
			type = RED;
			left = new Node(NIL);
			right = new Node(NIL);
		}
		void initialNode(const value_type &value)
		{
			this->key = value.first;
			this->value = value.second;
			this->type = RED;
			left = new Node(NIL);
			right = new Node(NIL);
		}
		bool operator==(const Node &right)
		{
			if (this->type == NIL)
				return NULL == right;
			else
				return this == right;
		}
		bool operator!=(const Node &right)
		{
			if (this->type == NIL)
				return NULL != right;
			else
				return this != right;
		}
	public:
		Key key;
		T value;
		Node *left;
		Node *right;
		Node *parent;
		int type;
	};
	class RBTree {
	private:
		Node *root;
		size_t _size;
		Compare comp;
	private:
		clearNode(Node *nd)
		{
			if (nd->left != NULL)
				clearNode(nd->left);
			if (nd->right != NULL)
				clearNode(nd->right);
			delete nd;
		}
	public:
		RBTree() { root = NULL; comp = c(); }
		size_t size() { return _size; }
		bool empty() { return root == NULL; }
		void clear()
		{
			
		}
		Node* begin() {
			Node* rNode = root;
			while (rNode->left != NULL && rNode->type != NIL)
			{
				rNode = rNode->left;
			}
			return rNode;
		}
		Node* end() {
			Node* rNode = root;
			while (rNode->right != NULL)
			{
				rNode = rNode->right;
			}
			return rNode;
		}
		Node* insert(const value_type &value) {
			if (root == NULL)
			{
				root = new Node(value);
				root->type = BLACK;
				return root;
			}
			Node* pNode = NULL;
			Node* cNode = root;
			while (cNode != NULL && cNode->type != NIL)
			{
				if (comp(cNode->value, value.first))
				{
					pNode = cNode;
					cNode = cNode->right;
				}
				else if (comp(value.first, cNode->value))
				{
					pNode = cNode;
					cNode = cNode->left;
				}
				else // value already exist
				{
					return NULL;
				}
			}
			// insert
			cNode->initialNode(value);
			cNode->parent = pNode;
			if (comp(value.first, pNode->key))
			{
				pNode->left = cNode;
			}
			else if (comp(pNode->key, value.first))
			{
				pNode->right = cNode;
			}

			// fixed up
			insert_fixed_up(cNode);
		}
		Node* find(Key key) {
			Node *ret = NULL;
			if (root == NULL)
				return ret;
			ret = root;
			while (root != NULL && root->type != NIL) {
				if (comp(key, ret->value))
				{
					root = root->left;
				}
				else if (comp(ret->value, key))
				{
					root = root->right;
				}
				else
				{
					break;
				}
			}
			return ret;
		}
		void remove(const value_type &value)
		{

		}

		void print()
		{
			printNode(root);
		}
		void printNode(Node* nd)
		{
			if (nd->left != NULL && nd->left->type != NIL)
				printNode(nd->left);
			std::cout << nd->key << " " << nd->value << " " << nd->type << std::endl;
			if (nd->right != NULL && nd->right->type != NIL)
				printNode(nd->right);
		}
		
	private:
		void insert_fixed_up(Node* cNode)
		{
			Node *pNode = cNode->parent;
			while (cNode != root && pNode->type == RED)
			{
				Node *gNode = pNode->parent;
				Node *uNode = NULL;
				// left
				if (pNode == gNode->left)
				{
					uNode = gNode->right;
					/* case-1:
					 * curr    left
					 * father  left  red
					 * uncle   right red
					 */
					if (uNode && uNode->type == RED)
					{
						pNode->type = uNode->type = BLACK;
						gNode->type = RED;
						cNode = gNode;
						pNode = gNode->parent;
						continue;
					}
					/* case-2:
				     * curr    left
					 * father  left  red
					 * uncle   right BLACK
					 */
					if (cNode == pNode->right)
					{
						Node *tmp;
						leftRotate(pNode);
						tmp = pNode;
						pNode = cNode;
						cNode = tmp;
					}
				    /* case-3:
					 * curr    right
					 * father  left  red
					 * uncle   right BLACK
					 */
					pNode->type = BLACK;
					gNode->type = RED;
					rightRotate(gNode);
				}
				else if (pNode == gNode->right)
				{
					uNode = gNode->left;
					if (uNode && uNode->type == RED)
					{
						pNode->type = uNode->type = BLACK;
						gNode->type = RED;
						cNode = gNode;
						pNode = gNode->parent;
						continue;
					}
					if (cNode == pNode->left)
					{
						Node *tmp;
						rightRotate(pNode);
						tmp = pNode;
						pNode = cNode;
						cNode = tmp;
					}
					pNode->type = BLACK;
					gNode->type = RED;
					leftRotate(gNode);
				}
				pNode = cNode->parent;
			}
			root->type = BLACK;
		}

		void leftRotate(Node*& parent)
		{
			Node* subR = parent->right;
			Node* subRL = subR->left;

			parent->right = subRL;
			if (subRL)
			{
				subRL->parent = parent;
			}

			subR->left = parent;
			subR->parent = parent->parent;
			parent->parent = subR;
			parent = subR;

			if (parent->parent == NULL)
			{
				root = parent;
			}
			else if (comp(parent->key, parent->parent->key))
			{
				parent->parent->left = parent;
			}
			else if (comp(parent->parent->key, parent->key))
			{
				parent->parent->right = parent;
			}
		}

		void rightRotate(Node*& parent)
		{
			Node* subL = parent->left;
			Node* subLR = subL->right;

			parent->left = subLR;
			if (subLR)
			{
				subLR->parent = parent;
			}

			subL->right = parent;
			subL->parent = parent->parent;
			parent->parent = subL;

			parent = subL;

			if (parent->parent == NULL)
			{
				root = parent;
			}
			else if (comp(parent->key, parent->parent->key))
			{
				parent->parent->left = parent;
			}
			else if (comp(parent->parent->key, parent->key))
			{
				parent->parent->right = parent;
			}
		}

	};
	class const_iterator;
	class iterator {
	private:
		Node* nd;
		value_type vt;
	public:
		iterator() {
			nd = NULL;
		}
		iterator(const iterator &other) {
			nd = other.nd;
			vt = other.vt;
		}
		iterator(Node* nd) {
			this->nd = nd;
			vt.first = nd->key;
			vt.second = nd->value;
		}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO iter++
		 */
		iterator operator++(int) 
		{
			Node *p = this->nd;
			if (p != NULL && p->type != NIL)
			{
				// find leftmost child at right sub-tree
				if (p->right != NULL)
				{
					p = p->right;
					while (p != NULL && p->type != NIL)
					{
						p = p->left;
					}
				}
				// if not find, parse parent if its a left sub-tree
				else if (p->parent != NULL && p->parent->type != NIL)
				{
					if (p->parent->left == p)
					{
						p = p->parent;
					}
				}
				// reach the position at right of end
				else
				{
					throw index_out_of_bound();
				}
				// update vt immediately
				updateValue();
				return iterator(p);
			}
			else
			{
				throw index_out_of_bound();
			}
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() 
		{
			if (nd->right != NULL)
			{
				nd = nd->right;
				while (nd->left != NULL && nd->left->type != NIL)
				{
					nd = nd->left;
				}
			}
			else if (nd->parent != NULL)
			{
				if (nd->parent->left == nd)
				{
					nd = nd->parent;
				}
			}
			else
			{
				throw index_out_of_bound();
			}
			updateValue();
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) 
		{
			Node *p = this->nd;
			if (p != NULL && p->type != NIL)
			{
				// find the rightmost child at left sub-tree
				if (p->left != NULL && p->left->type != NIL)
				{
					p = p->left;
					while (p->right != NULL && p->right->type != NIL)
					{
						p = p->right;
					}
				}
				// if not find, parse parent if its a right sub-tree
				else if (p->parent != NULL)
				{
					if (p->parent->right == p)
					{
						p = p->parent;
					}
				}
				else
				{
					throw index_out_of_bound();
				}
				updateValue();
				return iterator(p);
			}
			else
			{
				throw index_out_of_bound();
			}
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() 
		{
			if (nd->left != NULL)
			{
				nd = nd->left;
				while (nd->right != NULL && nd->right->type != NIL)
				{
					nd = nd->right;
				}
			}
			else if (nd->parent != NULL)
			{
				if (nd->parent->right == nd)
				{
					nd = nd->parent;
				}
			}
			else
			{
				throw index_out_of_bound();
			}
			updateValue();
			return *this;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const 
		{
			return value_type(nd->key, nd->value);
		}
		bool operator==(const iterator &rhs) const 
		{
			return nd == rhs.nd;
		}
		bool operator==(const const_iterator &rhs) const 
		{
			return nd == rhs.nd;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const 
		{
			return nd != rhs.nd;
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			return nd != rhs.nd;
		}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			return &vt;
		}
		iterator& operator=(const iterator &rhs) {
			nd = rhs.nd;
			vt = rhs.vt;
			return *this;
		}
	
	private:
		void updateValue()
		{
			vt.first = nd->key;
			vt.second = nd->value;
		}		
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			const Node* nd;
			value_type vt;
	    public:
			const_iterator() {
				nd == NULL;
			}
			const_iterator(Node* nd) {
				nd = nd;
			}
			const_iterator(const const_iterator &other) {
				nd = other.nd;
				vt = other.vt;
			}
			const_iterator(const iterator &other) {
				nd = other.nd;
				vt = other.vt;
			}
			const_iterator& operator=(const const_iterator &other) {
				nd = other.nd;
				vt = other.vt;
				return *this;
			}
			bool operator!=(const const_iterator &other) {
				return nd != other.nd;
			}
			bool operator==(const const_iterator &other) {
				return nd == other.nd;
			}
			value_type* operator->() {
				return &vt;
			}

			const_iterator operator++(int)
			{
				Node *p = this->nd;
				if (p != NULL && p->type != NIL)
				{
					// find leftmost child at right sub-tree
					if (p->right != NULL)
					{
						p = p->right;
						while (p != NULL && p->type != NIL)
						{
							p = p->left;
						}
					}
					// if not find, parse parent if its a left sub-tree
					else if (p->parent != NULL && p->parent->type != NIL)
					{
						if (p->parent->left == p)
						{
							p = p->parent;
						}
					}
					// reach the position at right of end
					else
					{
						throw index_out_of_bound();
					}
					// update vt immediately
					updateValue();
					return const_iterator(p);
				}
				else
				{
					throw index_out_of_bound();
				}
			}
			/**
			* TODO ++iter
			*/
			const_iterator & operator++()
			{
				if (nd->right != NULL)
				{
					nd = nd->right;
					while (nd->left != NULL && nd->left->type != NIL)
					{
						nd = nd->left;
					}
				}
				else if (nd->parent != NULL)
				{
					if (nd->parent->left == nd)
					{
						nd = nd->parent;
					}
				}
				else
				{
					throw index_out_of_bound();
				}
				updateValue();
				return *this;
			}
			/**
			* TODO iter--
			*/
			const_iterator operator--(int)
			{
				Node *p = this->nd;
				if (p != NULL && p->type != NIL)
				{
					// find the rightmost child at left sub-tree
					if (p->left != NULL && p->left->type != NIL)
					{
						p = p->left;
						while (p->right != NULL && p->right->type != NIL)
						{
							p = p->right;
						}
					}
					// if not find, parse parent if its a right sub-tree
					else if (p->parent != NULL)
					{
						if (p->parent->right == p)
						{
							p = p->parent;
						}
					}
					else
					{
						throw index_out_of_bound();
					}
					updateValue();
					return const_iterator(p);
				}
				else
				{
					throw index_out_of_bound();
				}
			}
			/**
			* TODO --iter
			*/
			const_iterator & operator--()
			{
				if (nd->left != NULL)
				{
					nd = nd->left;
					while (nd->right != NULL && nd->right->type != NIL)
					{
						nd = nd->right;
					}
				}
				else if (nd->parent != NULL)
				{
					if (nd->parent->right == nd)
					{
						nd = nd->parent;
					}
				}
				else
				{
					throw index_out_of_bound();
				}
				updateValue();
				return *this;
			}
	private:
		void updateValue()
		{
			vt.first = nd->key;
			vt.second = nd->value;
		}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};
	/**
	 * TODO two constructors
	 */
	map() {
		t = new RBTree();
	}
	map(const map &other) 
	{

	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) 
	{

	}
	/**
	 * TODO Destructors
	 */
	~map() {
		delete t;
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {}
	const T & at(const Key &key) const {}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() 
	{
		return iterator(t->begin());
	}
	const_iterator cbegin() const 
	{
		return const_iterator(t->begin());
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() 
	{ 
		return iterator(t->end());
	}
	const_iterator cend() const 
	{
		return const_iterator(t->end());
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const 
	{
		return t->empty();
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {}
	/**
	 * clears the contents
	 */
	void clear() {}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		Node* n = t->insert(value);
		if (n != NULL)
		{
			iterator iter(n);
			return pair<iterator, bool>(iter, true);
		}
		else
		{
			iterator iter = this->end();
			return pair<iterator, bool>(iter, false);
		}
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {}
	const_iterator find(const Key &key) const {}
	void print() { t->print(); }
private:
	RBTree* t;
};

}

#endif
