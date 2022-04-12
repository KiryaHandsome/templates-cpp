#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <initializer_list>
#include <stdexcept>

template<class T>
class vector
{ 
public:
    template<class U> class Iterator;
    template<class U> class Reverse_iterator;
    typedef Iterator<T> iterator;
    typedef Iterator<const T> const_iterator;
    typedef Reverse_iterator<T> reverse_iterator;
    typedef Reverse_iterator<const T> const_reverse_iterator;
private:
    T* arr = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;

public:
    vector() {}

    vector(const vector& v) {
        reserve(v.size());
        for (size_t i = 0; i < v.size(); ++i) {
            new (arr + i) T(v[i]);
        }
    }

    vector(vector&& v) = default;

    vector(std::initializer_list<T> init) {
        reserve(init.size());
        for (int i = 0; i < init.size(); ++i) {
            new (arr + i) T(init[i]);
        }
        m_size = init.size();
    }

    explicit vector(size_t count, const T& value = T()) {
        reserve(count);
        for (int i = 0; i < count; ++i) {
            new (arr + i) T(value);
        }
        m_size = count;
    }

    ~vector() {
        for (size_t i = 0; i < m_size; ++i) {
            arr->~T();
        }
        delete[] reinterpret_cast<uint8_t*>(arr);
    }


    size_t max_size() const noexcept {
        return static_cast<size_t>(pow(2, 64) / sizeof(T));
    }

    void reserve(size_t new_cap) {
        if (new_cap > max_size()) {
            throw std::length_error("Too big size");
        }
        if (new_cap > m_capacity) {
            T* new_arr = reinterpret_cast<T*>(new uint8_t[sizeof(T) * new_cap]);
            //copy object to new arr
            for (size_t i = 0; i < m_size; ++i) {
                new (new_arr + i) T(arr[i]);	  //make exception safety!!!
            }
            //destruct objects
            for (size_t i = 0; i < m_size; ++i) {
                arr[i].~T();
            }
            //free memory
            delete[] reinterpret_cast<uint8_t*>(arr);

            arr = new_arr;
            m_capacity = new_cap;
        }
    }

    void resize(size_t count, const T& value = T()) {
        if (count > m_capacity) {
            reserve(count);
        }
        for (size_t i = m_size; i < count; ++i) {
            new(arr + i) T(value);
        }
        if (count < m_size) {
            m_size = count;
        }
    }

    void push_back(const T& value) {
        if (m_size == m_capacity) {
            size_t new_cap = (m_size ? m_capacity * 2 : 1);
            reserve(new_cap);
        }

        new (arr + m_size) T(value);
        ++m_size;
    }

    void pop_back() {
        --m_size;
    }

    void assign(size_t count, const T& value) {
        clear();
        reserve(count);
        for (size_t i = 0; i < count; ++i) {
            new (arr + i) T(value);
        }      
    }

    size_t size() const { return m_size; }

    size_t capacity() const { return m_capacity; }

