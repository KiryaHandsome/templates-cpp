#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include <utility>

template<class T>
struct ControlBlock
{
	ControlBlock(T* ptr) : object(ptr), sharedCount(1)
	{
	}

	template<class ...Args>
	ControlBlock(Args&&... arg)		//constructor for make_shared
	{
		object = new T(std::forward<Args>(arg)...);
		sharedCount = 1;
	}

	T* object = nullptr;
	int sharedCount = 0;
	int weakCount = 0;

	void incSharedCount() { sharedCount++; }
	void decSharedCount() { sharedCount--; }
	void incWeakCount() { weakCount++; }
	void decWeakCount() { weakCount--; }
};

template<class T>
class shared_ptr
{
private:
	template<class Y> friend class shared_ptr;
	template<class Y> friend class weak_ptr;
	ControlBlock<T>* c_ptr = nullptr;
public:
	shared_ptr() //default constructor
	{
	}

	shared_ptr(T* ptr) : c_ptr(new ControlBlock<T>(ptr))
	{
	}

	~shared_ptr()
	{
		if (!c_ptr || !c_ptr->object) return;
		c_ptr->decSharedCount();
		if (c_ptr->sharedCount <= 0) {
			delete c_ptr->object;
			c_ptr->object = nullptr;
			if (c_ptr->weakCount <= 0) {
				delete c_ptr;
				c_ptr = nullptr;
			}
		}
	}

	//***copy semantic***//

	shared_ptr(const shared_ptr& obj)
		: c_ptr(obj.c_ptr)
	{
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
		if (c_ptr) c_ptr->incSharedCount();

		return *this;
	}

	//***move semantic***//

	shared_ptr(shared_ptr&& obj) noexcept
	{
		c_ptr = obj.c_ptr;
		obj.c_ptr = nullptr;
	}

	shared_ptr<T>& operator=(shared_ptr<T>&& obj) noexcept
	{
		if (this->c_ptr) {
			c_ptr->decSharedCount();
		}
		c_ptr = obj.c_ptr;
		obj.c_ptr = nullptr;

		return *this;
	}

	T* operator->() const
	{
		return c_ptr->object;
	}

	T& operator*() const
	{
		return *(c_ptr->object);
	}

	int use_count() const
	{
		if (c_ptr) return c_ptr->sharedCount;
		return 0;
	}

	bool isNull() const
	{
		return (c_ptr == nullptr);
	}

	shared_ptr(ControlBlock<T>* cptr) : c_ptr(cptr)	//constructor for make_shared
	{
		c_ptr->object = cptr->object;
	}
};

template<class T, class... Args>
shared_ptr<T> make_shared(Args&& ...args)
{
	return shared_ptr<T>(new ControlBlock<T>(std::forward<Args>(args)...));
}



#endif

