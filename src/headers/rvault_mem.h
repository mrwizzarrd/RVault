#ifndef RVAULT_MEM_H
#define RVAULT_MEM_H

#include <sodium.h>

template <typename T>
class rvault_allocator {
public:
	using value_type = T;

	T* allocate(std::size_t n){
		if(n == 0){
			n = 1;
		}
		return reinterpret_cast<T*>(sodium_malloc(n * sizeof(T)));
	}

	void deallocate(T* ptr, std::size_t n){
	    if(ptr){
	        sodium_free(ptr);	
	    }
	}

};



#endif
