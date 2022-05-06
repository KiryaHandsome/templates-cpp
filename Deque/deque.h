#ifndef DEQUE_H
#define DEQUE_H

#include <algorithm>

template<class T>
class deque
{
public:
    class iterator;
private:

    static constexpr size_t initial_map_size = 8;

    T** map; // pointer to array of pointers
    size_t map_size;

    static constexpr size_t bytes = sizeof(T);
    static constexpr size_t block_size = bytes <= 1 ? 16	//size of one block(chunk)
                                       : bytes <= 2 ? 8
                                       : bytes <= 4 ? 4
                                       : bytes <= 8 ? 2
                                       : 1;
    size_t _size; //current length of sequence

    iterator start;
    iterator finish;
public:
    class iterator
    {
        friend class deque;
    private:
        T** node; //pointer to current chunk
        T* curr;
        T* first;	//first and last elements in current chunk
        T* last;

    public:
        iterator() = default;

        iterator(const iterator& x) : curr(x.curr), first(x.first), last(x.last), node(x.node) {}

        iterator(T* x, T** map_pointer)
            : curr(x), first(*map_pointer), last(*map_pointer + block_size), node(map_pointer) {}

        iterator& operator++()
        {
            ++curr;
            if (curr == last) {
                set_node(node + 1);
                curr = first;
            }
            return *this;
        }

        iterator& operator--()
        {
            if (curr == first) {
                set_node(node - 1);
                curr = last;
            }
            else {
                --curr;
            }
            return *this;
        }

        iterator& operator+=(ptrdiff_t n)
        {
            ptrdiff_t offset = n + curr - first;

            if (offset >= 0 && offset <= block_size) {		//if it in the same chunk
                curr += offset;
            }
            else {
                ptrdiff_t node_offset;
                if (offset > 0) {
                    node_offset = offset / block_size;
                }
                else {
                    node_offset = (offset + 1) / block_size - 1;
                }

                set_node(node + node_offset);

                curr = first + (offset - node_offset * block_size);
            }
            return *this;
        }

        iterator operator+(ptrdiff_t n) const
        {
            auto temp = *this;
            return temp += n;
        }

        iterator operator-(ptrdiff_t n) const
        {
            auto temp = *this;
            return temp += -n;
        }

        T& operator[](size_t index)
        {
            return *(*this + index);
        }

        T& operator*() { return *curr; }

        bool operator!=(const iterator& other) { return other.curr != curr; }

    private:
        void set_node(T** new_node)
        {
            node = new_node;
            first = *new_node;
            last = first + block_size;
        }
    };

public:
    deque() : map(nullptr), _size(0), map_size(0)
    {
        create_map_and_nodes(0);
    }

    ~deque()
    {
        for (T** node = start.node; node <= finish.node; ++node) {
            delete[] *node;
        }
        delete[] map;
    }

    void push_back(const T& value) {
        //  check if there is the enough memory to insert element
        if (finish.curr != finish.last - 1) {
            //  directly constructor.
            *finish.curr = value;
            //  adjust the finish map iterator.
            ++finish.curr;
        }
        else {
            //  the special push back version.
            push_back_aux(value);
        }
        ++_size;
    }

    void pop_back() {
        if (!empty()) {
            if (finish.curr != finish.first) {
                --finish.curr;
            }
            else {
                finish.set_node(finish.node - 1);
                finish.curr = finish.last - 1;
            }
            --_size;
        }
    }

    void push_front(const T& value)
    {
        if (start.curr != start.first) {
            --start.curr;
            *start.curr = value;
        }
        else {
            T t_copy = value;
            reserve_map_at_front();
            *(start.node - 1) = new T[block_size];
            start.set_node(start.node - 1);
            start.curr = start.last - 1;
            *start.curr = t_copy;
        }
        ++_size;
    }

    void pop_front()
    {
        if (!empty()) {
            if (start.curr != start.last - 1) {
                ++start.curr;
            }
            else {
                start.set_node(start.node + 1);
                start.curr = start.first;
            }
            --_size;
        }
    }

    void clear() {
        for (T** node = start.node; node <= finish.node; ++node) {
            delete[] *node;
        }
        delete[] map;
        _size = 0;
        create_map_and_nodes(0);
    }

    size_t size() const { return _size; }

    bool empty() const { return _size == 0; }

    T& operator[](size_t index) { return start[index]; }

    iterator begin() { return start; }

    iterator end() { return finish; }

    void resize(size_t count, const T& value = T())
    {
        size_t curr_size = _size;
        if (count > _size) {
            for (int i = 0; i < count - curr_size; ++i) {
                push_back(value);
            }
        }
        else {
            for (int i = 0; i < curr_size - count; ++i) {
                pop_back();
            }
        }
    }



private:	//private functions
    void reallocate_map(size_t nodes_to_add, bool add_at_front) {
        size_t old_num_nodes = finish.node - start.node + 1;
        size_t new_num_nodes = old_num_nodes + nodes_to_add;
        T** new_nstart;
        if (map_size > 2 * new_num_nodes) {
            //  to balance map size.
            //  just to avoid map size being off balance.
            new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start.node) {
                std::copy(start.node, finish.node + 1, new_nstart);
            }
            else {
                std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
            }
        }
        else {
            size_t new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
            //  allocate new memory for new map.
            T** new_map = new T*[new_map_size];
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            //  copy original map.
            std::copy(start.node, finish.node + 1, new_nstart);
            //  reallocate original map.
            delete[] map;
            //  set new map and corresponding size.
            map = new_map;
            map_size = new_map_size;
        }
        //  reset start and finish node.
        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
    }

    void reserve_map_at_back(size_t nodes_to_add = 1) {
        if (nodes_to_add > map_size - (finish.node - map) - 1) {
            //  if the size of back deque
            //  reallocate map at back.
            reallocate_map(nodes_to_add, false);
        }
    }

    void reserve_map_at_front(size_t nodes_to_add = 1) {
        if (nodes_to_add > start.node - map) {
            reallocate_map(nodes_to_add, true);
        }
    }

    void push_back_aux(const T& t) {
        T t_copy = t;
        reserve_map_at_back();
        //  allocate new node buffer.
        *(finish.node + 1) = new T[block_size];
        //  constructe.
        *finish.curr = t_copy;
        //  change finish iterator to point to new node.
        finish.set_node(finish.node + 1);
        //  set finish state.
        finish.curr = finish.first;
    }


    void create_map_and_nodes(size_t num_elements) {
        size_t num_nodes = num_elements / block_size + 1;
        map_size = std::max(initial_map_size, num_nodes + 2);
        //  begin and last will be allocate more memory
        //  to save time for inserting elements in deque.
        map = new T*[map_size];
        //  set start and finish node in the middle of map.
        T** nstart = map + (map_size - num_nodes) / 2;
        T** nfinish = nstart + num_nodes - 1;
        for (T** cur = nstart; cur <= nfinish; cur++) {
            *cur = new T[block_size];
        }
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.curr = start.first;
        finish.curr = finish.first + num_elements % block_size;
    }
};

#endif // DEQUE_H
