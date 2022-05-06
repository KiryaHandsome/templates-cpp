#pragma once

#include <functional>
#include <list>
#include <type_traits>



template<class Traits>
class RBTree
{
public:
	using KeyType		  = typename Traits::key;
	using ValueType		  = typename Traits::value;
	using key_compare     = typename Traits::key_compare;
	using reference		  = ValueType&;
	using const_reference = const ValueType&;

	class Iterator;
	class Reverse_Iterator;
	class Const_Iterator;
	class L_Iterator;
	class reverse_l_iterator;
	class Const_L_Iterator;

	using iterator = std::conditional_t<std::is_same_v<KeyType, ValueType>, Const_Iterator, Iterator>;
	using l_iterator = std::conditional_t<std::is_same_v<KeyType, ValueType>, Const_L_Iterator, L_Iterator>;


protected:
	key_compare comp;
	enum colors { RED, BLACK };
	struct Node
	{
		Node() {}
		Node(const KeyType& key, const ValueType& value)
			: key(key), value(value)
		{

		}
		Node* parent = nullptr;
		Node* left = nullptr;
		Node* right = nullptr;

		KeyType key;
		ValueType value;

		bool color = BLACK;
	};

	void insertToList(std::list<Node*>& list, Node* node) {
		if (list.empty()) list.push_back(node);
		else {
			auto it = list.begin();
			while (it != list.end() && comp((*it)->key, node->key)) ++it;
			list.insert(it, node);
		}
	}

	void removeFromList(std::list<Node*>& list, Node* node) {
		for (auto it = list.begin(); it != list.end(); ++it) {
			if (*it == node) {
				list.erase(it);
				return;
			}
		}
	}

protected:

	Node* root = nullptr;
	std::list<Node*> list;
	size_t _size = 0;


public:

	RBTree()
	{
	}

	~RBTree()
	{
		deleteTree(root);
	}



	Node* search(const KeyType& key)
	{
		Node* curr = root;
		while (curr != nullptr) {
			if (comp(curr->key, key)) {
				curr = curr->right;
			}
			else if (comp(key, curr->key)) {
				curr = curr->left;
			}
			else {
				return curr;
			}
		}
		return nullptr;
	}

	bool erase(const KeyType& key) {
		Node* node = root;
		Node* z = nullptr;
		Node* x, *y;
		while (node != nullptr) {
			if (node->key == key) {
				z = node;
				break;
			}

			if (node->key < key) {
				node = node->right;
			}
			else {
				node = node->left;
			}
		}

		if (z == nullptr) {
			std::cout << "Key not found in the tree" << std::endl;
			return false;
		}

		y = z;
		int y_original_color = y->color;
		if (z->left == nullptr) {
			x = z->right;
			rbTransplant(z, z->right);
		}
		else if (z->right == nullptr) {
			x = z->left;
			rbTransplant(z, z->left);
		}
		else {
			y = min_node(z->right);
			y_original_color = y->color;
			x = y->right;
			if (y->parent == z) {
				x->parent = y;
			}
			else {
				rbTransplant(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}

			rbTransplant(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->color = z->color;
		}
		delete z;
		if (y_original_color == 0) {
			delete_fixup(x);
		}
		return true;
	}


	void printTree()
	{
		printTree(root, 10);
	}

	void clear()
	{
		deleteTree(root);
		root = nullptr;
		list.clear();
		_size = 0;
	}

	bool empty() const { return _size == 0; }

	size_t size() const { return _size; }

	size_t max_size() const { return std::numeric_limits<ptrdiff_t>::max();  }

	size_t count(const KeyType& key)
	{
		return (search(key) == nullptr ? 0 : 1);
	}

	iterator find(const KeyType& key)
	{
		auto it = search(key);
		return iterator(it, this);
	}

protected:
	std::pair<iterator, bool> insert(const KeyType& key, const ValueType& value = ValueType())
	{
		Node* new_node = new Node(key, value);
		Node* parent = nullptr;
		Node* curr = root;
		while (curr != nullptr) {
			parent = curr;
			if (comp(key, curr->key)) {
				curr = curr->left;
			}
			else if (comp(curr->key, key)) {
				curr = curr->right;
			}
			else {
				return std::make_pair(iterator(curr, this), false);
			}
		}
		new_node->parent = parent;
		if (parent == nullptr) {
			this->root = new_node;
		}
		else if (comp(key, parent->key)) {
			parent->left = new_node;
		}
		else parent->right = new_node;

		new_node->color = RED;

		insert_fixup(new_node);

		insertToList(list, new_node);

		++_size;

		return std::make_pair(iterator(new_node, this), true);
	}

	void rbTransplant(Node* u, Node* v) {
		if (u->parent == nullptr) {
			root = v;
		}
		else if (u == u->parent->left) {
			u->parent->left = v;
		}
		else {
			u->parent->right = v;
		}
		if(v) v->parent = u->parent;
	}

public:

	class Iterator
	{
	private:
		Node* node = nullptr;
		RBTree* tree = nullptr;
	public:
		Iterator() { }

		Iterator(const Iterator& iter) : node(iter.node), tree(iter.tree) { }

		Iterator(Node* node, RBTree* tree) : node(node), tree(tree) { }

		Iterator& operator=(const Iterator& other)
		{
			node = other.node;
			tree = other->tree;
			return *this;
		}

		bool operator==(const Iterator& other) { return other.node == node; }

		bool operator!=(const Iterator& other) { return !(*this == other); }

		Iterator& operator++()
		{
			if (node->right != nullptr) {
				node = tree->min_node(node->right);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (node != nullptr && key_compare().operator()(node->key, key)) {
					node = node->parent;
				}
			}

			return *this;
		}

		Iterator& operator--()
		{
			if (node == nullptr) {
				node = tree->max_node(tree->root);
			}
			else if (node->left != nullptr) {
				node = tree->max_node(node->left);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(key, node->key) && node != nullptr) {
					node = node->parent;
				}
			}

			return *this;
		}

		Iterator operator++(int)
		{
			auto it = *this;
			if (node->right != nullptr) {
				node = tree->min_node(node->right);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(node->key, key) && node != nullptr) {
					node = node->parent;
				}
			}

			return *it;
		}

		Iterator operator--(int)
		{
			auto it = *this;
			if (node == nullptr) {
				node = tree->max_node(tree->root);
			}
			else if (node->left != nullptr) {
				node = tree->max_node(node->left);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(key, node->key) && node != nullptr) {
					node = node->parent;
				}
			}

			return it;
		}

