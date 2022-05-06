#pragma once

#include "RedBlackTree.h"

template<class Key, class Compare>
class SetTraits
{
public:
	using key = Key;
	using value = Key;
	using key_compare = Compare;
};

template<class Key, class Compare = std::less<Key>>
class Set : public RBTree<SetTraits<Key, Compare>>
{
	using Tree               = RBTree<SetTraits<Key, Compare>>;
	using Node               = typename Tree::Node;
	using iterator           = typename Tree::Const_Iterator;
	using reverse_iterator   = typename Tree::Reverse_Iterator;
	using l_iterator         = typename Tree::l_iterator;
	using reverse_l_iterator = typename Tree::reverse_l_iterator;
	using ValueType          = typename SetTraits<Key,Compare>::value;
	using KeyType            = typename SetTraits<Key,Compare>::key;

public:
	Set() { }

	Set(const Set& other) 
	{
		for (const auto& it : other.list) {
			insert(it->value);
		}
	}

	Set(std::initializer_list<ValueType> init)
	{
		for (const auto& it : init) {
			insert(it);
		}
	}

	~Set() { }

	bool contains(const Key& key)
	{
		if (search(key) != nullptr) {
			return true;
		}
		return false;
	}

	std::pair<iterator, bool> insert(const ValueType& value)
	{
		return Tree::insert(value, value);
	}

	

};
