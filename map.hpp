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
			Node() { vt = NULL; left = right = NULL; type = NIL; }
			Node(Node *other)
			{
				if (other->vt != NULL)
					vt = new value_type(*other->vt);
				else
					vt = NULL;
				type = other->type;
				left = right = parent = NULL;
			}
			Node(const int &color) { vt = NULL; type = color; left = right = nullptr; }
			Node(const value_type &value)
			{
				vt = new value_type(value);
				parent = NULL;
				type = RED;
				left = new Node(NIL);
				right = new Node(NIL);
				left->parent = right->parent = this;
			}
			~Node()
			{
				if (vt)
					delete vt;
				vt = NULL;
				type = NIL;
				if (left)
				{
					left = NULL;
				}
				if (right)
				{
					right = NULL;
				}
				if(parent)
					parent = NULL;
			}
			void initialNode(const value_type &value)
			{
				vt = new value_type(value);
				this->type = RED;
				if(!left)
					left = new Node(NIL);
				else
					left->type = NIL;
				if(!right)
					right = new Node(NIL);
				else
					right->type = NIL;
				left->parent = right->parent = this;
			}
			//bool operator==(const Node &right)
			//{
			//	if(this != NULL)
			//		if (this->type == NIL)
			//			return NULL == right;
			//	return this == right;
			//}
			//bool operator!=(const Node &right)
			//{
			//	if (this != NULL)
			//		if(this->type == NIL)
			//			return NULL != right;
			//	return this != right;
			//}
			value_type* operator->() const noexcept {
				return vt;
			}
		public:
			value_type *vt;
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
		public:
			RBTree()
			{
				root = NULL;
				comp = Compare();
				_size = 0;
			}
			RBTree(const RBTree &other)
			{
				copyTree(other.root);
				_size = other.size();
			}
			RBTree& operator=(const RBTree &other)
			{
				copyTree(other.root);
				_size = other.size();
				return *this;
			}
			~RBTree()
			{
				if (root)
					clear();
			}
			size_t size()
			{
				return _size;
			}
			bool empty()
			{
				return _size == 0;
			}
			void clear()
			{
				if(root)
					clearNode(root);
				_size = 0;
				root = NULL;
			}
			Node* begin() {
				Node* rNode = root;
				if (root == NULL || root->type == NIL)
				{
					return rNode;
				}
				while (rNode->left != NULL && rNode->type != NIL)
				{
					rNode = rNode->left;
				}
				return rNode->parent;
			}
			Node* end() {
				Node* rNode = root;
				if (root == NULL || root->type == NIL)
				{
					return rNode;
				}
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
					_size++;
					return root;
				}
				Node* pNode = NULL;
				Node* cNode = root;
				while (cNode != NULL && cNode->type != NIL)
				{
					if (comp(cNode->vt->first, value.first))
					{
						pNode = cNode;
						cNode = cNode->right;
					}
					else if (comp(value.first, cNode->vt->first))
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
				if (comp(value.first, pNode->vt->first))
				{
					pNode->left = cNode;
				}
				else if (comp(pNode->vt->first, value.first))
				{
					pNode->right = cNode;
				}

				// fixed up
				insert_fixed_up(cNode);
				_size++;
				return cNode;
			}
			Node* find(Key key) {
				Node *ret = NULL;
				if (root == NULL)
					return ret;
				ret = root;
				while (ret != NULL && ret->vt && ret->type != NIL) {
					if (comp(key, ret->vt->first))
					{
						ret = ret->left;
					}
					else if (comp(ret->vt->first, key))
					{
						ret = ret->right;
					}
					else
					{
						break;
					}
				}
				if (ret->vt && !(comp(ret->vt->first, key) && comp(key, ret->vt->first)))
					return ret;
				else
					return NULL;
			}
			void remove(const value_type &value)
			{
				Node* tNode;
				Node* sNode;
				Node* cNode = find(value.first);
				if (cNode == NULL)
					return;
				if (cNode->left->type == NIL || cNode->right->type == NIL)
				{
					tNode = cNode;
				}
				else
				{
					tNode = nextNode(cNode);
				}

				if (tNode->left && tNode->left->type != NIL)
				{
					sNode = tNode->left;
					tNode->left = NULL;
				}
				else
				{
					sNode = tNode->right;
					tNode->right = NULL;
				}

				if (tNode == root)
				{
					root = sNode;
				}
				else
				{
					if (tNode == tNode->parent->left)
						tNode->parent->left = sNode;
					else
						tNode->parent->right = sNode;
				}
				sNode->parent = tNode->parent;

				if (tNode != cNode)
				{
					if (tNode->type != NIL)
					{
						delete cNode->vt;
						cNode->vt = new value_type(*tNode->vt);
					}
				}
				if (tNode->type == BLACK)
					delete_fixed_up(sNode);
				if (tNode->left)
					delete tNode->left;
				else if(tNode->right)
					delete tNode->right;
				delete tNode;
				tNode = NULL;
				if (root->type == NIL)
				{
					delete root;
					root = NULL;
				}
				_size--;
			}

			void print()
			{
				printNode(root);
			}
			void printNode(Node* nd)
			{
				if (nd->left != NULL && nd->left->type != NIL)
					printNode(nd->left);
				std::cout << nd->vt->first << " " << nd->vt->second << " " << nd->type << std::endl;
				if (nd->right != NULL && nd->right->type != NIL)
					printNode(nd->right);
			}

		private:
			Node* nextNode(Node *cNode)
			{
				Node* t;
				if (cNode->type != NIL && cNode->right->type != NIL)
				{
					t = cNode->right;
					while (t->left && t->left->type != NIL)
						t = t->left;
					return t;
				}
				t = cNode->parent;
				while (t->type != NIL && cNode == t->parent->right)
				{
					cNode = t;
					t = t->parent;
				}
				return t;
			}
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

			void delete_fixed_up(Node* cNode)
			{
				Node *wNode;
				Node *pNode;
				// set black if its red
				if (cNode->type == RED)
					cNode->type = BLACK;
				else
				{
					while (cNode != root && cNode->type != RED)
					{
						pNode = cNode->parent;
						// left case
						if (cNode == cNode->parent->left)
						{
							wNode = cNode->parent->right;
							// case1: sibling is red. change to case 2-4
							if (wNode->type == RED)
							{
								wNode->type = BLACK;
								cNode->parent->type = RED;
								leftRotate(pNode);
								wNode = cNode->parent->right;
							}
							// case2:
							if ((!wNode->left || wNode->left->type != RED) && (!wNode->right || wNode->right->type != RED))
							{
								if(wNode->type != NIL)
									wNode->type = RED;
								cNode = wNode->parent;
							}
							else
							{
								// case 3: to 4
								if (!wNode->right || wNode->right->type != RED)
								{
									if (wNode->left && wNode->left->type != NIL)
										wNode->left->type = BLACK;
									if (wNode->type != NIL)
										wNode->type = RED;
									rightRotate(wNode);
									wNode = cNode->parent->right;
								}
								// case 4:
								wNode->type = cNode->parent->type;
								cNode->parent->type = BLACK;
								if (wNode->right && wNode->right->type != NIL)
									wNode->right->type = BLACK;
								pNode = cNode->parent;
								leftRotate(pNode);
								break;
							}
						}
						// right case
						else
						{
							wNode = cNode->parent->left;
							if (wNode->type == RED)
							{
								wNode->type = BLACK;
								cNode->parent->type = RED;
								rightRotate(pNode);
								wNode = cNode->parent->left;
							}
							if ((!wNode->left || wNode->left->type != RED) && (!wNode->right || wNode->right->type != RED))
							{
								if (wNode->type != NIL)
									wNode->type = RED;
								cNode = wNode->parent;
							}
							else
							{
								if (!wNode->left || wNode->left->type != RED)
								{
									if (wNode->right && wNode->right->type != NIL)
										wNode->right->type = BLACK;
									if (wNode->type != NIL)
										wNode->type = RED;
									leftRotate(wNode);
									wNode = cNode->parent->left;
								}
								wNode->type = cNode->parent->type;
								cNode->parent->type = BLACK;
								if (wNode->left && wNode->left->type != NIL)
									wNode->left->type = BLACK;
								pNode = cNode->parent;
								rightRotate(pNode);
								break;
							}
						}
					} // end while
				} // end else
				if (cNode)
				{
					if (cNode->type != NIL)
						cNode->type = BLACK;
					else
						cNode->type = NIL;
				}
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
				else if (parent->parent->left == parent->left)
				{
					parent->parent->left = parent;
				}
				else if (parent->parent->right == parent->left)
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
				else if (parent->parent->left == parent->right)
				{
					parent->parent->left = parent;
				}
				else if (parent->parent->right == parent->right)
				{
					parent->parent->right = parent;
				}
			}

			Node *sibling(Node* cNode)
			{
				if (cNode == NULL || cNode->parent == NULL)
					return NULL;
				if (cNode == cNode->parent->left)
					return cNode->parent->right;
				else
					return cNode->parent->left;
			}
		public:
			void copyTree(RBTree *t)
			{
				if (t == NULL || t->root == NULL)
					return;
				copyNode(root, t->root);
				_size = t->size();
			}
			void copyTree(Node *rt)
			{
				if (rt == NULL)
					return;
				copyNode(root, rt);
			}
		private:
			void copyNode(Node *&t, Node *&r)
			{
				if (r != NULL)
				{
					if(!t)
						t = new Node(r);
					if (r->type != NIL)
					{
						copyNode(t->left, r->left);
						t->left->parent = t;
						copyNode(t->right, r->right);
						t->right->parent = t;
					}
				}
			}

			void clearNode(Node *p)
			{
				if (p->left)
					clearNode(p->left);
				if (p->right)
					clearNode(p->right);
				delete p;
			}

		};
		class const_iterator;
		class iterator {
		public:
			Node* nd;
		public:
			iterator() {
				nd = NULL;
			}
			iterator(const iterator &other) {
				nd = other.nd;
			}
			iterator(Node* nd) {
				this->nd = nd;
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
				Node* t = this->nd;
				Node* cNode = this->nd;
				Node* end = getEnd();
				if (cNode == getEnd())
					throw index_out_of_bound();
				if (cNode->right == end)
				{
					this->nd = end;
					return iterator(end);
				}
				if (cNode->right->type != NIL)
				{
					t = cNode->right;
					while (t->left->type != NIL)
						t = t->left;
					this->nd = t;
					return iterator(cNode);
				}
				if (cNode->parent)
				{
					t = cNode->parent;
					while (t->parent && t->type != NIL && cNode == t->right)
					{
						cNode = t;
						t = t->parent;
					}
				}
				if (t)
				{
					cNode = this->nd;
					this->nd = t;
					return iterator(cNode);
				}
				else
					throw index_out_of_bound();
			}
			/**
			* TODO ++iter
			*/
			iterator & operator++()
			{
				Node* t = this->nd;
				Node* cNode = this->nd;
				Node* end = getEnd();
				if (cNode == end)
					throw index_out_of_bound();
				if (cNode->right == end)
				{
					this->nd = end;
					return *this;
				}
				if (cNode->right->type != NIL)
				{
					t = cNode->right;
					while (t->left->type != NIL)
						t = t->left;
					this->nd = t;
					return *this;
				}
				if (cNode->parent)
				{
					t = cNode->parent;
					while (t->type != NIL && cNode == t->right)
					{
						cNode = t;
						t = t->parent;
					}
				}
				this->nd = t;
				if (t->type != NIL)
					return *this;
				else
					throw index_out_of_bound();
			}
			/**
			* TODO iter--
			*/
			iterator operator--(int)
			{
				Node* t = this->nd;
				Node* cNode = this->nd;
				Node* begin = getBegin();
				if (cNode == begin)
					throw index_out_of_bound();
				if (cNode->left && cNode->left->type != NIL)
				{
					t = cNode->left;
					while (t->right->type != NIL)
						t = t->right;
					this->nd = t;
					return iterator(cNode);
				}
				if (cNode->parent)
				{
					t = cNode->parent;
					while (t->type != NIL && cNode == t->left)
					{
						cNode = t;
						t = t->parent;
					}
				}
				cNode = this->nd;
				this->nd = t;
				if (t->type != NIL)
					return iterator(cNode);
				else
					throw index_out_of_bound();
			}
			/**
			* TODO --iter
			*/
			iterator & operator--()
			{
				Node* t = this->nd;
				Node* cNode = this->nd;
				Node* begin = getBegin();
				if (cNode == begin)
					throw index_out_of_bound();
				if (cNode->left && cNode->left->type != NIL)
				{
					t = cNode->left;
					while (t->right->type != NIL)
						t = t->right;
					this->nd = t;
					return *this;
				}
				if (cNode->parent)
				{
					t = cNode->parent;
					while (t->type != NIL && cNode == t->left)
					{
						cNode = t;
						t = t->parent;
					}
				}
				this->nd = t;
				if (t->type != NIL)
					return *this;
				else
					throw index_out_of_bound();
			}
			/**
			* a operator to check whether two iterators are same (pointing to the same memory).
			*/
			value_type & operator*() const
			{
				return *nd->vt;
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
				return nd->vt;
			}
			iterator& operator=(const iterator &rhs) {
				nd = rhs.nd;
				return *this;
			}
		private:
			Node* getRoot()
			{
				Node *p = nd;
				if (p)
				{
					while (p->parent)
					{
						p = p->parent;
					}
				}
				return p;
			}
			Node* getBegin()
			{
				Node *r = getRoot();
				while (r->left)
				{
					r = r->left;
				}
				if (r)
					r = r->parent;
				return r;
			}
			Node* getEnd()
			{
				Node *r = getRoot();
				while (r->right)
				{
					r = r->right;
				}
				return r;
			}
		};
		class const_iterator {
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.
		public:
			const Node* nd;
		public:
			const_iterator() {
				nd == NULL;
			}
			const_iterator(const Node* nd) {
				this->nd = nd;
			}
			const_iterator(const const_iterator &other) {
				nd = other.nd;
			}
			const_iterator(const iterator &other) {
				nd = other.nd;
			}
			const_iterator& operator=(const const_iterator &other) {
				nd = other.nd;
				return *this;
			}
			bool operator!=(const const_iterator &other) {
				return nd != other.nd;
			}
			bool operator==(const const_iterator &other) {
				return nd == other.nd;
			}
			value_type* operator->() {
				return nd->vt;
			}
			value_type & operator*() const
			{
				return *nd->vt;
			}

			const_iterator operator++(int)
			{
				const Node* t = this->nd;
				const Node* cNode = this->nd;
				const Node* end = getEnd();
				if (cNode == end)
					throw index_out_of_bound();
				if (cNode->right == end)
				{
					this->nd = end;
					return const_iterator(end);
				}
				if (cNode->right->type != NIL)
				{
					t = cNode->right;
					while (t->left->type != NIL)
						t = t->left;
					this->nd = t;
					return const_iterator(t);
				}
				if (cNode->parent)
				{
					t = cNode->parent;
					while (t->parent && t->type != NIL && cNode == t->right)
					{
						cNode = t;
						t = t->parent;
					}
				}
				if (t)
				{
					this->nd = t;
					return const_iterator(t);
				}
				else
					throw index_out_of_bound();
			}
			/**
			* TODO ++iter
			*/
			const_iterator & operator++()
			{
				const Node* t = this->nd;
				const Node* cNode = this->nd;
				const Node* end = getEnd();
				if (cNode == end)
					throw index_out_of_bound();
				if (cNode->right == end)
				{
					this->nd = end;
					return *this;
				}
				if (cNode->right->type != NIL)
				{
					t = cNode->right;
					while (t->left->type != NIL)
						t = t->left;
					this->nd = t;
					return *this;
				}
				if (cNode->parent)
				{
					t = cNode->parent;
					while (t->parent && t->type != NIL && cNode == t->right)
					{
						cNode = t;
						t = t->parent;
					}
				}
				if (t)
				{
					this->nd = t;
					return *this;
				}
				else
					throw index_out_of_bound();
			}
			/**
			* TODO iter--
			*/
			const_iterator operator--(int)
			{
				const Node* t = this->nd;
				const Node* cNode = this->nd;
				const Node* begin = getBegin();
				if (cNode == begin)
					throw index_out_of_bound();
				if (cNode->left && cNode->left->type != NIL)
				{
					t = cNode->left;
					while (t->right->type != NIL)
						t = t->right;
					this->nd = t;
					return const_iterator(t);
				}
				if (cNode->parent)
				{
					t = cNode->parent;
					while (t->type != NIL && cNode == t->left)
					{
						cNode = t;
						t = t->parent;
					}
				}
				this->nd = t;
				if (t->type != NIL)
					return const_iterator(t);
				else
					throw index_out_of_bound();
			}
			/**
			* TODO --iter
			*/
			const_iterator & operator--()
			{
				const Node* t = this->nd;
				const Node* cNode = this->nd;
				const Node* begin = getBegin();
				if (cNode == begin)
					throw index_out_of_bound();
				if (cNode->left && cNode->left->type != NIL)
				{
					t = cNode->left;
					while (t->right->type != NIL)
						t = t->right;
					this->nd = t;
					return *this;
				}
				if (cNode->parent)
				{
					t = cNode->parent;
					while (t->type != NIL && cNode == t->left)
					{
						cNode = t;
						t = t->parent;
					}
				}
				this->nd = t;
				if (t->type != NIL)
					return *this;
				else
					throw index_out_of_bound();
			}
			const Node* getRoot()
			{
				const Node *p = nd;
				if (p)
				{
					while (p->parent)
					{
						p = p->parent;
					}
				}
				return p;
			}
			const Node* getEnd()
			{
				const Node *r = getRoot();
				while (r->right)
				{
					r = r->right;
				}
				return r;
			}
			const Node* getBegin()
			{
				const Node *r = getRoot();
				while (r->left)
				{
					r = r->left;
				}
				if (r)
					r = r->parent;
				return r;
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
		map(const map &other) {
			t = new RBTree();
			t->copyTree(other.t);
		}
		/**
		* TODO assignment operator
		*/
		map & operator=(const map &other)
		{
			if (!t)
				t = new RBTree();
			t->copyTree(other.t);
			return *this;
		}
		bool operator==(const map &other)
		{
			return t == other.t;
		}
		/**
		* TODO Destructors
		*/
		~map() {
			if(t)
				t->clear();
			delete t;
			t = NULL;
		}
		/**
		* TODO
		* access specified element with bounds checking
		* Returns a reference to the mapped value of the element with key equivalent to key.
		* If no such element exists, an exception of type `index_out_of_bound'
		*/
		T & at(const Key &key)
		{
			Node *p = t->find(key);
			if (p == NULL)
				throw index_out_of_bound();
			else
				return p->vt->second;
		}
		const T & at(const Key &key) const
		{
			Node *p = t->find(key);
			if (p == NULL)
				throw index_out_of_bound();
			else
			{
				return p->vt->second;
			}
		}
		/**
		* TODO
		* access specified element
		* Returns a reference to the value that is mapped to a key equivalent to key,
		*   performing an insertion if such key does not already exist.
		*/
		T & operator[](const Key &key)
		{
			Node *p = t->find(key);
			if (p == NULL)
			{
				T dt = T();
				t->insert(value_type(key, dt));
				p = t->find(key);
			}
			return p->vt->second;
		}
		/**
		* behave like at() throw index_out_of_bound if such key does not exist.
		*/
		const T & operator[](const Key &key) const
		{
			Node *p = t->find(key);
			if (p == NULL)
				throw index_out_of_bound();
			return p->vt->second;
		}
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
		size_t size() const
		{
			return t->size();
		}
		/**
		* clears the contents
		*/
		void clear()
		{
			if(t)
				t->clear();
			delete t;
			t = NULL;
			t = new RBTree();
		}
		/**
		* insert an element.
		* return a pair, the first of the pair is
		*   the iterator to the new element (or the element that prevented the insertion),
		*   the second one is true if insert successfully, or false.
		*/
		pair<iterator, bool> insert(const value_type &value) {
			Node *n = t->find(value.first);
			if(n == NULL)
			{
				n = t->insert(value);
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
			else
			{
				iterator iter(n);
				return pair<iterator, bool>(iter, false);
			}

		}
		/**
		* erase the element at pos.
		*
		* throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
		*/
		void erase(iterator pos)
		{
			if (!pos.nd || pos.nd == t->end())
				throw index_out_of_bound();
			Node *p = t->find(pos->first);
			iterator iter(p); // check if iterator point to this map
			if (p == NULL || iter != pos)
				throw invalid_iterator();
			else
			{
				value_type vt(pos->first, pos->second);
				t->remove(vt);
			}
		}
		/**
		* Returns the number of elements with key
		*   that compares equivalent to the specified argument,
		*   which is either 1 or 0
		*     since this container does not allow duplicates.
		* The default method of check the equivalence is !(a < b || b > a)
		*/
		size_t count(const Key &key) const {
			if (t->find(key))
				return 1;
			else
				return 0;
		}
		/**
		* Finds an element with key equivalent to key.
		* key value of the element to search for.
		* Iterator to an element with key equivalent to key.
		*   If no such element is found%, past-the-end (see end()) iterator is returned.
		*/
		iterator find(const Key &key)
		{
			Node *p = t->find(key);
			if (p)
			{
				return iterator(p);
			}
			else
			{
				return iterator(end());
			}
		}
		const_iterator find(const Key &key) const
		{
			Node *p = t->find(key);
			if (p)
			{
				return const_iterator(p);
			}
			else
			{
				return const_iterator(cend());
			}
		}
		void print() { t->print(); }
	private:
		void copyMap(const RBTree &other)
		{
			t = new RBTree(other);
		}
	private:
		RBTree* t;
	};

}

#endif
