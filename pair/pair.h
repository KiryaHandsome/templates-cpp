#pragma once

template<class T1, class T2>
struct pair
{
	T1 first;
	T2 second;


	pair() = default;
	pair(const T1& f, const T2& s) :first(f), second(s) { }
	pair(const pair& p) = default;
	pair(pair&& p) = default;

	
	pair& operator=(const pair& other) {
		first = other.first;
		second = other.second;
	}

	bool operator==(const pair& other) {
		return (first == other.first && second == other.second);
	}

	void swap(pair& other) {
		std::swap(first, other.first);
		std::swap(second, other.second);
	}	
};
