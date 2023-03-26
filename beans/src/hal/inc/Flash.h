// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#include <arsenal.h>

#ifndef __FLASH_H__
#define __FLASH_H__


typedef struct _tag_FLASHFILEPROPS
{
    PVOID                   ptr;
    UINT                    begin;
    UINT                    sectors;
    INT                     size;
} FLASHFILEPROPS, * PFLASHFILEPROPS;


#endif // #ifndef __FLASH_H__

