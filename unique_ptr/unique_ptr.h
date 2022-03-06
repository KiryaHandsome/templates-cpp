#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

template<class T>
class unique_ptr
{
private:
	T* m_ptr;
public:
	unique_ptr(unique_ptr<T>& a) = delete;
	unique_ptr<T>& operator=(unique_ptr<T>& a) = delete;


	unique_ptr(T* ptr = nullptr);
	unique_ptr(unique_ptr<T>&& a);
	~unique_ptr();
	
	T& operator*() const;
	T* operator->() const;

	unique_ptr<T>& operator=(unique_ptr<T>&& a);

	bool isNull() const;

};

#endif