		reference operator*() { return node->value; }

		ValueType* operator->() { return &node->value; }
	};

	class Const_Iterator
	{
	private:
		Node* node = nullptr;
		RBTree* tree = nullptr;
	public:
		//Const_Iterator() { }

		Const_Iterator(const Const_Iterator& iter) : node(iter.node), tree(iter.tree) { }

		Const_Iterator(Node* node, RBTree* tree) : node(node), tree(tree) { }

		Const_Iterator& operator=(const Const_Iterator& other)
		{
			node = other.node;
			tree = other->tree;
			return *this;
		}

		bool operator==(const Const_Iterator& other) { return other.node == node; }

		bool operator!=(const Const_Iterator& other) { return !(*this == other); }

		Const_Iterator& operator++()
		{
			if (node->right != nullptr) {
				node = tree->min_node(node->right);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (node != nullptr && key_compare().operator()(node->key, key)) {
					node = node->parent;
				}
			}

			return *this;
		}

		Const_Iterator& operator--()
		{
			if (node == nullptr) {
				node = tree->max_node(tree->root);
			}
			else if (node->left != nullptr) {
				node = tree->max_node(node->left);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(key, node->key) && node != nullptr) {
					node = node->parent;
				}
			}

			return *this;
		}

		Const_Iterator operator++(int)
		{
			auto it = *this;
			if (node->right != nullptr) {
				node = tree->min_node(node->right);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(node->key, key) && node != nullptr) {
					node = node->parent;
				}
			}

			return *it;
		}

		Const_Iterator operator--(int)
		{
			auto it = *this;
			if (node == nullptr) {
				node = tree->max_node(tree->root);
			}
			else if (node->left != nullptr) {
				node = tree->max_node(node->left);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(key, node->key) && node != nullptr) {
					node = node->parent;
				}
			}

			return it;
		}

		const_reference operator*() { return node->value; }

		const ValueType* operator->() { return &node->value; }
	};

	class Reverse_Iterator
	{
	private:
		Node* node = nullptr;
		RBTree* tree = nullptr;
	public:
		Reverse_Iterator() { }

		Reverse_Iterator(const Reverse_Iterator& iter) : node(iter.node), tree(iter.tree) { }

		Reverse_Iterator(Node* node, RBTree* tree) : node(node), tree(tree) { }

		Reverse_Iterator& operator=(const Reverse_Iterator& other)
		{
			node = other.node;
			return *this;
		}

		bool operator==(const Reverse_Iterator& other) { return other.node == node; }

		bool operator!=(const Reverse_Iterator& other) { return !(*this == other); }

		Reverse_Iterator& operator--()
		{
			if (node->right != nullptr) {
				node = tree->min_node(node->right);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(node->key, key) && node != nullptr) {
					node = node->parent;
				}
			}

			return *this;
		}

		Reverse_Iterator& operator++()
		{
			if (node == nullptr) {
				node = tree->max_node(tree->root);
			}
			else if (node->left != nullptr) {
				node = tree->max_node(node->left);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(key, node->key) && node != nullptr) {
					node = node->parent;
				}
			}

			return *this;
		}

		Reverse_Iterator operator--(int)
		{
			auto it = *this;
			if (node->right != nullptr) {
				node = tree->min_node(node->right);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(node->key, key) && node != nullptr) {
					node = node->parent;
				}
			}

			return *it;
		}

		Reverse_Iterator operator++(int)
		{
			auto it = *this;
			if (node == nullptr) {
				node = tree->max_node(root);
			}
			else if (node->left != nullptr) {
				node = tree->max_node(node->left);
			}
			else {
				KeyType key = node->key;
				node = node->parent;
				while (key_compare().operator()(node->key, key) && node != nullptr) {
					node = node->parent;
				}
			}

			return it;
		}

		ValueType& operator*() { return this->node->value; }

		ValueType* operator->() { return &this->node->value; }
	};

	class L_Iterator
	{
	private:
		typename std::list<Node*>::iterator it;

	public:
		L_Iterator() { }

		L_Iterator(typename std::list<Node*>::iterator it) : it(it) { }

		L_Iterator(const L_Iterator& iter) : it(iter.it) { }

		L_Iterator& operator=(const L_Iterator& other)
		{
			it = other.it;
			return *this;
		}

		bool operator==(const L_Iterator& other) { return other.it == it; }

		bool operator!=(const L_Iterator& other) { return !(*this == other); }

		L_Iterator& operator++()
		{
			++it;
			return *this;
		}

		L_Iterator& operator--()
		{
			--it;
			return *this;
		}

		L_Iterator operator++(int)
		{
			auto iter = *this;
			++it;
			return iter;
		}

		L_Iterator operator--(int)
		{
			auto iter = *this;
			--it;
			return iter;
		}

		ValueType& operator*() { return (*it)->value; }

		ValueType* operator->() { return &(*it)->value; }
	};

	class Const_L_Iterator
	{
	private:
		typename std::list<Node*>::const_iterator it;

	public:
		Const_L_Iterator() { }

		Const_L_Iterator(typename std::list<Node*>::const_iterator it) : it(it) { }

		Const_L_Iterator(const Const_L_Iterator& iter) : it(iter.it) { }

		Const_L_Iterator& operator=(const Const_L_Iterator& other)
		{
			it = other.it;
			return *this;
		}

		bool operator==(const Const_L_Iterator& other) { return other.it == it; }

		bool operator!=(const Const_L_Iterator& other) { return !(*this == other); }

		Const_L_Iterator& operator++()
		{
			++it;
			return *this;
		}

		Const_L_Iterator& operator--()
		{
			--it;
			return *this;
		}

		Const_L_Iterator operator++(int)
		{
			auto iter = *this;
			++it;
			return iter;
		}

		Const_L_Iterator operator--(int)
		{
			auto iter = *this;
			--it;
			return iter;
		}

		const_reference& operator*() { return (*it)->value; }

		const ValueType* operator->() { return &(*it)->value; }
	};

	class reverse_l_iterator
	{
	private:
		typename std::list<Node*>::reverse_iterator it;

	public:
		reverse_l_iterator() { }

		reverse_l_iterator(typename std::list<Node*>::reverse_iterator it) : it(it) { }

		reverse_l_iterator(const reverse_l_iterator& iter) : it(iter.it) { }

		reverse_l_iterator& operator=(const reverse_l_iterator& other)
		{
			it = other.it;
			return *this;
		}

		bool operator==(const reverse_l_iterator& other) { return other.it == it; }

		bool operator!=(const reverse_l_iterator& other) { return !(*this == other); }

		reverse_l_iterator& operator++()
		{
			++it;
			return *this;
		}

		reverse_l_iterator& operator--()
		{
			--it;
			return *this;
		}

		reverse_l_iterator operator++(int)
		{
			auto iter = *this;
			++it;
			return iter;
		}

		reverse_l_iterator operator--(int)
		{
			auto iter = *this;
			--it;
			return iter;
		}

		ValueType& operator*() { return (*it)->value; }

		ValueType* operator->() { return &(*it)->value; }
	};


	iterator begin() { return iterator(min_node(root), this); }

	iterator end() { return iterator(nullptr, this); }

	Const_Iterator cbegin() { return Const_Iterator(min_node(root), this); }

	Const_Iterator cend() { return Const_Iterator(nullptr, this); }

	Reverse_Iterator rbegin() { return Reverse_Iterator(max_node(root), this); }

	Reverse_Iterator rend() { return Reverse_Iterator(nullptr, this); }

	Const_L_Iterator clbegin() { return Const_L_Iterator(list.cbegin()); }

	Const_L_Iterator clend() { return Const_L_Iterator(list.cend()); }

	l_iterator l_begin() { return l_iterator(list.begin()); }

	l_iterator l_end() { return l_iterator(list.end()); }

	reverse_l_iterator rl_begin() { return reverse_l_iterator(list.rbegin()); }

	reverse_l_iterator rl_end() { return reverse_l_iterator(list.rend()); }


