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

// TODO:

// %lu %llu %ld %2.4x

/*
double round(double scr, int rank) //������ �������� ��� 10,100,1000, � ����� ���������� ����� ��� �������� ����������
{
double ret,nret,r5=5,r1=1;
int rnk;
rnk=rank*10;
r5=r5/rnk;
r1=r1/rank;
nret=floor(src*rnk)/rnk;
ret=floor(src*rank)/rank;
if(fabs(nret-ret)>=r5)
{
ret+=r1;
}
return ret;
}

/---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "Round.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

double __fastcall Round(double Argument, int Precision)
{
double div = 1.0;
if(Precision >= 0)
while(Precision--)
div *= 10.0;
else
while(Precision++)
div /= 10.0;
return floor(Argument * div + 0.5) / div;
}


double round(double src, int rank)
{
double ret;
ret=floor(src*rank+0.5)/rank;
return ret;
}

double RoundTo(double value,int precision)
{
//precision - ���-�� ������ ����� �������
int multipler = pow(10,precision);
return floor(value*multipler+0.5)/multipler;
}
 */

#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <axstring.h>
#include <string.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define FLAG_MINUS          1
#define FLAG_SIGN           2
#define FLAG_BIG            4
#define FLAG_REVERT         8

#define DECIMAL_DIGITS      10

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_P
{
    int         len;
    int         sign;
    int         p1;
    int         p2;
    int         size;
    va_list     list;
    PCSTR       on;
    PFNPUTCHAR  pfn_putchar;
    PVOID       ptr;
} P, * PP;

