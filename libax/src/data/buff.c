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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <axdata.h>
#include <axsystem.h>
#include <axstring.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

/*
// ***************************************************************************
// FUNCTION
//      _search
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PAXBUFF   pst_buff --
//      UINT      u_start  --
// RESULT
//      INT --
// ***************************************************************************
static INT _search(PAXBUFF  pst_buff,
                   UINT     u_start)
{
    INT             i_result        = 0;
    PU8             p_on;
    PU8             p_end;

    ENTER(true);

    p_on    = pst_buff->ab_data + u_start,
    p_end   = pst_buff->ab_data + pst_buff->u_used;

    for (; !i_result && (p_on < p_end); p_on++)
    {
        switch (*p_on)
        {
            case '\radius':
                if (((p_on + 1) < p_end) && (*(p_on + 1) == '\n'))
                {
                    *(p_on++) = 0;
                }
                // No 'break;' here

            case '\n':
                *p_on                   = 0;
                i_result                = 1;
                pst_buff->u_index    = ++p_on - pst_buff->ab_data;
                break;

            default:
                break;
        }
    }

    RETURN(i_result);
}
*/
// ***************************************************************************
// FUNCTION
//      _search
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PAXBUFF   pst_buff --
//      UINT      u_start  --
// RESULT
//      INT --
// ***************************************************************************
static INT _search(PAXBUFF  pst_buff,
                   UINT     u_start,
                   CHAR     c_n)
{
    INT             i_result        = 0;
    PU8             p_on;
    PU8             p_start;
    PU8             p_end;

    ENTER(true);

    p_start = pst_buff->ab_data + u_start,
    p_end   = pst_buff->ab_data + pst_buff->u_used;

    for (p_on = p_start; !i_result && (p_on < p_end); p_on++)
    {
        if (*p_on == c_n)
        {
            if ((p_on > p_start) && (*(p_on - 1) < ' '))
            {
                *(p_on - 1)     = 0;
            }

            *p_on               = 0;
            i_result            = 1;
            pst_buff->u_index   = ++p_on - pst_buff->ab_data;
        }
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      _read_n_search
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PAXBUFF   pst_buff --
//      UINT    u_mTO    --
// RESULT
//      INT --
// ***************************************************************************
static INT _read_n_search(PAXBUFF       pst_buff,
                          PAXDEV        pst_dev,
                          CHAR          c_n,
                          UINT          u_mTO)
{
    INT     i_result        = -1;
    UINT    u_start;
    UINT    u_max;

    ENTER(true);

    u_max = pst_buff->u_size - 1;

    while ((i_result = axdev_read(pst_dev,
                            pst_buff->ab_data + pst_buff->u_used,
                            u_max - pst_buff->u_used, u_mTO)) > 0)
    {
        //*(pst_buff->ab_data + pst_buff->u_used + i_result) = 0;
        //printf(" {{{%s}}}\n", pst_buff->ab_data + pst_buff->u_used);
        //la6_dump_print("READ: ", pst_buff->ab_data + pst_buff->u_used, i_result);

        u_start             = pst_buff->u_used;
        pst_buff->u_used   += i_result;

        if ((i_result = _search(pst_buff, u_start, c_n)) == 0)
        {
            if (pst_buff->u_used == u_max)
            {
                pst_buff->u_index    = u_max;
                i_result                = 1;
                break;
            }
        }
        else
            break;
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      _shake
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PAXBUFF    pst_buff --
// RESULT
//      void --
// ***************************************************************************
static void _shake(PAXBUFF        pst_buff)
{
    ENTER(true);

    if (pst_buff->u_index)
    {
        if (pst_buff->u_index < pst_buff->u_used)
        {
            pst_buff->u_used -= pst_buff->u_index;

            memmove(pst_buff->ab_data,
                    pst_buff->ab_data + pst_buff->u_index,
                    pst_buff->u_used);
        }
        else
            pst_buff->u_used = 0;

        pst_buff->u_index = 0;
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axbuff_create
// PURPOSE
//
// PARAMETERS
//      UINT   u_size --
// RESULT
//      PAXBUFF   --
// ***************************************************************************
PAXBUFF  axbuff_create(UINT u_size)
{
    PAXBUFF        pst_axbuff    = NULL;

    ENTER(u_size);

    if ((pst_axbuff = CREATE_X(AXBUFF , u_size + 1)) != NULL)
    {
        pst_axbuff->u_size = u_size;
    }

    RETURN(pst_axbuff);
}
// ***************************************************************************
// FUNCTION
//      axbuff_gets
// PURPOSE
//
// PARAMETERS
//      PAXBUFF    pst_buff --
//      PAXDEV     pst_dev  --
//      UINT       u_mTO    --
// RESULT
//      INT  --
// ***************************************************************************
INT axbuff_gets_x(PAXBUFF       pst_buff,
                  PAXDEV        pst_dev,
                  CHAR          c_n,
                  UINT          u_mTO)
{
    INT     i_result            = -1;

    ENTER(pst_buff);

    _shake(pst_buff);

    if ((i_result = _search(pst_buff, 0, c_n)) == 0)
    {
        if (pst_dev)
        {
            i_result = _read_n_search(pst_buff, pst_dev, c_n, u_mTO);
        }
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axbuff_gets
// PURPOSE
//
// PARAMETERS
//      PAXBUFF    pst_buff --
//      PAXDEV     pst_dev  --
//      UINT       u_mTO    --
// RESULT
//      INT  --
// ***************************************************************************
INT axbuff_recv(PAXBUFF        pst_buff,
                 PAXDEV         pst_dev,
                 UINT           u_mTO)
{
    INT     i_result            = -1;
    UINT    u_left;

    ENTER(pst_buff && pst_dev);

    _shake(pst_buff);

    if ((u_left = pst_buff->u_size  - pst_buff->u_used) > 0)
    {
        switch (i_result = axdev_read(pst_dev,
                                      pst_buff->ab_data + pst_buff->u_used,
                                      pst_buff->u_size  - pst_buff->u_used, u_mTO))
        {
            case -1:
                break;

            case 0:
                i_result = pst_buff->u_used;
                break;

            default:
                pst_buff->u_used   += i_result;
                i_result            = pst_buff->u_used;
                break;
        }
    }
    else
        i_result = pst_buff->u_used;

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axbuff_read
// PURPOSE
//
// PARAMETERS
//      PAXBUFF    pst_buff --
//      PVOID      p_data   --
//      UINT       u_size   --
// RESULT
//      INT  --
// ***************************************************************************
INT axbuff_get_count(PAXBUFF        pst_buff)
{
    INT     i_result            = -1;

    ENTER(pst_buff);

    i_result = pst_buff->u_used - pst_buff->u_index;

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axbuff_read
// PURPOSE
//
// PARAMETERS
//      PAXBUFF    pst_buff --
//      PVOID      p_data   --
//      UINT       u_size   --
// RESULT
//      INT  --
// ***************************************************************************
INT axbuff_read(PAXBUFF        pst_buff,
                 PVOID          p_data,
                 UINT           u_size)
{
    INT     i_result            = -1;
    UINT    u_left;

    ENTER(pst_buff && u_size);

    if ((u_left = (pst_buff->u_used - pst_buff->u_index)) > 0)
    {
        i_result = u_size > u_left ? u_left : u_size;

        if (p_data)
        {
            memcpy(p_data, pst_buff->ab_data + pst_buff->u_index,  i_result);
        }

        pst_buff->u_index += i_result;
    }
    else
        i_result = 0;

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axbuff_append
// PURPOSE
//
// PARAMETERS
//      PAXBUFF    pst_buff --
//      PVOID      p_data   --
//      UINT       u_size   --
// RESULT
//      INT  --
// ***************************************************************************
INT axbuff_append(PAXBUFF        pst_buff,
                   PVOID          p_data,
                   UINT           u_size)

{
    INT     i_result            = -1;
    UINT    u_left;

    ENTER(pst_buff && p_data && u_size);

    if ((u_left = (pst_buff->u_size - pst_buff->u_used)) > 0)
    {
        i_result = u_size > u_left ? u_left : u_size;

        memcpy(pst_buff->ab_data + pst_buff->u_used, p_data, i_result);

        pst_buff->u_used += i_result;
    }
    else
        i_result = 0;

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axbuff_pop
// PURPOSE
//
// PARAMETERS
//      PAXBUFF   pst_buff --
//      PVOID   p_data   --
//      INT     i_size   --
// RESULT
//      INT  --
// ***************************************************************************
UINT axbuff_pop(PAXBUFF     pst_buff,
                PVOID       p_data,
                UINT        u_size)
{
    UINT            u_read          = 0;

    ENTER(pst_buff && p_data && u_size);

    u_read  = MAC_MIN(u_size, pst_buff->u_used);

    memcpy(p_data, pst_buff->ab_data, u_read);

    pst_buff->u_used -= u_read;

    memmove(pst_buff->ab_data,
           pst_buff->ab_data + u_read,
           pst_buff->u_used);

    RETURN(u_read);
}
// ***************************************************************************
// FUNCTION
//      axbuff_push
// PURPOSE
//
// PARAMETERS
//      PAXBUFF   pst_buff --
//      PVOID     p_data     --
//      UINT      u_size     --
// RESULT
//      UINT  --
// ***************************************************************************
UINT axbuff_push(PAXBUFF    pst_buff,
                 PVOID      p_data,
                 UINT       u_size)
{
    INT             i_written       = 0;
    UINT            u_tail;
    UINT            u_offset;

    ENTER(pst_buff && p_data && u_size);

    i_written   = MAC_MIN(u_size, pst_buff->u_size);
    u_tail      = pst_buff->u_size - pst_buff->u_used;

    if (u_tail <= (unsigned)i_written)
    {
        if (((UINT)i_written) < pst_buff->u_size)
        {
            u_offset            = pst_buff->u_size - i_written;
            pst_buff->u_used   -= u_offset;

            memcpy(pst_buff->ab_data,
                   pst_buff->ab_data + u_offset,
                   pst_buff->u_used);
        }
        else
            pst_buff->u_used = 0;
    }

    memcpy(pst_buff->ab_data + pst_buff->u_used, p_data, i_written);

    pst_buff->u_used += i_written;

    RETURN(i_written);
}

