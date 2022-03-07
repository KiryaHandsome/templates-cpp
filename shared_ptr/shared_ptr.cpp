#include "shared_ptr.h"
#include <utility>




template<class T>
shared_ptr<T>::shared_ptr() : m_ptr(nullptr), refCount(new int(0))	//default constructor
{
}

template<class T>
shared_ptr<T>::shared_ptr(T* ptr) : m_ptr(ptr), refCount(new int(1))
{
}

template<class T>
shared_ptr<T>::~shared_ptr()
{
	if (m_ptr != nullptr) {
		(*refCount)--;
		if ((*refCount) == 0) {
			delete m_ptr;
			delete refCount;
		}
	}
}


//***copy semantic***//

template<class T>
shared_ptr<T>::shared_ptr(const shared_ptr& obj)	
{
	m_ptr = obj.m_ptr;
	refCount = obj.refCount;	//copy values

	if (m_ptr != nullptr) (*refCount)++;	//increase this count
}

template<class T>
shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr<T>& obj)
{
	if (obj.m_ptr != m_ptr) {
		if (m_ptr != nullptr) (*refCount)--;	//decrease this count

		m_ptr = obj.m_ptr;
		refCount = obj.refCount;	//copy values

		if (m_ptr != nullptr) (*obj.refCount)++;	//increase common count
	}
	
	return *this;
}


//***move semantic***//

template<class T>
shared_ptr<T>::shared_ptr(shared_ptr&& obj) noexcept
{
	m_ptr = obj.m_ptr;		//share obj
	refCount = obj.refCount;

	obj.refCount = nullptr;	 //clean old values
	obj.m_ptr = nullptr;
}

template<class T>
shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr<T>&& obj) noexcept		
{
	if (this->m_ptr != nullptr) (*refCount)--;	//decrease obj count

	m_ptr = obj.m_ptr;		//share obj
	refCount = obj.refCount;

	obj.refCount = nullptr;	 //clean old values
	obj.m_ptr = nullptr;

	return *this;
}

template<class T>
T* shared_ptr<T>::operator->() const
{
	return m_ptr;
}

template<class T>
T& shared_ptr<T>::operator*() const
{
	return *m_ptr;
}

template<class T>
int shared_ptr<T>::getCount() const
{
	return *refCount;
}

template<class T>
bool shared_ptr<T>::isNull() const
{
	return (m_ptr == nullptr);
}

template<class T, class... Args>
shared_ptr<T> make_shared(Args&& ...args)
{
	return shared_ptr<T>(new T(std::forward<Args>(args)...));
}
