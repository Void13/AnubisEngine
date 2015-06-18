#pragma once

#include "PhysXLoader.h"

class CPhysXAllocatorCallback : public PxAllocatorCallback
{
public:
	virtual ~CPhysXAllocatorCallback()
	{
		if (pData)
		{
			_aligned_free(pData);
			pData = nullptr;
		}
	}

	/**
	\brief Allocates size bytes of memory, which must be 16-byte aligned.

	This method should never return NULL.  If you run out of memory, then
	you should terminate the app or take some other appropriate action.

	<b>Threading:</b> This function should be thread safe as it can be called in the context of the user thread
	and physics processing thread(s).

	\param size			Number of bytes to allocate.
	\param typeName		Name of the datatype that is being allocated
	\param filename		The source file which allocated the memory
	\param line			The source line which allocated the memory
	\return				The allocated block of memory.
	*/
	void* allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		pData = _aligned_malloc(size, 16);

		return pData;
	};

	/**
	\brief Frees memory previously allocated by allocate().

	<b>Threading:</b> This function should be thread safe as it can be called in the context of the user thread
	and physics processing thread(s).

	\param ptr Memory to free.
	*/
	virtual void deallocate(void* ptr)
	{
		_aligned_free(ptr);
		pData = nullptr;
	};

private:
	void *pData = nullptr;
};