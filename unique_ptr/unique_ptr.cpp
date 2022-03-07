#include "unique_ptr.h"
#include <utility>

template<class T>
unique_ptr<T>::unique_ptr(T* ptr) : m_ptr(ptr) { }

template<class T>
unique_ptr<T>::~unique_ptr()
{
	delete m_ptr;
}

template<class T>
unique_ptr<T>::unique_ptr(unique_ptr&& a) noexcept //move constructor
{
	delete m_ptr;

	m_ptr = a.m_ptr;
	a.m_ptr = nullptr;
}

template<class T>
T& unique_ptr<T>::operator*() const
{
	return *m_ptr;
}

template<class T>
T* unique_ptr<T>::operator->() const
{
	return m_ptr;
}

template<class T>
unique_ptr<T>& unique_ptr<T>::operator=(unique_ptr&& a) noexcept
{
	if (&a == this) {
		return *this;
	}
	
	delete m_ptr;
	m_ptr = a.m_ptr;
	a.m_ptr = nullptr;

	return *this;
}

template<class T>
bool unique_ptr<T>::isNull() const { return m_ptr == nullptr; }

template<class T, class... Args>
unique_ptr<T> make_unique(Args&& ...args)
{
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}
