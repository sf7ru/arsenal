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
    seen        = 0;
    strEnd      = 0;
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
    if (seen)
    {
        //strz_dump("BEFORE SHAKE: ", buffData, used);

        if (seen < used)
        {
            used -= seen;

            if (strEnd)
            {
                strEnd -= seen;
            }

            memmove(buffData, buffData + seen, used);
        }
        else
            used = 0;

        //strz_dump("AFTER  SHAKE: ", buffData, used);

        seen = 0;
    }
}
// ***************************************************************************
// FUNCTION
//      AXBuffer::searchString
// PURPOSE
//
// PARAMETERS
//      UINT   start --
// RESULT
//      INT  --
// ***************************************************************************
INT AXBuffer::searchString(UINT start)
{
    INT           i_result        = 0;
    PU8           p_on;
    PU8           p_start;
    PU8           p_end;

    ENTER(true);

    p_start = buffData + start,
    p_end   = buffData + used;

//    printf(">> Search stage 1 start = %d (ch %X) used = %d strEnd = %d, *(strend) = %X (eol1 = %X eol2 = %X)\n", start, *p_start, used, strEnd, *(buffData + strEnd), eol1, eol2);
//    strz_dump_w_txt("  Search BUFF: ", buffData, used);

    for (p_on = p_start; !i_result && (p_on < p_end); p_on++)
    {
        if (*p_on == eol2)
        {

            strEnd      = (UINT)(p_on - buffData) + 1;
//            printf("Search stage 2 p_on = %X strEnd = %d, *(strend) = %X\n", *p_on, strEnd, *(buffData + strEnd));

            if (  eol1                      &&
                  ((p_on > buffData)        &&
                  (*(p_on - 1) == eol1))    )
            {
                *(p_on - 1)  = 0;
            }

            *p_on       = 0;
            i_result    = 1;
        }
    }
//    printf("<< Search stage 10  result = %d p_start = %X strEnd = %d, *(strend) = %X (eol1 = %X eol2 = %X)\n", i_result, *p_start, strEnd, *(buffData + strEnd), eol1, eol2);

    RETURN(i_result);
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
                    UINT           size,
                    EOLMODE        eolmode)
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
                     UINT           size,
                     EOLMODE        eolmode)
{
    BOOL            b_result        = false;

    ENTER(true);

    if (buff)
    {
        setEolMode(eolmode);

        buffData = (PU8)buff;
        dev         = theDev;
        buffSize    = size;
        used        = 0;
        seen        = 0;
        strEnd      = 0;
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
    INT     start;
    INT     rd;

    ENTER(true);

    shake();
    start = 0;

    //printf(">> GETS TO = %d\n", TO);

    while ((result <= 0) && (result = searchString(start)) == 0)
    {
        if (used == STR_MAX)
        {
            strEnd  = STR_MAX;
            result  = 1;
        }
        else if ((rd = dev->read(buffData + used, STR_MAX - used, TO)) > 0)
        {
//            *(buffData + used + rd) = 0;
//            printf("%s", buffData + used);

            //strz_dump_w_txt("   GETS HEX: ", buffData + used, rd);

            echo(rd, TO);

            start   = used;
            used   += rd;

//            printf("GETS (rd = %d, used = %d, seen = %d)\n", rd, used, seen);
//            strz_dump_w_txt("BUFF: ", buffData, used);
        }
        else
        {
            result = rd;
            break;
        }
    }

//    printf("<< GETS result: %d\n", result);

    return result;
}
void AXBuffer::echo(UINT           size,
                    UINT           TO)
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
            break;

        default:
            for (left = size, ok = true; ok && left; left--, on++)
            {
                ok = dev->write(on, 1, TO) > 0;

                if (eol1 && ok && (*on == eol1))
                {
                    dev->write(&eol2, 1, TO);
                }
            }
            break;
    }

}
INT AXBuffer::readDevice(UINT           TO)
{
    INT     result      = 0;

    if ((buffSize - used) > 0)
    {
        if ((result = dev->read(buffData + used, buffSize - used, TO)) > 0)
        {
            echo(result, TO);

            //strz_dump_w_txt("READ: ", buffData + used, result > 20 ? 20 : result);

            used += result;
            result = used;
        }
    }

    return result;
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
INT AXBuffer::read(UINT           TO,
                   INT            size)
{
    shake();

    return (INT)used > size ? used : readDevice(TO);
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

    if ((result = read(TO)) > 0)
    {
        result = MAC_MIN(used, (UINT)size);

        memcpy(data, buffData, result);

        purge(result);
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

//    strz_dump_w_txt("WRITE: ", (PVOID)data, size);
//        printf("WRITE: '%s'\n", data);

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

    if (strEnd)
    {
        result              = (PSTR) (buffData + seen);
        seen                = strEnd;
        strEnd              = 0;

//        printf("GETSTRZ = '%s'\n", result);
    }

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
