#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

#include <compare>

template<class T>
class unique_ptr
{
private:
	T* m_ptr = nullptr;
public:
	unique_ptr(T* ptr) : m_ptr(ptr) { }

	~unique_ptr()
	{
		delete m_ptr;
		m_ptr = nullptr;
	}

	unique_ptr(unique_ptr&& a) noexcept //move constructor
	{
		delete m_ptr;

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
	unique_ptr(T* ptr) : m_ptr(ptr) { }

	~unique_ptr()
	{
		delete [] m_ptr;
		m_ptr = nullptr;
	}

	unique_ptr(unique_ptr&& a) noexcept //move constructor
	{
		delete m_ptr;

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
		delete [] m_ptr;
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

