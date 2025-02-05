#include <malloc.h>

#ifdef __cplusplus
extern "C"
{
#endif
    extern size_t getTotalHeap(void);
    extern size_t getFreeHeap(void);
    extern void *gmalloc(size_t size);
    extern void *gcalloc(unsigned int n, size_t size);
#ifdef __cplusplus
}
#endif