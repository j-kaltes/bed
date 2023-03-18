#pragma once

#ifdef ALIGNMEMORY 
	#ifdef __MINGW32__
	#define alignedalloc(bs,size) (char *)::_aligned_malloc(size,bs)
	#define alignedfree(x) ::_aligned_free(x)
	#else
	#define alignedalloc(bs,size) (char *)::aligned_alloc(bs, size)
	#define alignedfree(x) ::free(x)
	#endif
#else
#define alignedalloc(bs,size) (char *)::malloc(size)
#define alignedfree(x) ::free(x)
#endif
