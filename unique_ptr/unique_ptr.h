#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

template<class T>
class unique_ptr
{
private:
	T* m_ptr;
public:
	unique_ptr(T* ptr) : m_ptr(ptr) { }

	~unique_ptr()
	{
		delete m_ptr;
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
};

template<class T, class... Args>
unique_ptr<T> make_unique(Args&& ...args)
{
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif

