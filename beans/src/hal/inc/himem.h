#ifndef __HIMEMH__
#define __HIMEMH__

#include <arsenal.h>

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

void *  himemfree                           (void *         toFree);

void *  himemmalloc                         (UINT           size);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __HIMEMH__
