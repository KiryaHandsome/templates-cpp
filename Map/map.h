#pragma once


#include "RedBlackTree.h" 

template<class KeyType, class ValueType, class Compare>
class MapTraits 
{
public:
	using key = KeyType;
	using value = std::pair<const KeyType, ValueType>;
	using key_compare = Compare;
};


template<class KeyT, class ValueT, class Compare = std::less<KeyT>>
class Map : public RBTree<MapTraits<KeyT, ValueT, Compare>>
{
	using Tree = RBTree<MapTraits<KeyT, ValueT, Compare>>;
public:
	using Node				 = typename Tree::Node;
	using MappedType	     = ValueT;
	using iterator			 = typename Tree::Iterator;
	using reverse_iterator   = typename Tree::Reverse_Iterator;
	using l_iterator		 = typename Tree::l_iterator;
	using reverse_l_iterator = typename Tree::reverse_l_iterator;
	using ValueType			 = typename MapTraits<KeyT, ValueT, Compare>::value;
	using KeyType		     = typename MapTraits<KeyT, ValueT, Compare>::key;	


	Map(): Tree() {

	}

	Map(const Map& other) : Tree() {
		for (auto& it : other.list) {
			insert(it->value);
		}
	}

	Map(std::initializer_list<ValueType> init) : Tree() {
		for (auto it : init) {
			insert(std::make_pair(it.first, it.second));
		}
	}

	~Map() {

	}

	std::pair<iterator, bool> insert(const ValueType& value)
	{
		return Tree::insert(value.first, value);
	}

	ValueType& operator[](const KeyType& key)
	{
		return insert(key, ValueType()).first->value;
	}


	MappedType& at(const KeyType& key)
	{
		Node* node = Tree::search(key);
		if (node != nullptr) {
			return node->value.second;
		}
		else throw std::out_of_range("invalid map<K, T> key");
	}

};
