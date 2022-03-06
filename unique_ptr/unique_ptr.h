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
	unique_ptr(unique_ptr<T>&& a) noexcept;
	~unique_ptr();
	
	T& operator*() const;
	T* operator->() const;

	unique_ptr<T>& operator=(unique_ptr<T>&& a) noexcept;

	bool isNull() const;
};

template<class T, class...Args>
unique_ptr<T> make_unique(Args&&... args);

#endif