private:
	void rotateRight(Node* node)
	{
		Node* x = node->left;
		node->left = x->right;

		if (x->right != nullptr) {
			x->right->parent = node;
		}
		if (x != nullptr) x->parent = node->parent;
		if (node->parent == nullptr) {		//it's a root
			this->root = x;
		}
		else if (node == node->parent->left) {	//it's left subtree
			node->parent->left = x;
		}
		else {									//it's right subtree
			node->parent->right = x;
		}

		x->right = node;
		if (node != nullptr) node->parent = x;
	}

	void rotateLeft(Node* node)
	{
		Node* y = node->right;
		node->right = y->left;

		if (y->left != nullptr) {
			y->left->parent = node;
		}
		if (y != nullptr) y->parent = node->parent;
		if (node->parent == nullptr) {			//it's a root
			this->root = y;
		}
		else if (node == node->parent->left) {	//it's left subtree
			node->parent->left = y;
		}
		else {									//it's right subtree
			node->parent->right = y;
		}
		y->left = node;
		if (node != nullptr) node->parent = y;
	}

	void insert_fixup(Node* node)
	{
		while (node != root && node->parent->color == RED) {
			if (node->parent == node->parent->parent->left) {
				Node* y = node->parent->parent->right;
				if (y && y->color == RED) {
					node->parent->color = BLACK;
					y->color = BLACK;
					node->parent->parent->color = RED;
					node = node->parent->parent;
				}
				else {
					if (node == node->parent->right) {
						node = node->parent;
						rotateLeft(node);
					}

					node->parent->color = BLACK;
					node->parent->parent->color = RED;
					rotateRight(node->parent->parent);
				}
			}
			else {
				Node* y = node->parent->parent->left;
				if (y && y->color == RED) {
					node->parent->color = BLACK;
					y->color = BLACK;
					node->parent->parent->color = RED;
					node = node->parent->parent;
				}
				else {
					if (node == node->parent->left) {
						node = node->parent;
						rotateRight(node);
					}

					node->parent->color = BLACK;
					node->parent->parent->color = RED;
					rotateLeft(node->parent->parent);
				}
			}
		}
		this->root->color = BLACK;
	}

