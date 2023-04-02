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

#include <string.h>
#include <axcomm.h>
#include <axlog.h>
#include <string.h>
#include <axstring.h>
#include <axtime.h>
#include <axmotion.h>
#include <stdlib.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// ---------------------------------------------------------------------------

//static PSTR PSTRZ_sent          = "Sent";
static PSTR PSTRZ_unsent        = "Unsent";
static PSTR PSTRZ_dropped       = "Dropped";
static PSTR PSTRZ_expired       = "Expired";
static PSTR PSTRZ_enroute       = "Enroute";
static PSTR PSTRZ_delivered     = "Delivered";
static PSTR PSTRZ_other         = "Other";
//static PSTR PSTRZ_error         = "System error";
//static PSTR PSTRZ_hz1           = "Unknown (ask Egor)";

static struct
{
    PSTR    psz_sign;
    PSTR    psz_short_fmt;
    PSTR    psz_short;
    PSTR    psz_desc;
} ast_detail_level        [] =
{
    {   "M", "EMERG", "EMERG",   "Emergency"      },
    {   "C", "CRIT ", "CRIT",    "Critical"       },
    {   "A", "ALERT", "ALERT",   "Alert"          },
    {   "E", "ERROR", "ERROR",   "Error"          },
    {   "W", "WARN ", "WARN",    "Warning"        },
    {   "N", "NOTE ", "NOTE",    "Notice"         },
    {   "I", "INFO ", "INFO",    "Information"    },
    {   "D", "DEBUG", "DEBUG",   "Debug"          },
    {   "V", "VERB ", "VERB",    "Verbose"        },
    {   "H", "CHAT",  "CHAT",    "Chatter"        },
    {   "B", "BCAST", "BCAST",   "Broadcast"      }
} ;

static PSTR    apsz_monthes             [] =
{
    "Jan",    "Feb",    "Mar",    "Apr",    "May",    "Jun",
    "Jul",    "Aug",    "Sep",    "Oct",    "Nov",    "Dec",
    "N/A"
};

static PSTR    apsz_fmonthes            [] =
{
    "January",  "February", "March",     "April",   "May",      "June",
    "July",     "August",   "September", "October", "November", "December",
    "WRONG"
};

/*
static PSTR    apsz_days_of_week        [] =
{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
    "WRONG"
};

static PSTR    apsz_full_days_of_week   [] =
{
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
    "WRONG"
};
*/

