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

#include <stdlib.h>
#include <arsenal.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      a7utils_destroy
// PURPOSE
//
// PARAMETERS
//      PVOID   p_ptr --
// RESULT
//      PVOID  --
// ***************************************************************************
PVOID axutils_destroy(PVOID p_ptr)
{
    ENTER(p_ptr);

    FREE(p_ptr);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      read_int
// PURPOSE
//
// PARAMETERS
//      PCVOID p_int  --
//      UINT   size   --
//      BOOL   invert --
// RESULT
//      UINT --
// ***************************************************************************
UINT read_int(PCVOID          p_int,
              UINT            size,
              BOOL            invert)
{
    UINT            result          = 0;
    UINT            idx;
    PU8          on;

    ENTER(p_int && size);

    on = (PU8)p_int;

    if (invert)
    {
        for (idx = 0; idx < size; idx++, on++)
        {
            result = (result << 8) | *on;
        }
    }
    else
    {
        for (idx = 0, on += (size - 1); idx < size; idx++, on--)
        {
            result = (result << 8) | *on;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      read_int64
// PURPOSE
//
// PARAMETERS
//      PCVOID p_int  --
//      UINT   size   --
//      BOOL   invert --
// RESULT
//      U64 --
// ***************************************************************************
U64 read_int64(PCVOID          p_int,
               UINT            size,
               BOOL            invert)
{
    U64          result          = 0;
    UINT            idx;
    PU8          on;

    ENTER(p_int && size);

    on = (PU8)p_int;

    if (invert)
    {
        for (idx = 0; idx < size; idx++, on++)
        {
            result = (result << 8) | *on;
        }
    }
    else
    {
        for (idx = 0, on += (size - 1); idx < size; idx++, on--)
        {
            result = (result << 8) | *on;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      write_int
// PURPOSE
//
// PARAMETERS
//      PCVOID p_int  --
//      UINT   value  --
//      UINT   size   --
//      BOOL   invert --
// RESULT
//      void --
// ***************************************************************************
void write_int(PCVOID             p_int,
               UINT               value,
               UINT               size,
               BOOL               invert)
{
    UINT            idx;
    UINT            val     = value;
    PU8          on;

    ENTER(p_int && size);

    on = (PU8)p_int;

    if (invert)
    {
        for (idx = 0, on += (size - 1); idx < size; idx++, on--)
        {
            *on = (U8)val;
            val >>= 8;
        }
    }
    else
    {
        for (idx = 0; idx < size; idx++, on++)
        {
            *on = (U8)val;
            val >>= 8;
        }
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      a7utils_realloc_lesser
// PURPOSE
//
// PARAMETERS
//      PVOID mem  --
//      UINT  size --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID axutils_realloc_lesser(PVOID  mem,
                             UINT   size)
{
    PVOID           result;

    if ((result = REALLOC(mem, size)) == NULL)
    {
        result = mem;
    }

    return result;
}
