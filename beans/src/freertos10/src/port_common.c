
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

void *pvPortRealloc(void *mem, size_t newsize)
{
    if (newsize == 0) {
        vPortFree(mem);
        return NULL;
    }

    void *p;
    p = pvPortMalloc(newsize);
    if (p) {
        /* zero the memory */
        if (mem != NULL) {
            memcpy(p, mem, newsize);
            vPortFree(mem);
        }
    }
    return p;
}


void *pvPortCalloc(int count, size_t newsize)
{
    void *p;
    int fullSize = newsize * count;

    p = pvPortMalloc(fullSize);

    if (p) 
    {
        memset(p, 0, fullSize);
    }

    return p;
}