protected:

	Node* min_node(Node* node) {
		Node* curr = node;
		while (curr && curr->left) {
			curr = curr->left;
		}
		return curr;
	}

	Node* max_node(Node* node) {
		Node* curr = node;
		while (curr && curr->right) {
			curr = curr->right;
		}
		return curr;
	}

	//***********function for output tree*******************//
	void printTree(Node* node, int space)
	{
		if (node && node != nullptr) {
			space += 10;
			printTree(node->right, space);
			std::cout << "\n";
			for (int i = 10; i < space; ++i) {
				std::cout << " ";
			}

			if (node->color == RED) std::cout << "\033[31m" << node->key << "-" << node->pair.second << "\033[0m";
			else std::cout << node->key << "-" << node->pair.second << "\n";

			printTree(node->left, space);
		}
	}
private:

	void delete_fixup(Node* x)
	{
		if (!x) return;
		while (x != root && x->color == BLACK) {
			if (x == x->parent->left) {
				Node* w = x->parent->right;
				if (w->color == RED) {
					w->color = BLACK;
					x->parent->color = RED;
					rotateLeft(x->parent);
					w = x->parent->right;
				}
				if (w->left->color == BLACK && w->right->color == BLACK) {
					w->color = RED;
					x = x->parent;
				}
				else {
					if (w->right->color == BLACK) {
						w->left->color = BLACK;
						w->color = RED;
						rotateRight(w);
						w = x->parent->right;
					}
					w->color = x->parent->color;
					x->parent->color = BLACK;
					w->right->color = BLACK;
					rotateLeft(x->parent);
					x = root;
				}
			}
			else {
				Node* w = x->parent->left;
				if (w->color == RED) {
					w->color = BLACK;
					x->parent->color = RED;
					rotateRight(x->parent);
					w = x->parent->left;
				}
				if (w->right->color == BLACK && w->left->color == BLACK) {
					w->color = RED;
					x = x->parent;
				}
				else {
					if (w->left->color == BLACK) {
						w->right->color = BLACK;
						w->color = RED;
						rotateLeft(w);
						w = x->parent->left;
					}
					w->color = x->parent->color;
					x->parent->color = BLACK;
					w->left->color = BLACK;
					rotateRight(x->parent);
					x = root;
				}
			}
		}
		x->color = BLACK;
	}

	void deleteTree(Node* node)
	{
		if (node && node != nullptr) {
			deleteTree(node->left);
			deleteTree(node->right);
			delete node;
		}
	}

};
