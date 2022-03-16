#ifndef SMART_PTRS_H
#define SMART_PTRS_H

#include <utility>
#include <compare>
#include <iostream>

template<class T>
struct ControlBlock
{
	T* m_ptr;
	int sharedCount = 0;
	int weakCount = 0;

	void incSharedCount() { sharedCount++; }
	void decSharedCount() { sharedCount--; }
	void incWeakCount() { weakCount++; }
	void decWeakCount() { weakCount--; }
};

template<class T>
class weak_ptr;

template<class T>
class shared_ptr
{
private:
	template<class Y> friend class shared_ptr;
	template<class Y> friend class weak_ptr;

	ControlBlock<T>* c_ptr = nullptr;
public:
	using element_type = std::remove_extent_t<T>;

	shared_ptr() //default constructor
	{
	}

	explicit shared_ptr(T* ptr) : c_ptr(new ControlBlock<T>)
	{
		c_ptr->m_ptr = ptr;
		c_ptr->incSharedCount();
	}

	~shared_ptr()
	{
		if (!c_ptr || !c_ptr->m_ptr) return;
		c_ptr->decSharedCount();
		if (c_ptr->sharedCount <= 0) {
			delete c_ptr->m_ptr;
			c_ptr->m_ptr = nullptr;
			if (c_ptr->weakCount <= 0) {
				delete c_ptr;
				c_ptr = nullptr;
			}
		}
	}

	//***copy semantic***//

	shared_ptr(const shared_ptr& obj)
	{
		c_ptr = obj.c_ptr;
		if (c_ptr) {
			c_ptr->incSharedCount();
		}
	}

	shared_ptr<T>& operator=(shared_ptr<T>& obj)
	{
		if (c_ptr) {
			c_ptr->decSharedCount();
		}
		c_ptr = obj.c_ptr;
		if (c_ptr) {
			c_ptr->incSharedCount();
		}

		return *this;
	}

	//***move semantic***//

	shared_ptr(shared_ptr&& obj) noexcept
	{
		c_ptr = obj.c_ptr;
		if (c_ptr) c_ptr->m_ptr = obj.c_ptr->m_ptr;
		obj.c_ptr->m_ptr = nullptr;
	}

	shared_ptr<T>& operator=(shared_ptr<T>&& obj) noexcept
	{
		if (this->c_ptr) {
			c_ptr->decSharedCount();
		}
		c_ptr = obj.c_ptr;
		if (c_ptr) c_ptr->m_ptr = obj.c_ptr->m_ptr;
		obj.c_ptr = nullptr;

		return *this;
	}

	T* operator->() const
	{
		return c_ptr->m_ptr;
	}

	T& operator*() const
	{
		return *(c_ptr->m_ptr);
	}

	int use_count() const
	{
		if (c_ptr) return c_ptr->sharedCount;
		return 0;
	}

	bool isNull() const
	{
		if (c_ptr) {
			return (c_ptr->m_ptr == nullptr);
		}
		return true;
	}

	void swap(const shared_ptr<T>& obj)
	{
		std::swap(c_ptr, obj.c_ptr);
	}

	explicit shared_ptr(const weak_ptr<T>& ptr)
		: c_ptr(ptr.c_ptr)
	{
		if (c_ptr) c_ptr->incSharedCount();
	}

	void reset(T* ptr)
	{
		delete c_ptr->m_ptr;
		c_ptr->m_ptr = ptr;
	}

	void swap(shared_ptr& r) noexcept
	{
		std::swap(c_ptr, r.c_ptr);
	}

	T* get() const noexcept { return (c_ptr ? c_ptr->m_ptr : nullptr); }

	bool unique() const noexcept { return this->use_count() == 1; }

    explicit operator bool() const noexcept { return (c_ptr ? (c_ptr->m_ptr == nullptr) : 0); }

    bool owner_before(const weak_ptr<T>& other) const noexcept { return (c_ptr < other.c_ptr); }

	bool owner_before(const shared_ptr<T>& other) const noexcept { return c_ptr < other.c_ptr; }


};

template<class T>
class shared_ptr<T[]>
{
private:
	template<class Y> friend class shared_ptr;
	template<class Y> friend class weak_ptr;

	T* object = nullptr;
	ControlBlock<T>* c_ptr = nullptr;
public:
	using element_type = std::remove_extent_t<T>;

	shared_ptr() //default constructor
	{
	}

	explicit shared_ptr(T* ptr) : c_ptr(new ControlBlock<T>)
	{
		c_ptr->m_ptr = ptr;
		c_ptr->incSharedCount();
	}

	~shared_ptr()
	{
		if (!c_ptr || !c_ptr->m_ptr) return;
		c_ptr->decSharedCount();
		if (c_ptr->sharedCount <= 0) {
			delete[] c_ptr->m_ptr;
			c_ptr->m_ptr = nullptr;
			if (c_ptr->weakCount <= 0) {
				delete c_ptr;
				c_ptr = nullptr;
			}
		}
	}