typedef struct __tag_STORSTR
{
    PSTR        on;
    INT         left;
} STORSTR, * PSTORSTR;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _x_putchar
// PURPOSE
//
// PARAMETERS
//      PP   p --
//      CHAR c --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _x_putchar(PP        p,
                       CHAR      c)
{
    BOOL            result        = false;
    INT             rd  = 0;

    ENTER(true);

    if ((rd = p->pfn_putchar(p->ptr, c)) > 0)
    {
        p->len += rd;
        result  = true;
    }

    RETURN(result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _get_opts
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      void --
// ***************************************************************************
static void _get_opts(PP        p)
{
    int *       currP;

    ENTER(true);

    p->sign = 0;
    p->p1   = 0;
    p->p2   = 0;
    p->size = 0;

    if (*p->on == '-')
    {
        p->on++;
    }

    if (*p->on == '+')
    {
        p->on++;
    }

    currP   = &p->p1;

    for(; *p->on; p->on++)
    {
        if ((*p->on >= '0') && (*p->on <= '9'))
        {
            *currP *= 10;
            *currP += (*p->on - '0');
        }
        else
        {
            if (*p->on == '.')
            {
                currP = &p->p2;
            }
            else if (*p->on == 'l')
            {
                p->size++;
            }
            else
                break;
        }
    }

    QUIT;
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_string
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_string(PP         p)
{
    BOOL        result      = true;
    PCSTR       on;

    ENTER(true);

    if ((on = va_arg(p->list, PSTR)) == NULL)
        on = "(null)";

    while (result && *on)
    {
        result = _x_putchar(p, *(on++));
    }

    RETURN(result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _binary
// PURPOSE
//
// PARAMETERS
//      PP  p     --
//      int flags --
//      INT arg   --
//      INT size  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _binary(PP      p,
                    int     flags,
                    INT    arg,
                    INT    size)
{
    BOOL        result      = true;
    PU8       at;
    U8        dig;
    INT         cd;
    int         bit;
    int         sz;

    ENTER(true);

    at  = (PU8)&arg;
    cd  = size;

    if ((sz  = (size >> 3)) > 0)
        at += (sz - 1);

    if (!(bit = size & 7))
        bit = 8;

    for (; result && cd; at--)
    {
        dig = *at;

        while (result && cd && bit--)
        {
            result = _x_putchar(p, '0' + ((dig >> bit) & 1));
            cd--;
        }

        bit = 8;
    }

    RETURN(result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_binary
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_binary(PP     p)
{
    INT        arg;

    arg = va_arg(p->list, INT);

    return _binary(p, 0, arg, p->p1 ? p->p1 : sizeof(arg) * 8);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _spacesAndZeroed
// PURPOSE
//
// PARAMETERS
//      PP   p       --
//      INT  numbers --
//      PINT spaces  --
//      PINT zeroes  --
//      PINT sign    --
//      BOOL flag    --
// RESULT
//      void --
// ***************************************************************************
static void _spacesAndZeroed(PP         p,
                             INT        numbers,
                             PINT       spaces,
                             PINT       zeroes,
                             PINT       sign,
                             BOOL       flag)
{
    if ((p->p2 > numbers))
    {
        *zeroes = p->p2 - numbers;
    }

    if (flag)
        (*sign)++;

    if (p->p1 > (numbers + *zeroes + *sign))
    {
        *spaces = p->p1 - (numbers + *zeroes + *sign);
    }

}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _hexadecimal
// PURPOSE
//
// PARAMETERS
//      PP  p     --
//      int flags --
//      PU8 arg   --
//      INT size  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _hexadecimal(PP             p,
                         int            flags,
                         PU8          arg,
                         INT            size)
{
    BOOL        result      = true;
    PU8       at;
    int         delta;
    int         dig;
    INT         cd;
    int         aaa;
    int         numbers;
    int         spaces      = 0;
    int         zeroes      = 0;
    int         sign        = 0;

    BOOL        meaned      = false;

    ENTER(true);

    numbers = size;

    _spacesAndZeroed(p, numbers, &spaces, &zeroes, &sign, false);

    if (flags & FLAG_REVERT)
    {
        delta   = -1;
        at      = arg + size - 1;
    }
    else
    {
        delta   = 1;
        at      = arg;
    }

    aaa = (flags & FLAG_BIG) ? ('A' - 10) : ('a' - 10);

    for (; result && spaces; spaces--)
    {
        result = _x_putchar(p, ' ');
    }

//    for (; result && zeroes; zeroes--)
//    {
//        result = _x_putchar(p, '0');
//    }

    for (cd = numbers; result && cd; at += delta)
    {
        if (meaned || (*at != 0) || cd == 1)
        {
            if (*at != 0)
                meaned = true;

            dig = ((*at) >> 4) & 0xf;
            dig += dig > 9 ? aaa : '0';

            if ((result = _x_putchar(p, dig)) != false)
            {
                dig = (*at) & 0xf;
                dig += dig > 9 ? aaa : '0';

                result = _x_putchar(p, dig);

                cd--;
            }
        }
        else
            cd--;
    }

    RETURN(result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_hex
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_hex(PP        p)
{
    INT    arg;

    arg = va_arg(p->list, unsigned);

    return _hexadecimal(p, FLAG_REVERT, (PU8)&arg, p->p2 ? p->p2 : sizeof(arg));
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_HEX
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_HEX(PP        p)
{
    INT    arg;

    arg = va_arg(p->list, unsigned);

    return _hexadecimal(p, FLAG_BIG | FLAG_REVERT, (PU8)&arg, sizeof(arg));
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_pointer
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_pointer(PP    p)
{
    PVOID   arg;

    arg = va_arg(p->list, PVOID);

    return _hexadecimal(p, FLAG_REVERT, (PU8)&arg, sizeof(arg));
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_POINTER
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_POINTER(PP        p)
{
    PVOID   arg;

    arg = va_arg(p->list, PVOID);

    return _hexadecimal(p, FLAG_BIG | FLAG_REVERT, (PU8)&arg, sizeof(arg));
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_dump
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_dump(PP       p)
{
    PU8   arg1;
    int     arg2;

    arg1 = va_arg(p->list, PU8);
    arg2 = va_arg(p->list, int);

    return _hexadecimal(p, 0, arg1, arg2);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_DUMP
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_DUMP(PP       p)
{
    PU8   arg1;
    int     arg2;

    arg1 = va_arg(p->list, PU8);
    arg2 = va_arg(p->list, int);

    return _hexadecimal(p, FLAG_BIG, arg1, arg2);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _invertedDecimal
// PURPOSE
//
// PARAMETERS
//      PSTR          buff   --
//      int           digits --
//      unsigned long arg    --
// RESULT
//      int --
// ***************************************************************************
static int _invertedDecimal(PSTR             buff,
                            int              digits,
                            unsigned long    arg)
{
    PSTR        at          = buff;
    int         left        = digits;

    for (; arg && left; left--, at++)
    {
        *at = ('0' + (U8)(arg % 10));

        arg /= 10;
    }

    if (at == buff)
    {
        *(at++) = '0';
    }

    return at - buff;
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _decimal
// PURPOSE
//
// PARAMETERS
//      PP            p      --
//      int           flags  --
//      int           digits --
//      unsigned long arg    --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _decimal(PP              p,
                     int             flags,
                     int             digits,
                     unsigned long   arg)
{
    BOOL        result      = true;
    PSTR        at;
    int         numbers;
    int         spaces      = 0;
    int         zeroes      = 0;
    int         sign        = 0;
    CHAR        inverted    [ DECIMAL_DIGITS ];

    ENTER(true);

    numbers = _invertedDecimal(inverted, digits, arg);
    _spacesAndZeroed(p, numbers, &spaces, &zeroes, &sign, flags & FLAG_MINUS);

    for (; result && spaces; spaces--)
    {
        result = _x_putchar(p, ' ');
    }

    if (sign)
    {
        result = _x_putchar(p, '-');
    }

    for (; result && zeroes; zeroes--)
    {
        result = _x_putchar(p, '0');
    }

    for (at = inverted + numbers - 1; result && (at >= inverted); at--)
    {
        result = _x_putchar(p, *at);
    }

    RETURN(result);
}
#ifndef STRZ_NO_FLOAT
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _double
// PURPOSE
//
// PARAMETERS
//      PP     p     --
//      int    flags --
//      double arg   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _double(PP              p,
                    int             flags,
                    double          arg)
{
    BOOL            result      = true;
    PSTR            at;
    double          tmp;
    int             len;
    int             quot;
    int             rem;
    int             spaces      = 0;
    int             sign        = 0;
    int             remnumbers;
    CHAR            quotStr     [ 10 ];
    CHAR            remStr      [ 10 ];

    ENTER(true);

    rem         = _invertedDecimal(remStr,  10, (unsigned long)(modf(arg, &tmp) * 1e9));
    quot        = _invertedDecimal(quotStr, 10, (unsigned long)tmp);
    remnumbers  = MAC_MIN(rem, p->p2);

    if (flags & FLAG_MINUS)
        sign++;

    len         = remnumbers + quot + sign + 1;

    if (p->p1 > len)
    {
        spaces = p->p1 - len;
    }

    for (; result && spaces; spaces--)
    {
        result = _x_putchar(p, ' ');
    }

    if (flags & FLAG_MINUS)
    {
        result = _x_putchar(p, '-');
    }

    for (at = quotStr + quot - 1; result && (at >= quotStr); at--)
    {
        result = _x_putchar(p, *at);
    }

    result = _x_putchar(p, '.');

    for (at = remStr + rem - 1; result && remnumbers; at--, remnumbers--)
    {
        result = _x_putchar(p, *at);
    }

    RETURN(result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_float
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_float(PP        p)
{
    double      val;

    val = va_arg(p->list, double);

    if (!p->p2)
        p->p2 = 6;

    return (val < 0) ? _double(p, FLAG_MINUS, -val) : _double(p, 0, val);

}
#endif
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_iunsigned
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      INT --
// ***************************************************************************
static INT _type_iunsigned(PP      p)
{
    return _decimal(p, 0, DECIMAL_DIGITS, va_arg(p->list, unsigned));
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_isigned
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      INT --
// ***************************************************************************
static INT _type_isigned(PP    p)
{
    int         arg;

    arg = va_arg(p->list, int);

    return (arg < 0) ? _decimal(p, FLAG_MINUS, DECIMAL_DIGITS, -arg) : _decimal(p, 0, DECIMAL_DIGITS, arg);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_percent
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_percent(PP        p)
{
    return _x_putchar(p, '%');
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _type_char
// PURPOSE
//
// PARAMETERS
//      PP p --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _type_char(PP        p)
{
    return _x_putchar(p, va_arg(p->list, int));
}
// ***************************************************************************
// FUNCTION
//      strz_vformat
// PURPOSE
//
// PARAMETERS
//      PFNPUTCHAR pfn_putchar --
//      PVOID      pfn_ptr     --
//      PCSTR      format      --
//      va_list    list        --
// RESULT
//      INT --
// ***************************************************************************
INT strz_vformat(PFNPUTCHAR     pfn_putchar,
                 PVOID          pfn_ptr,
                 PCSTR          format,
                 va_list        list)
{
#define TYPE(s,t) case s: ok = _type_##t(&p); break

    BOOL        ok          = true;
    P           p;

    ENTER(true);

    p.len           = 0;
    p.on            = format;
    p.pfn_putchar   = pfn_putchar;
    p.ptr           = pfn_ptr;

    axva_copy(p.list, list);

    while (ok && *p.on)
    {
        if (*p.on == '%')
        {
            ++p.on;

            _get_opts(&p);

            switch (*(p.on++))
            {
                TYPE('%', percent);

                TYPE('s', string);
                TYPE('d', isigned);
                TYPE('u', iunsigned);

                TYPE('x', hex);
                TYPE('X', HEX);

                TYPE('p', pointer);
                TYPE('P', POINTER);

                TYPE('h', dump);
                TYPE('H', DUMP);

                TYPE('c', char);

                TYPE('b', binary);

#ifndef STRZ_NO_FLOAT
                TYPE('f', float);
#endif

                default:
                    break;
            }
        }
        else
            ok = _x_putchar(&p, *(p.on++));
    }

    axva_copy_end(p.list);

    RETURN(p.len);
}
// ***************************************************************************
// FUNCTION
//      strz_format
// PURPOSE
//
// PARAMETERS
//      PFNPUTCHAR pfn_putchar --
//      PVOID      pfn_ptr     --
//      PCSTR      format      --
//                 ...         --
// RESULT
//      INT --
// ***************************************************************************
INT strz_format(PFNPUTCHAR pfn_putchar,
                PVOID      pfn_ptr,
                PCSTR      format,
                ...)
{
    INT             result      = 0;
    va_list         list;
    va_list         listCopy;

    ENTER(true);

    va_start(list, format);
    axva_copy(listCopy, list);
    result = strz_vformat(pfn_putchar, pfn_ptr, format, listCopy);
    axva_copy_end(listCopy);
    va_end(list);

    RETURN(result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _storstr
// PURPOSE
//
// PARAMETERS
//      PSTORSTR stor --
//      CHAR     c    --
// RESULT
//      INT --
// ***************************************************************************
static INT _storstr(PSTORSTR    stor,
                    CHAR        c)
{
    INT         result      = 0;

    if (--stor->left)
    {
        *(stor->on++)  = c;
        result          = 1;
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      strz_vsformat
// PURPOSE
//
// PARAMETERS
//      PSTR    buff   --
//      INT     size   --
//      PCSTR   format --
//      va_list list   --
// RESULT
//      INT --
// ***************************************************************************
INT strz_vsformat(PSTR               buff,
                  INT                size,
                  PCSTR              format,
                  va_list            list)
{
    INT         result      = 0;
    STORSTR     stor;

    ENTER(buff && size && format);

    stor.left   = size;
    stor.on     = buff;

    result = strz_vformat((PFNPUTCHAR)_storstr, &stor, format, list);

    *(buff + result) = 0;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_sformat
// PURPOSE
//
// PARAMETERS
//      PSTR  buff   --
//      INT   size   --
//      PCSTR format --
//            ...    --
// RESULT
//      INT --
// ***************************************************************************
INT strz_sformat(PSTR               buff,
                 INT                size,
                 PCSTR              format,
                 ...)
{
    INT         result      = 0;
    va_list     list;
    va_list     listCopy;

    ENTER(buff && size && format);

    va_start(list, format);
    axva_copy(listCopy, list);
    result = strz_vsformat(buff, size, format, listCopy);
    axva_copy_end(listCopy);
    va_end(list);

    *(buff + result) = 0;

    RETURN(result);
}