    void clear() {
        //destruct objects
        for (size_t i = 0; i < m_size; ++i) {
            arr[i].~T();
        }
        //free memory
        delete[] reinterpret_cast<uint8_t*>(arr);
        arr = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    T& operator[](size_t indx) { return arr[indx]; }

    void operator=(const vector& v) {
        this->clear();
        reserve(v.size());
        for (size_t i = 0; i < v.size(); ++i) {
            new (arr + i) T(v[i]);
        }
    }

    T& at(size_t indx) {
        if (indx >= m_size) {
            throw std::out_of_range;
        }
        return arr[indx];
    }

    T& back() { return arr[m_size - 1]; }

    T* data() { return &arr[0]; }

    template< class... Args >
    void emplace_back(Args&&... args) {
        if (m_size == m_capacity) {
            reserve(m_size * 2);
        }
        new (arr + m_size) T(std::forward<Args>(args)...);
    }

    template< class... Args >
    void emplace(iterator pos, Args&&... args) {
        if (m_size == m_capacity) {
            reserve(m_size * 2);
        }
        T temp = *pos;
        new (&(*pos)) T(std::forward<Args>(args)...);
        T buff;
        auto it = pos + 1;
        for (; it != end(); ++it) {
            buff = *it;
            *it = temp;
            temp = buff;
        }
        *it = temp;
        ++m_size;
    }

    T& front() { return arr[0]; }

    bool empty() const { return m_size == 0; }

    void swap(vector<T>& other) {
        std::swap(arr, other.arr);
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
    }

    void insert(iterator pos, const T& value) {
        emplace(pos, value);
    }

    //void insert(iterator pos, size_t count, const T& value) {
    //    size_t new_size = m_size + count;
    //    if (m_capacity < new_size) {
    //        reserve(static_cast<size_t>(new_size * 1.5));
    //    }
    //    //count number of elements after pos
    //    size_t counter = 0;
    //    for (auto it = pos; it != end(); ++it) {
    //        ++counter;
    //    }
    //    //write elements to buffer
    //    T* buffer = reinterpret_cast<T*>(new uint8_t[counter * sizeof(T)]);
    //    size_t i = 0;
    //    for (auto it = pos; it != end(); ++it) {
    //        new (buffer + i) T(*it);
    //        ++i;
    //    }
    //    //insert


    //}

    void erase(iterator pos) {
        //destruct object
        (*pos).~T();
        //copy old values
        auto temp = pos;
        for (auto it = pos + 1; it != end(); ++it) {
            *temp = *it;
            ++temp;
        }
        --m_size;
    }


    void erase(iterator first, iterator last) {
        //destruct the objects
        size_t counter = 0;
        for (auto it = first; it != last; ++it) {
            (*it).~T();
            ++counter;
        }
        //copy old values
        iterator temp = first;
        for (auto it = last; it != end(); ++it) {
            *temp = *it;
            ++temp;
        }
        m_size -= counter;
    }

    template<class U>
    class Iterator {
    private:
        int index = 0;
        vector<U>* p_arr = nullptr;
    public:
        Iterator() {}
        Iterator(vector* arr, int indx) : index(indx), p_arr(arr) { }

        Iterator& operator++() { ++index; return *this; }
        Iterator& operator--() { --index; return *this; }
        Iterator& operator++(int) { Iterator it(*this);  ++index; return *this; }
        Iterator& operator--(int) { Iterator it(*this);  --index; return *this; }

        T& operator*() { return (*p_arr)[index]; }
        const T& operator*() const { return (*p_arr)[index]; }

        T* operator->() { return &(*p_arr)[index]; }
        const T* operator->() const { return &(*p_arr)[index]; }

        bool operator==(const Iterator& other) { return index == other.index; }
        bool operator!=(const Iterator& other) { return index != other.index; }

        Iterator& operator+=(int num) { index += num; }
        Iterator& operator-=(int num) { index -= num; }
        Iterator operator+(int num) {
            Iterator it(*this);
            it.index += num;
            return it;
        }
        Iterator operator-(int num) {
            Iterator it(*this);
            it.index -= num;
            return it;
        }
    };

    template<class U>
    class Reverse_iterator {
    private:
        int index = 0;
        vector<U>* p_arr = nullptr;
    public:
        Reverse_iterator() {}
        Reverse_iterator(vector* arr, int indx) : index(indx), p_arr(arr) { }

        Reverse_iterator& operator++() { --index; return *this; }
        Reverse_iterator& operator--() { ++index; return *this; }
        Reverse_iterator& operator++(int) { Reverse_iterator it(*this);  --index; return *this; }
        Reverse_iterator& operator--(int) { Reverse_iterator it(*this);  ++index; return *this; }

        T& operator*() { return (*p_arr)[index]; }
        const T& operator*() const { return (*p_arr)[index]; }

        T* operator->() { return &(*p_arr)[index]; }
        const T* operator->() const { return &(*p_arr)[index]; }

        bool operator==(const Reverse_iterator& other) { return index == other.index; }
        bool operator!=(const Reverse_iterator& other) { return index != other.index; }

        Reverse_iterator& operator+=(int num) { index -= num; }
        Reverse_iterator& operator-=(int num) { index += num; }

        Reverse_iterator operator+(int num) {
            Reverse_iterator it(*this);
            it.index += num;
            return it;
        }
        Reverse_iterator operator-(int num) {
            Reverse_iterator it(*this);
            it.index -= num;
            return it;
        }
    };

    
    

    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, m_size); }

    const_iterator cbegin() const { return iterator(this, 0); }
    const_iterator cend() const { return iterator(this, m_size); }

    reverse_iterator rbegin() { return reverse_iterator(this, m_size - 1); }
    reverse_iterator rend() { return reverse_iterator(this, -1); }

    const_reverse_iterator rcbegin() const { return const_reverse_iterator(this, m_size - 1); }
    const_reverse_iterator rcend() const { return const_reverse_iterator(this, -1); }


    

    
    
};

#endif // MY_VECTOR_H