	//***copy semantic***//

	shared_ptr(const shared_ptr& obj)
	{
		c_ptr = obj.c_ptr;
		if (c_ptr) {
			c_ptr->incSharedCount();
		}
	}

	shared_ptr<T>& operator=(shared_ptr<T>& obj)
	{
		if (c_ptr) {
			c_ptr->decSharedCount();
		}
		c_ptr = obj.c_ptr;
		if (c_ptr) {
			c_ptr->incSharedCount();
		}

		return *this;
	}

	//***move semantic***//

	shared_ptr(shared_ptr&& obj) noexcept
	{
		c_ptr = obj.c_ptr;
		if (c_ptr) c_ptr->m_ptr = obj.c_ptr->m_ptr;
		obj.c_ptr->m_ptr = nullptr;
	}

	shared_ptr<T>& operator=(shared_ptr<T>&& obj) noexcept
	{
		if (this->c_ptr) {
			c_ptr->decSharedCount();
		}
		c_ptr = obj.c_ptr;
		if (c_ptr) c_ptr->m_ptr = obj.c_ptr->m_ptr;
		obj.c_ptr = nullptr;

		return *this;
	}

	T* operator->() const
	{
		return c_ptr->m_ptr;
	}

	T& operator*() const
	{
		return *(c_ptr->m_ptr);
	}

	int use_count() const
	{
		if (c_ptr) return c_ptr->sharedCount;
		return 0;
	}

	bool isNull() const
	{
		if (c_ptr) {
			return (c_ptr->m_ptr == nullptr);
		}
		return true;
	}

	void swap(const shared_ptr<T>& obj)
	{
		std::swap(c_ptr, obj.c_ptr);
	}

	explicit shared_ptr(const weak_ptr<T>& ptr)
		: c_ptr(ptr.c_ptr)
	{
		if (c_ptr) c_ptr->incSharedCount();
	}

	void reset(T* ptr)
	{
		delete c_ptr->m_ptr;
		c_ptr->m_ptr = ptr;
	}

	void swap(shared_ptr& r) noexcept
	{
		std::swap(c_ptr, r.c_ptr);
	}

	T* get() const noexcept { return (c_ptr ? c_ptr->m_ptr : nullptr); }

	bool unique() const noexcept { return this->use_count() == 1; }

    explicit operator bool() const noexcept { return (c_ptr ? (c_ptr->m_ptr == nullptr) : 0); }

	bool owner_before(const weak_ptr<T>& other) const noexcept { return c_ptr < other.c_ptr; }

	bool owner_before(const shared_ptr<T>& other) const noexcept { return c_ptr < other.c_ptr; }

	T& operator[](int indx) noexcept { return c_ptr->m_ptr[indx]; }
};

template< class T, class... Args>
std::enable_if_t<!std::is_array<T>::value, shared_ptr<T>>
make_shared(Args&&... args)
{
	return shared_ptr<T>(new T(std::forward<Args>(args)...));	//for no arrays
}

template<class U>
std::enable_if_t<std::is_array<U>::value, shared_ptr<U>>
make_shared(int n)
{
	return shared_ptr<U>(new std::remove_extent_t<U>[n]());	//for arrays
}


template< class T, class U, class V >
std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os, const shared_ptr<T>& ptr)
{
	os << ptr.get();
	return os;
}

template< class T, class U >
bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept { return lhs.get() == rhs.get(); }

template< class T, class U >
bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept { return lhs.get() != rhs.get(); }

template<class T>
class weak_ptr
{
private:
	template<class Y> friend class shared_ptr;
	template<class Y> friend class weak_ptr;

	ControlBlock<T>* c_ptr = nullptr;
public:
	weak_ptr() : c_ptr(nullptr)
	{
	}

	~weak_ptr()
	{
		if (!c_ptr) return;
		c_ptr->decWeakCount();
		if (c_ptr->sharedCount <= 0 && c_ptr->weakCount <= 0) {
			delete c_ptr;
			c_ptr = nullptr;
		}
	}

	weak_ptr(const shared_ptr<T>& obj)
	{
		c_ptr = obj.c_ptr;
		if (c_ptr) c_ptr->incWeakCount();
	}

	bool expired()
	{
		return (!c_ptr || c_ptr->sharedCount <= 0);
	}



	weak_ptr<T>& operator=(const weak_ptr<T>& a)
	{
		if (c_ptr) {
			c_ptr->decWeakCount();
		}
		c_ptr = a.c_ptr;
		if (c_ptr) c_ptr->incWeakCount();

		return *this;
	}

	weak_ptr<T>& operator=(const shared_ptr<T>& ptr) noexcept
	{
		if (c_ptr) c_ptr->decWeakCount();
		c_ptr = ptr.c_ptr;
		if (c_ptr) c_ptr->incWeakCount();

		return *this;
	}

