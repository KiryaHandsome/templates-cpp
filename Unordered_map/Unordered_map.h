#pragma once

#include <functional>
#include <vector>
#include <forward_list>
#include <list>
#include <stdexcept>

template<class KeyT, class ValueT, class Hash = std::hash<KeyT>, class KeyEqual = std::equal_to<KeyT>>
class Unordered_map
{
public:
    using key_type = KeyT;
    using value_type = std::pair<const KeyT, ValueT>;
    using mapped_type = ValueT;
    constexpr static const float MAX_LOAD_FACTOR = 0.8;
    constexpr static const int MIN_BUCKET_COUNT = 16;	// must be a positive power of 2
    using iterator = typename std::list<value_type>::iterator;


private:

    Hash hasher;
    std::list<value_type> list;
    std::vector<std::forward_list<iterator>> buckets;
    size_t _size;
    size_t _buckets_count;


public:
    Unordered_map() :list(), buckets(MIN_BUCKET_COUNT), _buckets_count(MIN_BUCKET_COUNT), _size(0)
    {

    }

    ~Unordered_map()
    {

    }

    std::pair<iterator, bool> insert(const value_type& value)
    {
        size_t bucket = hasher(value.first) % _buckets_count;
        //check for element with such key
        for (auto& cur : buckets[bucket]) {
            if ((*cur).first == value.first) {
                return std::make_pair(cur, false);
            }
        }

        list.push_back(value);

        auto iter = --list.end();
        buckets[bucket].push_front(iter);

        ++_size;
        if (need_rehash()) {
            rehash(_buckets_count * 2);
        }

        return std::make_pair(iter, true);
    }

    bool rehash(size_t count)
    {
        if (count <= _buckets_count) return false;

        auto temp_list = list;
        clear();
        _buckets_count = count;
        buckets.resize(_buckets_count);
        for (auto& it : temp_list) {
            insert(it);
        }
        return true;
    }

    void clear()
    {
        buckets.clear();
        list.clear();
        buckets.resize(MIN_BUCKET_COUNT);
        _buckets_count = MIN_BUCKET_COUNT;
        _size = 0;
    }

    bool contains(const key_type& key)
    {
        return count(key);
    }

    size_t erase(const key_type& key)
    {
        size_t bucket = hasher(key) % _buckets_count;

        auto iter = list.end();
        auto prev = buckets[bucket].before_begin();//element before current

        for (auto& curr : buckets[bucket]) {
            if ((*curr).first == key) {
                iter = curr;
                buckets[bucket].erase_after(prev);
                break;
            }
            ++prev;
        }
        if (iter != list.end()) {
            --_size;
            list.erase(iter);
            return 1;
        }
        return 0;
    }

    iterator erase(iterator pos)
    {
        size_t key = (*pos).first;
        size_t bucket = hasher(key) % _buckets_count;

        auto iter = list.end();
        auto prev = buckets[bucket].begin();//element before current

        for (auto curr = buckets[bucket].begin(); curr != buckets[bucket].end(); ++curr) {
            if ((*(*curr)).first == key) {
                iter = *curr;
                buckets[bucket].erase_after(prev);
                break;
            }
            prev = curr;
        }

        if (iter != list.end()) {
            return list.erase(iter);
        }
        return iter;
    }

    mapped_type& at(const key_type& key)
    {
        size_t bucket = hasher(key) % _buckets_count;
        for (auto& it : buckets[bucket]) {
            if ((*it).first == key) {
                return (*it).second;
            }
        }
        throw std::out_of_range("No such key in unordered_map");
    }

    mapped_type& operator[](const key_type& key)
    {
        auto iter = find(key);
        if (iter != list.end()) return (*iter).second;
        return (*insert({ key, mapped_type() }).first).second;
    }

    size_t count(const key_type& key)
    {
        if (find(key) == list.end()) return 0;
        return 1;
    }

    iterator find(const key_type& key)
    {
        size_t bucket = hasher(key) % _buckets_count;
        for (auto& it : buckets[bucket]) {
            if ((*it).first == key) {
                return it;
            }
        }
        return list.end();
    }

    iterator begin() { return list.begin(); }

    iterator end() { return list.end(); }

    size_t size() const { return _size; }

    bool empty() const { return list.empty(); }

    float load_factor() { return static_cast<float>(size()) / static_cast<float>(_buckets_count); }

    float max_load_factor() { return MAX_LOAD_FACTOR; }

    size_t bucket_count() const { return _buckets_count; }

private:
    bool need_rehash() {
        float k = static_cast<float>(size()) / static_cast<float>(_buckets_count);
        if (MAX_LOAD_FACTOR < k) {
            return true;
        }
        return false;
    }


};


