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
#include <string.h>

#include <string.h>
#include <axstring.h>
#include <AXBuffer.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define STR_MAX     (buffSize - 1)


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      AXBuffer::AXBuffer
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXBuffer::AXBuffer(void)
{
    buffData    = NULL;
    used        = 0;
    cursor      = 0;
    allocated   = false;

    echoMode    = 0;

    eol1        = '\r';
    eol2        = '\n';
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::~AXBuffer
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXBuffer::~AXBuffer(void)
{
    close();
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::shake
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      void  --
// ***************************************************************************
void AXBuffer::shake(void)
{
    if (cursor)
    {
        if (cursor < used)
        {
            used -= cursor;
            memmove(buffData, buffData + cursor, used);
        }
        else
            used = 0;

        cursor = 0;
    }
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::search
// PURPOSE
//
// PARAMETERS
//      UINT   start --
// RESULT
//      INT  --
// ***************************************************************************
INT AXBuffer::search(UINT start)
{
    INT             i_result        = 0;
    PU8           p_on;
    PU8           p_start;
    PU8           p_end;

    ENTER(true);

    p_start = buffData + start,
    p_end   = buffData + used;

    for (p_on = p_start; !i_result && (p_on < p_end); p_on++)
    {
        if (*p_on == eol2)
        {
            if (  eol1                      &&
                  ((p_on > p_start)         &&
                  (*(p_on - 1) == eol1))    )
            {
                *(p_on - 1) = 0;
            }

            *p_on       = 0;
            i_result    = 1;
            cursor      = (++p_on) - buffData;
        }
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::readAndSearch
// PURPOSE
//
// PARAMETERS
//      UINT   TO --
// RESULT
//      INT  --
// ***************************************************************************
INT AXBuffer::readAndSearch(UINT TO)
{
    INT     result        = -1;
    UINT    start;

    ENTER(true);

    while ((result = dev->read(buffData + used, STR_MAX - used, TO)) > 0)
    {
//        *(buffData + used) = 0;
//        printf("READ (%d): '%s'\n", result, buffData + used);
//        strz_dump("\n>> ", buffData + used, result);
//        printf("\n");


        echo(result, TO);

        start   = used;
        used   += result;

        if ((result = search(start)) == 0)
        {
            if (used == STR_MAX)
            {
                cursor  = STR_MAX;
                result  = 1;
                break;
            }
        }
        else
            break;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::close
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXBuffer::close(void)
{
    BOOL            b_result        = false;

    ENTER(true);

    if (buffData)
    {
        if (allocated && buffData)
        {
            allocated = false;
            FREE(buffData);
        }

        buffData = nil;

        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::open
// PURPOSE
//
// PARAMETERS
//      AXDevice *   dev  --
//      UINT         size --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXBuffer::open(AXDevice *     theDev,
                    UINT           size)
{
    BOOL            b_result        = false;

    ENTER(true);

    if (_realloc(size))
    {
        b_result    = open2(theDev, buffData, size);
    }

    RETURN(b_result);
}
BOOL AXBuffer::open2(AXDevice *     theDev,
                     PVOID          buff,
                     UINT           size)
{
    BOOL            b_result        = false;

    ENTER(true);

    if (buff)
    {
        buffData = (PU8)buff;
        dev         = theDev;
        buffSize    = size;
        used        = 0;
        cursor      = 0;
        b_result    = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::gets
// PURPOSE
//
// PARAMETERS
//      UINT   TO --
// RESULT
//      INT  --
// ***************************************************************************
INT AXBuffer::gets(UINT           TO)
{
    INT     result      = -1;

    shake();

    if ((result = search(0)) == 0)
    {
        result = readAndSearch(TO);
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::recv
// PURPOSE
//
// PARAMETERS
//      UINT   TO --
// RESULT
//      INT  --
// ***************************************************************************
INT AXBuffer::readFast(UINT           TO)
{
    shake();

    return used ? used : read(TO);
}
void AXBuffer::echo(UINT           size,
                    UINT            TO)
{
    PSTR    on     = (PSTR)(buffData + used);
    INT     left;
    BOOL    ok;

    switch (echoMode)
    {
        case 0:
            break;

        case 1:
            dev->write(on, size, TO);

        default:
            for (left = size, ok = true; ok && left; left--, on++)
            {
                ok = dev->write(on, 1, TO) > 0;

                if (ok && (*on == eol1))
                {
                    dev->write(&eol2, 1, TO);
                }
            }
            break;
    }

}
// ***************************************************************************
// FUNCTION
//      AXBuffer::read
// PURPOSE
//
// PARAMETERS
//      UINT TO --
// RESULT
//      INT --
// ***************************************************************************
INT AXBuffer::read(UINT           TO)
{
    INT     result      = -1;

    shake();

    result = dev->read(buffData + used, buffSize - used, TO);

    if (result > 0)
    {
        echo(result, TO);
        //strz_dump("\n>> ", buffData + used, result > 20 ? 20 : result);
        //printf("read %d\n", result);
        used    += result;
        result   = used;
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::recv
// PURPOSE
//
// PARAMETERS
//      UINT   TO --
// RESULT
//      INT  --
// ***************************************************************************
INT AXBuffer::read(PVOID          data,
                   INT            size)
{
    INT     result      = 0;

    if (used)
    {
        result = MAC_MIN(used, (UINT)size);

        memcpy(data, buffData, result);

        purge(result);
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::read
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      INT   size --
//      UINT  TO   --
// RESULT
//      INT --
// ***************************************************************************
INT AXBuffer::read(PVOID          data,
                   INT            size,
                   UINT           TO)
{
    INT     result      = 0;

    if ((result = read(data, size)) == 0)
    {
        if (read(TO))
        {
            result = read(data, size);
        }
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::write
// PURPOSE
//
// PARAMETERS
//      PCVOID   data --
//      INT      size --
//      UINT     TO   --
// RESULT
//      INT  --
// ***************************************************************************
INT AXBuffer::write(PCVOID          data,
                    INT             size,
                    UINT            TO)
{
    INT     result      = -1;

    result = dev->write(data ? data : buffData, size, TO);

    return result;
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::getStrz
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      PSTR --
// ***************************************************************************
PSTR AXBuffer::getStrz(void)
{
    PSTR    result        = NULL;

    ENTER(true);

    result              = (PSTR)(buffData);
    *(result + used)    = 0;

    RETURN(result);
}
BOOL AXBuffer::_realloc(UINT       size)
{
    BOOL        result          = false;

    ENTER(true);

    if (buffData && ((size != buffSize)))
    {
        FREE(buffData);
        buffData = nil;
    }

    if (!buffData)
    {
        // + 1 for strz
        if  ((buffData = (PU8)MALLOC(size + 1)) != nil)
        {
            allocated = true;
            result    = true;
        }
    }
    else
        result = true;

    RETURN(result);
}