static PCSTR MOTPHASE_names[] =
          {
              "unknown",
              "still",
              "accel",
              "decel",
              "uniform",
              "user",
          };

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      strz_from_AXRDS
// PURPOSE
//
// PARAMETERS
//      AXRDS   v_status --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR strz_from_AXRDS(AXRDS  v_status)
{
#define STATUS(a)       case AXRDS_##a: psz_result = PSTRZ_##a; break;


    PSTR        psz_result      = PSTRZ_other;


    ENTER(true);

    switch (v_status)
    {
        STATUS(unsent);
        STATUS(dropped);
        STATUS(expired);
        STATUS(enroute);
        STATUS(delivered);

        default:
            break;
    }

    RETURN(psz_result);

#undef  STATUS
}
// ***************************************************************************
// FUNCTION
//      a7log_dl_desc
// PURPOSE
//      Retrieve log importance level description
// PARAMETERS
//      AXLOGDL v_level -- Log importance level
// RESULT
//      PSTR  -- Pointer to description string
// ***************************************************************************
PSTR strz_desc_from_AXLOGDL(AXLOGDL v_level)
{
    PSTR        psz_result;

    ENTER(true);

    psz_result = ast_detail_level[v_level > AXLOGDL_bcast ?
                                  AXLOGDL_bcast :
                                  v_level].psz_desc;

    RETURN(psz_result);
}
// ***************************************************************************
// FUNCTION
//      a7log_dl_desc_short
// PURPOSE
//      Retrieve log importance level short description
// PARAMETERS
//      AXLOGDL v_level -- Log importance level
// RESULT
//      PSTR  -- Pointer to description string
// ***************************************************************************
PSTR strz_sign_from_AXLOGDL(AXLOGDL v_level)
{
    PSTR        psz_result;

    ENTER(true);

    psz_result = ast_detail_level[v_level > AXLOGDL_bcast ?
                                  AXLOGDL_bcast :
                                  v_level].psz_sign;

    RETURN(psz_result);
}
// ***************************************************************************
// FUNCTION
//      a7log_dl_desc_short
// PURPOSE
//      Retrieve log importance level short description
// PARAMETERS
//      AXLOGDL v_level -- Log importance level
// RESULT
//      PSTR  -- Pointer to description string
// ***************************************************************************
PSTR strz_from_AXLOGDL(AXLOGDL v_level)
{
    PSTR        psz_result;

    ENTER(true);

      psz_result = ast_detail_level[v_level > AXLOGDL_bcast ?
                                    AXLOGDL_bcast :
                                    v_level].psz_short_fmt;

    RETURN(psz_result);
}
// ***************************************************************************
// FUNCTION
//      strz_to_AXLOGDL
// PURPOSE
//
// PARAMETERS
//      PSTR string --
// RESULT
//      AXLOGDL --
// ***************************************************************************
AXLOGDL strz_to_AXLOGDL(PSTR    string)
{
    AXLOGDL         result        = (AXLOGDL)AXII;
    AXLOGDL         idx;

    ENTER(true);

    for (  idx = 0;
           (result == AXII) && (idx <= AXLOGDL_bcast);
           idx++)
    {
        if ( !strcmp(ast_detail_level[idx].psz_sign,   string) ||
             !strcmp(ast_detail_level[idx].psz_short,  string) ||
             !strcmp(ast_detail_level[idx].psz_desc,   string) )
        {
            result = idx;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_from_date
// PURPOSE
//
// PARAMETERS
//      PSTR    target   --
//      UINT    size     --
//      PCSTR   templ    --
//      PAXDATE pst_date --
// RESULT
//      UINT --
// ***************************************************************************
UINT strz_from_date(PSTR      target,
                    UINT      size,
                    PCSTR     templ,
                    PAXDATE   pst_date)
{
#define MAC_CHOICE(a,b,c) case a: sz = strz_sformat(onT, left, b, pst_date->c); break

    UINT        result      = 0;
    PCSTR       onS;
    PSTR        onT;
    UINT        sz;
    UINT        left;

    ENTER(target && size && templ && pst_date);

    onS     = templ;
    onT     = target;
    left    = size;

    while (*onS && left)
    {
        if (*onS == '%')
        {
            sz = 0;

            switch (*(++onS))
            {
                MAC_CHOICE('Y', "%.4d", year);
                MAC_CHOICE('y', "%.2d", year % 100);
                MAC_CHOICE('M', "%.2d", month);
                MAC_CHOICE('D', "%.2d", day);
                MAC_CHOICE('d', "%.2d", day);
                MAC_CHOICE('h', "%.2d", hour);
                MAC_CHOICE('m', "%.2d", minute);
                MAC_CHOICE('s', "%.2d", second);

                case '%':
                    *onT = '%';
                    sz  = 1;
                    break;

                default:
                    break;
            }

            left -= sz;
            onT  += sz;
            onS++;
        }
        else
        {
            *(onT++) = *(onS++);
            left--;
        }
    }

    if (left)
    {
        result  = size - left;
        *onT    = 0;
    }

    RETURN(result);

#undef  MAC_CHOICE
}
// ***************************************************************************
// FUNCTION
//      strz_from_time
// PURPOSE
//
// PARAMETERS
//      PSTR   target --
//      UINT   size   --
//      PCSTR  templ  --
//      AXTIME time   --
//      BOOL   local  --
// RESULT
//      UINT --
// ***************************************************************************
UINT strz_from_time(PSTR      target,
                    UINT      size,
                    PCSTR     templ,
                    AXTIME    time,
                    BOOL      local)
{
    UINT            result          = 0;
    AXDATE          date;

    ENTER(true);

    if (axdate_from_utime(&date, time))
    {
        result = strz_from_date(target, size, templ, &date);
    }

    RETURN(result);
}
//// ***************************************************************************
//// TYPE
////      static
//// FUNCTION
////      _strz_from_size
//// PURPOSE
////
//// PARAMETERS
////      PSTR   buff       --
////      UINT   len        --
////      U64    dwFileSize --
////      U64    divider    --
////      PCSTR  posfix     --
//// RESULT
////      void --
//// ***************************************************************************
//static void _strz_from_lsize(PSTR    buff,
//                            UINT    len,
//                            U64     size,
//                            U64     divider,
//                            PCSTR   postfix,
//                            PCSTR   postfix2)
//{
//    U64     number;
//    U64     remainder;
//
//    number    = size / divider;
//    remainder = (size * 100 / divider) % 100;
//
//    if (remainder)
//    {
//        strz_sformat(buff, len, "%"LL"u.%.2"LL"u %s%s",
//                     number,
//                     remainder,
//                     postfix,
//                     postfix2 ? postfix2 : "");
//    }
//    else
//    {
//        strz_sformat(buff, len, "%"LL"u %s%s",
//                     number,
//                     postfix,
//                     postfix2 ? postfix2 : "");
//    }
//}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _strz_from_size
// PURPOSE
//
// PARAMETERS
//      PSTR   buff       --
//      UINT   len        --
//      U64    dwFileSize --
//      U64    divider    --
//      PCSTR  posfix     --
// RESULT
//      void --
// ***************************************************************************
static void _strz_from_size(PSTR    buff,
                            UINT    len,
                            U64     size,
                            U64     divider,
                            PCSTR   postfix,
                            PCSTR   postfix2)
{
    UINT  number;
    UINT  remainder;

    number    = (UINT)(size / divider);
    remainder = (UINT)((size * 100 / divider) % 100);

    if (remainder)
    {
        strz_sformat(buff, len, "%u.%.2u %s%s",
                     number,
                     remainder,
                     postfix,
                     postfix2 ? postfix2 : "");
    }
    else
    {
        strz_sformat(buff, len, "%u %s%s",
                     number,
                     postfix,
                     postfix2 ? postfix2 : "");
    }
}
// ***************************************************************************
// FUNCTION
//      strz_from_size
// PURPOSE
//
// PARAMETERS
//      PSTR   buff       --
//      UINT   len        --
//      U64    dwFileSize --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR strz_from_size(PSTR   buff,
                     UINT   len,
                     U64    size,
                     PCSTR  postfix)
{
    if (size < (1 KB))
    {
        _strz_from_size(buff, len, size, 1, "", postfix);
    }
    else
    {
        if (size < (1 MB))
        {
            _strz_from_size(buff, len, size, 1 KB, "K", postfix);
        }
        else
        {
            if (size < (1 GB))
            {
                _strz_from_size(buff, len, size, 1 MB, "M", postfix);
            }
            else
            {
                if (size < (1 TB))
                {
                    _strz_from_size(buff, len, size, 1 GB, "G", postfix);
                }
                else
                    _strz_from_size(buff, len, size, 1 TB, "T", postfix);
            }
        }
    }

  return (PCSTR)buff;
}
// ***************************************************************************
// FUNCTION
//      strz_from_month
// PURPOSE
//
// PARAMETERS
//      UINT month --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR strz_from_month(UINT month)
{
    return apsz_fmonthes[(month && month < 13 ? month - 1 : 12)];
}
// ***************************************************************************
// FUNCTION
//      strz_from_month_short
// PURPOSE
//
// PARAMETERS
//      UINT month --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR strz_from_month_short(UINT month)
{
    return apsz_monthes[(month && (month < 13) ? month - 1 : 12)];
}
PCSTR strz_from_MOTPHASE(MOTPHASE phase)
{
    PCSTR result = "unknown";

    if ((phase >= 0) && (phase < MOTPHASE_COUNT))
    {
        result = MOTPHASE_names[phase];
    }

    return result;
}

typedef struct __tagTIMEUNITNAMES
{
    PCSTR       one;
    PCSTR       many;
    PCSTR       oneAlt;
    PCSTR       manyAlt;
} TIMEUNITNAMES, * PTIMEUNITNAMES;

static TIMEUNITNAMES TIMEUNITNAMES_ENG[] =
{
    { "year",   "year",    nil, nil },
    { "month",  "months",  nil, nil },
    { "day",    "days",    nil, nil },
    { "hour",   "hours",   nil, nil },
    { "minute", "minutes", nil, nil },
    { "second", "seconds", nil, nil },
    { "msec",   "msecs",   nil, nil },
    { "usec",   "usecs",   nil, nil },
    { "nsec",   "msecs",   nil, nil }
};

static TIMEUNITNAMES TIMEUNITNAMES_RUS[] =
 {
     { "год",     "лет",    nil, "года" },
     { "месяц",   "месяцев",  nil, "месяца" },
     { "день",    "дней",    nil, "дня" },
     { "час",     "часов",   nil, "часа" },
     { "минута",  "минут", nil, "миууты" },
     { "секунда", "секунд", nil, "секунды" },
     { "мсек",    "мсек",   nil, "мсек" },
     { "мксек",   "мксек",   nil, "мксек" },
     { "нсек",    "нсек",   nil, "нсек" }
 };

PCSTR strz_timeunit_name_eng(TIMEUNIT           unitId,
                             UINT               number)
{
    PCSTR           result          = nil;

    ENTER(true);

    result = (number % 10) > 1 ? TIMEUNITNAMES_ENG[unitId].many : TIMEUNITNAMES_ENG[unitId].one;

    RETURN(result);
}

PCSTR strz_timeunit_name_rus_UTF8(TIMEUNIT           unitId,
                                  UINT               number)
{
    PCSTR           result          = nil;
    UINT            rem             = (number % 10);

    ENTER(true);

    //if (unitId == TIMEUNIT_year)
    {
        if (rem == 1)
        {
            result = TIMEUNITNAMES_RUS[unitId].one;
        }
        else if ((rem >= 2) && (rem <= 4))
        {
            result = TIMEUNITNAMES_RUS[unitId].manyAlt;
        }
        else
            result = TIMEUNITNAMES_RUS[unitId].many;
    }
//    else
//        result = ((rem == 0) || (rem > 1)) ? TIMEUNITNAMES_RUS[unitId].many : TIMEUNITNAMES_RUS[unitId].one;

    RETURN(result);
}

PCSTR strz_from_estimated_time(PSTR               buff,
                               UINT               len,
                               AXUTIME            time,
                               PFNTIMEUNITNAME    cbUnitName)
{
    PCSTR           result          = nil;
    ldiv_t          div_time;
    UINT            idx;
    PSTR            on              = buff;
    int             rd;

    div_time.rem = time;

    for (idx = 0; !result; idx++)
    {
        switch (idx)
        {
            case TIMEUNIT_year:     div_time = ldiv(div_time.rem, SECS_IN_YEAR);    break;
            case TIMEUNIT_month:    div_time = ldiv(div_time.rem, SECS_IN_MONTH);   break;
            case TIMEUNIT_day:      div_time = ldiv(div_time.rem, SECS_IN_DAY);     break;
            case TIMEUNIT_hour:     div_time = ldiv(div_time.rem, SECS_IN_HOUR);    break;
            case TIMEUNIT_minute:   div_time = ldiv(div_time.rem, SECS_IN_MINUTE);  break;
            case TIMEUNIT_second:   div_time.quot = div_time.rem;                   break;

            default:                div_time.quot = 0; result = (PCSTR)buff;        break;
        }

        if (div_time.quot)
        {
            if ((rd = snprintf(on, len, "%lu %s ",
                               div_time.quot,
                               cbUnitName ?
                                   (*cbUnitName)((TIMEUNIT)idx, (UINT)div_time.quot) :
                                   strz_timeunit_name_eng((TIMEUNIT)idx, (UINT)div_time.quot))) > 0)
            {
                on  += rd;
                len -= rd;
            }
            else
                result = (PCSTR)buff;
        }
    }

   return (PCSTR)buff;
}
