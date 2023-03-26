#include <stdlib.h>
#include <arsenal.h>

void *operator new(size_t size)
{
    return MALLOC(size);
}

void *operator new[](size_t size)
{
    return MALLOC(size);
}

void operator delete(void *p)
{
    FREE(p);
}

void operator delete[](void *p)
{
    FREE(p);
}
