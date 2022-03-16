#ifndef WEAK_PTR_H
#define WEAK_PTR_H

#include "shared_ptr.h"


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

#endif

