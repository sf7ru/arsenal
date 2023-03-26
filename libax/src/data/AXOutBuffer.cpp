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
#include <AXOutBuffer.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define STR_MAX     (buffSize - 1)


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      AXOutBuffer::AXOutBuffer
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXOutBuffer::AXOutBuffer(void)
{
    buffData    = nil;
    used        = 0;
}
// ***************************************************************************
// FUNCTION
//      AXOutBuffer::~AXOutBuffer
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXOutBuffer::~AXOutBuffer(void)
{
    close();
}
// ***************************************************************************
// FUNCTION
//      AXOutBuffer::close
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXOutBuffer::close(void)
{
    BOOL            b_result        = false;

    ENTER(true);

    if (buffData)
    {
        FREE(buffData);
        buffData = nil;

        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXOutBuffer::open
// PURPOSE
//
// PARAMETERS
//      AXDevice *   dev  --
//      UINT         size --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXOutBuffer::open(AXDevice *     theDev,
                       UINT           size)
{
    BOOL            b_result        = false;

    ENTER(true);

    if (_realloc(size))
    {
        dev         = theDev;
        used        = 0;
        buffSize    = size;
        b_result    = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXOutBuffer::write
// PURPOSE
//
// PARAMETERS
//      PCVOID   data --
//      INT      size --
//      UINT     TO   --
// RESULT
//      INT  --
// ***************************************************************************
INT AXOutBuffer::write(PCVOID          data,
                       INT             size,
                       UINT            TO)
{
    INT     result      = -1;

    if (size >= (INT)buffSize)
    {
        if ((result = flush(TO)) > 0)
        {
            result = dev->write(data, size, TO);
        }
    }
    else if ((size + used) > buffSize)
    {
        if ((result = flush(TO)) > 0)
        {
            memcpy(buffData + used, data, size);
            used += size;
            result = size;
        }
    }
    else
    {
        memcpy(buffData + used, data, size);
        used   += size;
        result  = size;
    }

    return result;
}
INT AXOutBuffer::flush(UINT            TO)
{
    INT     result      = -1;

    if (used)
    {
        result = dev->write(buffData, used, TO);
        used   = 0;
    }

    return result;
}
BOOL AXOutBuffer::_realloc(UINT       size)
{
    BOOL        result          = false;

    ENTER(true);

    if (buffData && ((size != buffSize)))
    {
        FREE(buffData);
        buffData = nil;
    }

    // + 1 for strz
    if (buffData || ((buffData = (PU8)MALLOC(size + 1)) != nil))
    {
        result = true;
    }

    RETURN(result);
}
