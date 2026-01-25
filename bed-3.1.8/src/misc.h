#ifndef MISC_H
#define MISC_H
#include <stdint.h>
#include <type_traits>
template <typename T, int size> 
struct unsign {
	using type=T;
};

template  <typename T>
struct unsign<T,1> {
	using type=uint8_t;
	};
template  <typename T>
struct unsign<T,2> {
	using type=uint16_t;
	};

template  <typename T >
struct unsign<T,sizeof(uintptr_t)!=4?4:0> {
	using type=uint32_t;
};

template  <typename T>
struct unsign<T,sizeof(uintptr_t)> {
	using type=uintptr_t;
	};

template  <typename T >
struct unsign<T,sizeof(uintptr_t)!=8?8:INT_MAX> {
	using type=uint64_t;
};

template <class T> 
inline void myswap(T &a,T &b) {
using Uns=typename unsign<T,sizeof(T)>::type;  
	reinterpret_cast<Uns&>(a)^=reinterpret_cast<Uns&>(b);
	reinterpret_cast<Uns&>(b)^=reinterpret_cast<Uns&>(a);
	reinterpret_cast<Uns&>(a)^=reinterpret_cast<Uns&>(b);
	}
/*
inline void swap(T &a,T &b) {
	T tmp=a;
	a=b;
	b=tmp;
	}
*/

#endif