	weak_ptr<T>& operator=(weak_ptr<T>&& a) noexcept
	{
		if (this->c_ptr) {
			c_ptr->decWeakCount();
		}
		c_ptr = a.c_ptr;

		a.c_ptr = nullptr;


		return *this;
	}

	void reset() noexcept
	{
		if (c_ptr) {
			c_ptr->decWeakCount();
		}
		c_ptr = nullptr;
	}

	void swap(weak_ptr& r) noexcept
	{
		std::swap(c_ptr, r.c_ptr);
	}

	int use_count() const
	{
		if (c_ptr) {
			return c_ptr->sharedCount;
		}
		return 0;
	}

	shared_ptr<T> lock()
	{
		return (expired() ? shared_ptr<T>() : shared_ptr<T>(*this));
	}

	bool owner_before(const weak_ptr<T>& other) const noexcept { return c_ptr < other.c_ptr; }

	bool owner_before(const shared_ptr<T>& other) const noexcept { return c_ptr < other.c_ptr; }

};

template<class T>
class unique_ptr
{
private:
	T* m_ptr = nullptr;
public:
	unique_ptr(const unique_ptr& ptr) = delete;
	unique_ptr& operator=(unique_ptr& a) = delete;

	explicit unique_ptr(T* ptr) : m_ptr(ptr) { }

	~unique_ptr()
	{
		delete m_ptr;
		m_ptr = nullptr;
	}

	unique_ptr(unique_ptr&& a) noexcept //move constructor
	{
		m_ptr = a.m_ptr;
		a.m_ptr = nullptr;
	}

	T& operator*() const
	{
		return *m_ptr;
	}

	T* operator->() const
	{
		return m_ptr;
	}

	unique_ptr<T>& operator=(unique_ptr&& a) noexcept
	{
		if (&a == this) {
			return *this;
		}

		delete m_ptr;
		m_ptr = a.m_ptr;
		a.m_ptr = nullptr;

		return *this;
	}

	bool isNull() const { return m_ptr == nullptr; }

	T* get() const { return m_ptr; }

	T* release()
	{
		T* ptr = m_ptr;
		m_ptr = nullptr;
		return ptr;
	}

	void reset(T* ptr = nullptr) noexcept
	{
		delete m_ptr;
		m_ptr = ptr;
	}

	void swap(unique_ptr& other) noexcept
	{
		std::swap(this->m_ptr, other.m_ptr);
	}

	explicit operator bool() const noexcept { return (m_ptr != nullptr); }


};



template<class T>
class unique_ptr<T[]>
{
private:
	T* m_ptr;
public:
	unique_ptr(const unique_ptr& ptr) = delete;
	unique_ptr& operator=(unique_ptr& a) = delete;

	explicit unique_ptr(T* ptr) : m_ptr(ptr) { }

	~unique_ptr()
	{
		delete[] m_ptr;
		m_ptr = nullptr;
	}

	unique_ptr(unique_ptr&& a) noexcept //move constructor
	{
		m_ptr = a.m_ptr;
		a.m_ptr = nullptr;
	}

	T& operator*() const
	{
		return *m_ptr;
	}

	T* operator->() const
	{
		return m_ptr;
	}

	unique_ptr& operator=(unique_ptr&& a) noexcept
	{
		if (&a == this) {
			return *this;
		}

		delete m_ptr;
		m_ptr = a.m_ptr;
		a.m_ptr = nullptr;

		return *this;
	}

	T& operator[](int a) const { return m_ptr[a]; }

	bool isNull() const { return m_ptr == nullptr; }

	T* get() const { return m_ptr; }

	T* release()
	{
		T* ptr = m_ptr;
		m_ptr = nullptr;
		return ptr;
	}

	void reset(T* ptr = nullptr) noexcept
	{
		delete[] m_ptr;
		m_ptr = ptr;
	}

	void swap(unique_ptr& other) noexcept
	{
		std::swap(this->m_ptr, other.m_ptr);
	}

	explicit operator bool() const noexcept { return (m_ptr != nullptr); }
};

template<class U, class... Args>
std::enable_if_t<!std::is_array<U>::value, unique_ptr<U>>
make_unique(Args&&... args)
{
	return unique_ptr<U>(new U(std::forward<Args>(args)...));	//for no arrays
}

template<class U>
std::enable_if_t<std::is_array<U>::value, unique_ptr<U>>
make_unique(int n)
{
	return unique_ptr<U>(new std::remove_extent_t<U>[n]());	//for arrays
}

template<class T, class U>
bool operator==(const unique_ptr<T>& x, const unique_ptr<U>& y) { return x.get() == y.get(); }

template<class T, class U>
bool operator!=(const unique_ptr<T>& x, const unique_ptr<U>& y) { return x.get() != y.get(); }



#endif


