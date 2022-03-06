#ifndef SHARED_PTR_H
#define SHARED_PTR_H


template<class T>
class shared_ptr
{
private:
	T* m_ptr = nullptr;
	int* refCount = nullptr;
public:
	shared_ptr();
	shared_ptr(T* ptr);
	shared_ptr(const shared_ptr& a);
	shared_ptr(shared_ptr&& a) noexcept;
	~shared_ptr();

	shared_ptr<T>& operator=(const shared_ptr<T>& a);

	shared_ptr<T>& operator=(shared_ptr<T>&& a) noexcept;	


	T* operator->() const;
	T& operator*() const;

	int getCount() const;
	bool isNull() const;
};

template<class T, class... Args>
shared_ptr<T> make_shared(Args&& ...args);

#endif

