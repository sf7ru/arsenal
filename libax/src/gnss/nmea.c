// ***************************************************************************
// TITLE
//      NMEA 0183 Locations
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <arsenal.h>

#include <axstring.h>
#include <axnavi.h>
//#include <RCtelematics.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define GETFIELD           ((on = strz_substrs_get_u(on, &len, SEP)) != NULL)

#define SATSPERGPGSV        4

#define GSA_SAT_NUM         12

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static  CHAR                SEP     = ',';
static  PSTR                GPRMC   = "$GPRMC";
static  PSTR                GNRMC   = "$GNRMC";
static  PSTR                GPZDA   = "$GPZDA";
static  PSTR                GPGSV   = "$GPGSV";
static  PSTR                GLGSV   = "$GLGSV";
static  PSTR                GNGSV   = "$GNGSV";
static  PSTR                GPGSA   = "$GPGSA";
static  PSTR                GLGSA   = "$GLGSA";
static  PSTR                GNGSA   = "$GNGSA";
static  PSTR                PNVGBLS = "$PNVGBLS";
static  PSTR                hhmmss  = "hhmmss";
static  PSTR                DDMMYY  = "DDMMYY";

static  PSTR                G       = "$G";
static  PSTR                HDT     = "HDT";


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STATIC FUNCTION
//      _isNumber
// PURPOSE
//      Check validity of numeric field
// PARAMETERS
//      PSTR psz_string -- Pointer to string for check
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
static BOOL _isNumber(PSTR  psz_string)
{
    BOOL            b_result    = false;
    PSTR            on;

    ENTER(psz_string);

    on = psz_string;

    while (  *on &&
             ((*on == '.') || (*on == '-') || ((*on > 47) && (*on < 58))))
    {
        on++;
    }

    b_result = !(*on);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      nmea_add_checksum
// PURPOSE
//
// PARAMETERS
//      PSTR psz_string --
//      UINT len        --
// RESULT
//      UINT --
// ***************************************************************************
UINT nmea_add_checksum(PSTR               psz_string,
                       UINT               buffSize)

{
    UINT    result           = 0;
    PSTR    on              = psz_string;

    ENTER(on);

    if (*on == '$')
    {
        on++;
        buffSize--;
    }

    while (*on && (*on != '*') && buffSize--)
    {
        result ^= *(on++);
    }

    if (buffSize > 2)
    {
        *(on++) = '*';
        strz_bin2hex(on, &result, 1);
        *(on + 2) = 0;

        if (result == 0)
        {
            result = 1;
        }
    }
    else
        result = 0;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      nmea_sentence_approve
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL nmea_validate(PSTR psz_string)
{
    BOOL    b_result        = false;
    UINT    u_sum           = 0;
    PSTR    on              = psz_string;

    ENTER(on);

    if (*on == '$')
    {
        on++;
    }

    while (*on && (*on != '*'))
    {
        u_sum ^= *(on++);
    }

    if (*on == '*')
    {
        if (u_sum == (UINT)strtol(on + 1, NULL, 16))
        {
            *on         = 0;
            b_result    = true;
        }
    }

    //printf("nmea_validate stage 10\n");

    RETURN(b_result);
}
// ***************************************************************************
// STATIC FUNCTION
//      _sentence_GPRMC
// PURPOSE
//      Recomended Minimum Specific GPS Data
// PARAMETERS
//      PLA6RSS pst_RSS    -- Pointer to Route Stanshot structure
//      PSTR    psz_string -- Pointer to Location string
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL nmea_parse_GPRMC(PLOCATION       loc,
                      PSTR            psz_string,
                      UINT            u_len)
{
    BOOL            b_result        = false;
    PSTR            on;
    UINT            len;
    double          f_tmp;
    BOOL            valid           = false;

    ENTER(loc && psz_string);

    on          = psz_string;
    len         = u_len;

    axnavi_loc_reset(loc);

    // UTC Time
    if (  GETFIELD                                          &&
          _isNumber(on)                                     &&
          strz_to_date_fmt(&loc->date, on, hhmmss)   )
    {
        // Status
        if (GETFIELD && (('A' == *on) || ('V' == *on)))
        {
            if ('A' == *on)
            {
                valid = true;
            }

            // Latitude
            if (GETFIELD && _isNumber(on))
            {
                //printf("nmea_parse_GPRMC stage lat 1 %f\n", pst_loc->latitude);

                MAC_CVT_MINS2HUNDS(f_tmp, loc->latitude,
                                     atof(on) / 100);

                //printf("nmea_parse_GPRMC stage lat 2 %f\n", pst_loc->latitude);

                // Latitude hemisphere
                if (GETFIELD && (('S' == *on) || ('N' == *on)))
                {
                    if ('S' == *on)
                    {
                        loc->latitude = -loc->latitude;
                    }

                    // Longitude
                    if (GETFIELD && _isNumber(on))
                    {
                        //printf("nmea_parse_GPRMC stage lon 1 %f\n", pst_loc->longitude);

                        MAC_CVT_MINS2HUNDS(f_tmp, loc->longitude,
                                             atof(on) / 100);

                        //printf("nmea_parse_GPRMC stage lon 2 %f\n", pst_loc->longitude);

                        // Longitude hemisphere
                        if (GETFIELD && (('W' == *on) || ('E' == *on)))
                        {
                            if ('W' == *on)
                            {
                                loc->longitude = -loc->longitude;
                            }

                            // Speed
                            if (GETFIELD && _isNumber(on))
                            {
                                loc->speed = (float)
                                        (atof(on) * DEF_KNOTSTOKMHFACTOR);

                                // Course
                                if (GETFIELD && _isNumber(on))
                                {
                                    loc->course = (float)atof(on);

                                    // UTC date of position fix
                                    if (GETFIELD && _isNumber(on))
                                    {
                                        strz_to_date_fmt(&loc->date, on, DDMMYY);
                                    }

                                    loc->valid  = valid;
                                    b_result    = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      nmea_parse_GPGSA
// PURPOSE
//
// PARAMETERS
//      PLOCATION pst_loc    --
//      PSTR      psz_string --
//      UINT      u_len      --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL nmea_parse_GPGSA(PLOCATION       loc,
                      PSATELLITES     pst_sats,
                      UINT            sats_num,
                      PSTR            psz_string,
                      UINT            u_len)
{
    BOOL            b_result        = false;
    PSTR            on;
    UINT            len;
    UINT            nsats           = 0;
    UINT            asats           = 0;
    PSATELLITES     p;
    BOOL            ok;
    int             i;

    ENTER(loc && psz_string);

//    memset(loc, 0, sizeof(LOCATION));
    loc->pdop = 0;
    loc->hdop = 0;
    loc->vdop = 0;

    on  = psz_string;
    len = u_len;

    //            $GPGSA,A,3,06,19,11,03,18,24,09,32,22,14,,,2.02,1.03,1.74

    // Mode: M=Manual, forced to operate in 2D or 3D A=Automatic, 3D/2D
    if (  GETFIELD  )
    {
        // Mode: 1=Fix not available 2=2D 3=3D
        if (GETFIELD && _isNumber(on))
        {
#ifdef _NMEA_FLAG_CLEAN_ACTIVE_SATS
            // Cleaning active
            for (int j = 0; j < sats_num; j++)
            {
                p = &pst_sats[j];

                p->u_asats = 0;

                for (int i = 0; i < p->u_sats; i++)
                {
                    p->st_sat[i].active = false;
                }
            }
#endif // CBRD_NMEA_FLAG_CLEAN_ACTIVE_SATS

            for (nsats = 0; nsats < GSA_SAT_NUM; nsats++)
            {
                if (GETFIELD)
                {
                    if (*on)
                    {
                        UINT prn = (UINT)atoi(on);

                        if ((prn >= 1) && (prn <= 32) && sats_num >= LOCSRC_gps)
                        {
                            p = &pst_sats[LOCSRC_gps - 1];
                        }
                        else if ((prn >= 65) && (prn <= 96) && sats_num >= LOCSRC_glonass)
                        {
                            p = &pst_sats[LOCSRC_glonass - 1];
                        }
                        else
                            p = nil;

                        if (p && p->b_complete)
                        {
                            ok = false;

                            for (i = 0; !ok && (i < p->u_sats); i++)
                            {
                                if (p->st_sat[i].u_PRN == prn)
                                {
                                    p->st_sat[i].active = true;
                                    ok = true;
                                }
                            }

                            if (ok)
                                p->u_asats++;
                        }
                    }
                }
                else
                    break;
            }
            //printf("############ nmea_parse_GPGSA stage asats = %u\n", asats);

            //printf("nmea_parse_GPGSA stage !!!!!!!!!!!!!!!!!!!!!!!!! nsats = %u\n", nsats);

            if (nsats == GSA_SAT_NUM)
            {
                //printf("nmea_parse_GPGSA stage 9\n");

                // PDOP
                if (GETFIELD && _isNumber(on))
                {
                    //printf("nmea_parse_GPGSA stage 10\n");
                    loc->pdop = (float)atof(on);

                    // HDOP
                    if (GETFIELD && _isNumber(on))
                    {
                        //printf("nmea_parse_GPGSA stage 12\n");
                        loc->hdop = (float)atof(on);

                        // VDOP
                        if (GETFIELD && _isNumber(on))
                        {
                            //printf("nmea_parse_GPGSA stage 13\n");

                            loc->vdop           = (float)atof(on);
                            pst_sats->u_asats   = asats;

                            b_result            = true;
                        }
                    }
                }
            }
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// STATIC FUNCTION
//      _sentence_GPZDA
// PURPOSE
//      Recomended Minimum Specific GPS Data
// PARAMETERS
//      PLA6RSS pst_RSS    -- Pointer to Route Stanshot structure
//      PSTR    psz_string -- Pointer to Location string
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL nmea_parse_GPZDA(PAXDATE         pst_time,
                      PSTR            psz_string,
                      UINT            u_len)
{
    BOOL            b_result        = false;
    PSTR            on;
    UINT            len;

    ENTER(pst_time && psz_string);

    on  = psz_string;
    len = u_len;

    memset(pst_time, 0, sizeof(AXDATE));

    // UTC Time
    if (  GETFIELD                                  &&
          _isNumber(on)                             &&
          strz_to_date_fmt(pst_time, on, hhmmss)    )
    {
        // UTC day
        if (GETFIELD && _isNumber(on))
        {
            pst_time->day = (U16)atol(on);

            // UTC Month
            if (GETFIELD && _isNumber(on))
            {
                pst_time->month = (U16)atol(on);

                // UTC Year
                if (GETFIELD && _isNumber(on))
                {
                    pst_time->year = (U16)atol(on);

                    b_result = true;
                }
            }
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _sentence_GPGSV
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSATELLITES   pst_sats   --
//      PSTR       psz_string --
// RESULT
//      BOOL --
// ***************************************************************************
BOOL nmea_parse_GPGSV(PSATELLITES     pst_sats,
                      PSTR            psz_string,
                      UINT            u_len)
{
    BOOL            b_result        = false;
    UINT            u_msgs;
    UINT            u_msg;
    UINT            sat;
    UINT            u_cnt;
    PSTR            on;
    UINT            len;

#define SATPROP(a,b) case a: pst_sats->st_sat[sat].b = atol(on); break

    ENTER(pst_sats && psz_string);

    on                  = psz_string;
    len                 = u_len;

    // Total number of messages
    if (GETFIELD && _isNumber(on))
    {
        u_msgs = atol(on);

        if (!pst_sats->u_msgs || (pst_sats->u_msgs != u_msgs))
        {
            pst_sats->u_msg  = 0;
        }

        // Current message number
        if (GETFIELD && _isNumber(on))
        {
            if (1 == (u_msg = atol(on)))
            {
                memset(&pst_sats->st_sat, 0, sizeof(pst_sats->st_sat));
                pst_sats->u_msg = 0;
            }

            pst_sats->u_msgs = u_msgs;

            if ((pst_sats->u_msg + 1) == u_msg)
            {
                sat             = (pst_sats->u_msg << 2);
                pst_sats->u_msg = u_msg;

                // Sats number
                if (GETFIELD && _isNumber(on))
                {
                    pst_sats->u_sats    = atol(on);

                    for  (  b_result = true, u_cnt = 0;
                            b_result && GETFIELD && _isNumber(on);
                            u_cnt++)
                    {
                        if (u_cnt > 3)
                        {
                            u_cnt = 0;
                            sat++;
                        }

                        switch (u_cnt)
                        {
                            SATPROP(0, u_PRN);
                            SATPROP(1, u_height);
                            SATPROP(2, u_asimuth);
                            SATPROP(3, i_SNR);

                            default:
                                break;
                        }
                    }
                }
            }
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      nmea_parse_PNVGBLS
// PURPOSE
//
// PARAMETERS
//      PLOCATION loc        --
//      PAXDATE   pst_time   --
//      PSTR      psz_string --
//      UINT      u_len      --
// RESULT
//      LOCT --
// ***************************************************************************
LOCT nmea_parse_PNVGBLS(PLOCATION       loc,
                        PAXDATE         pst_time,
                        PSTR            psz_string,
                        UINT            u_len)
{
    LOCT            v_result        = LOCT_unknown;
    PSTR            on;
    UINT            len;

    ENTER(pst_time && psz_string);

    on  = psz_string;
    len = u_len;

    memset(pst_time, 0, sizeof(AXDATE));

    // Время навигационных определений
    if (  GETFIELD                                  &&
          _isNumber(on)                             &&
          strz_to_date_fmt(pst_time, on, hhmmss)    )
    {
        // Проекция базовой линии на широту, м
        if (GETFIELD && _isNumber(on))
        {
            //pst_time->day = (U16)atol(on);

            // Проекция базовой линии на долготу, м
            if (GETFIELD && _isNumber(on))
            {
                //pst_time->month = (U16)atol(on);

                // Проекция базовой линии на высоту, м
                if (GETFIELD && _isNumber(on))
                {
                    //pst_time->year = (U16)atol(on);

                    // Длина базовой линии (Расстояние от Базы до Ровера), м
                    if (GETFIELD && _isNumber(on))
                    {
                        //pst_time->year = (U16)atol(on);

                        // Азимут или Направление базовой линии (угол между вектором
                        // базовой линии и направлением на Север), градусы
                        if (GETFIELD && _isNumber(on))
                        {
                            loc->course = (float)atof(on);

                            // Тангаж, Дифферент (угол между вектором базовой линии
                            // и горизонтом),градусы
                            if (GETFIELD && _isNumber(on))
                            {
                                // Индикатор режима
                                if (GETFIELD)
                                {
                                    switch (*on)
                                    {
//                                        case 'A': // автономный режим
//                                        case 'D': // дифференциальный режим
//                                        case 'F': // RTK Float
                                        case 'R': // RTK Fix
//                                        case 'E': // навигационные данные получены счислением
                                            loc->valid  = true;
                                            v_result    = LOCT_azimuth;
                                            break;

                                        case 'N': // данные не достоверны
                                            loc->valid  = false;
                                            v_result    = LOCT_azimuth;
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    RETURN(v_result);
}
// ***************************************************************************
// FUNCTION
//      nmea_parse_GPHDT
// PURPOSE
//
// PARAMETERS
//      PLOCATION loc        --
//      PSTR      psz_string --
//      UINT      u_len      --
// RESULT
//      LOCT --
// ***************************************************************************
LOCT nmea_parse_GPHDT(PLOCATION       loc,
                      PSTR            psz_string,
                      UINT            u_len)
{
#define MAC_SYS(a,b) case a: loc->source = LOCSRC_##b; break

    LOCT            v_result        = LOCT_unknown;
    PSTR            on;
    UINT            len;

    ENTER(loc && psz_string);

    on  = psz_string;
    len = u_len;

    switch (*(on + 2))
    {
        MAC_SYS('P', gps);
        MAC_SYS('N', mixed);
        MAC_SYS('L', glonass);
        MAC_SYS('A', galileo);

        default:
            loc->source = LOCSRC_unknown;
            break;
    }

    // Курс истинный в градусах
    if (GETFIELD && _isNumber(on))
    {
        loc->course = atof(on);
        loc->valid  = true;
        v_result    = LOCT_azimuth;
    }

    RETURN(v_result);

#undef MAC_SYS
}

/*
// ***************************************************************************
// FUNCTION
//      _sentence_GPGSV
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSATELLITES   pst_sats   --
//      PSTR       psz_string --
// RESULT
//      BOOL --
// ***************************************************************************
BOOL nmea_parse_GPGSV(PSATELLITES     pst_sats,
                      PSTR            psz_string)
{
    BOOL            b_result        = false;
    UINT            u_msg;
    UINT            u_cnt;
    PSTR            on;
    PSTR            psz_zh;
    CHAR            sz_string       [ AXLMEDIUM + 1 ];

    ENTER(pst_sats && psz_string);

    u_cnt       = 0;
    b_result    = true;
    on      = psz_string;

    while (b_result && ((on = la6_substring_get(sz_string,
                            on, AXLMEDIUM, ',')) != NULL))
    {
        if ((psz_zh = strchr(sz_string, '*')) != NULL)
        {
            *psz_zh = 0;
        }

        switch (u_cnt)
        {
// Total messages
            case 0:
                if ( !( _isNumber(sz_string)                     &&
                        ((pst_sats->u_msgs = atol(sz_string)) != 0)    ))
                {
                    b_result = false;
                }
                break;
// Current message
            case 1:
                if (  _isNumber(sz_string)               &&
                      ((u_msg = atol(sz_string)) != 0)      )
                {
                    if (u_msg == 1)
                    {
                        memset(&pst_sats->st_sat, 0, sizeof(pst_sats->st_sat));
                    }
                    else
                    {
                        if ((pst_sats->u_msg + 1) != u_msg)
                        {
                            b_result = false;
                        }
                    }

                    pst_sats->u_msg = u_msg;
                }
                else

                break;

            case 2:
                if ( !( _isNumber(sz_string)                 &&
                        ((pst_sats->u_sats = atol(sz_string)) != 0) ))
                {
                    b_result = false;
                }
                break;

            case 3:
            case 7:
            case 11:
            case 15:
                if (*sz_string && _isNumber(sz_string))
                {
                    pst_sats->st_sat[u_cnt - 3].u_PRN = atol(sz_string);
                }
                break;

            case 4:
            case 8:
            case 12:
            case 16:
                if (*sz_string && _isNumber(sz_string))
                {
                    pst_sats->st_sat[u_cnt - 4].u_height = atol(sz_string);
                }
                break;

            case 5:
            case 9:
            case 13:
            case 17:
                if (*sz_string && _isNumber(sz_string))
                {
                    pst_sats->st_sat[u_cnt - 5].u_asimuth = atol(sz_string);
                }
                break;

            case 6:
            case 10:
            case 14:
            case 18:
                if (*sz_string && _isNumber(sz_string))
                {
                    pst_sats->st_sat[u_cnt - 6].i_SNR = atol(sz_string);
                }
                break;

            default:
                b_result = false;
                break;
        }

        u_cnt++;
    }

    RETURN(b_result);
}
*/
// ***************************************************************************
// FUNCTION
//      nmea_parse
// PURPOSE
//
// PARAMETERS
//      PSTR       psz_string --
//      PSATELLITES   pst_sats   --
// RESULT
//      BOOL  --
// ***************************************************************************
LOCT nmea_parse(PLOCATION          pst_loc,
                PAXDATE            pst_time,
                PSATELLITES        pst_sats,
                UINT               u_sats_num,
                PSTR               psz_string)
{
    LOCT            v_result        = LOCT_unknown;
    PSTR            on;
    UINT            len;
    PSATELLITES     psats           = nil;
//    PSATELLITES     psats2          = nil;
    BOOL            gps             = false;
    BOOL            glo             = false;
    BOOL            mix             = false;

    ENTER(pst_loc && pst_time && pst_sats && psz_string);


//    strcpy(bebe, "$GNGSA,A,1,,,,,,,,,,,,,99.0,99.0,99.0*1E\n");
//    psz_string = bebe;

    //printf("nmea_parse stage 1 '%s'\n", psz_string);
//
    if (nmea_validate(psz_string))
    {
        //printf("nmea_parse stage 2 '%s'\n", psz_string);
//
        on      = psz_string;
        len     = strlen(psz_string);

        // Signature
        if (GETFIELD)
        {
            if (!strcmp(on, PNVGBLS))
            {
                v_result = nmea_parse_PNVGBLS(pst_loc, pst_time, on, len);
            }
            else if (!memcmp(on, G, 2) && !memcmp(on + 3, HDT, 3))
            {
                v_result = nmea_parse_GPHDT(pst_loc, on, len);
            }
            else if (!strcmp(on, GPRMC) || !strcmp(on, GNRMC))
            {
                v_result = nmea_parse_GPRMC(pst_loc, on, len) ?
                           LOCT_rmc : LOCT_error;
            }
            else
            {
                if (!strcmp(on, GPZDA))
                {
                    v_result = nmea_parse_GPZDA(pst_time, on, len) ?
                               LOCT_date : LOCT_error;
                }
                else
                {
                    if (  ((gps = !strcmp(on, GPGSV)) != false) ||
                          ((glo = !strcmp(on, GLGSV)) != false) ||
                          ((mix = !strcmp(on, GNGSV)) != false) )
                    {
                        if (gps)
                        {
                            psats           = &pst_sats[0];
                            psats->v_source = LOCSRC_gps;
                        }
                        else if (glo && (u_sats_num > 1))
                        {
                            psats           = &pst_sats[1];
                            psats->v_source = LOCSRC_glonass;
                        }
                        else if (mix && (u_sats_num > 2))
                        {
                            psats           = &pst_sats[2];
                            psats->v_source = LOCSRC_mixed;
                        }

                        if (psats)
                        {
                            if (nmea_parse_GPGSV(psats, on, len))
                            {
                                if (  psats->u_msgs                     &&
                                      (psats->u_msgs == psats->u_msg)   )
                                {
                                    psats->b_complete   = true;
                                    v_result            = LOCT_sats;
                                }
                                else
                                {
                                    psats->b_complete   = false;
                                    v_result            = LOCT_incomplete;
                                }
                            }
                            else
                                v_result = LOCT_error;
                        }
                        else
                            v_result = LOCT_none;
                    }
                    else
                    {
                        if (  ((gps = !strcmp(on, GPGSA)) != false) ||
                              ((glo = !strcmp(on, GLGSA)) != false) ||
                              ((mix = !strcmp(on, GNGSA)) != false) )
                        {
//                            if (gps)
//                            {
//                                psats           = &pst_sats[0];
//                                psats->v_source = LOCSRC_gps;
//                            }
//                            else if (glo && (u_sats_num > 1))
//                            {
//                                psats           = &pst_sats[1];
//                                psats->v_source = LOCSRC_glonass;
//                            }
//                            else if (mix && (u_sats_num > 2))
//                            {
//                                psats          = &pst_sats[0];
//                                psats->v_source = LOCSRC_gps;
//                            }

                            v_result = nmea_parse_GPGSA(pst_loc, pst_sats, u_sats_num, on, len) ?
                                       LOCT_dop : LOCT_error;
                        }
                    }
                }
            }
        }
    }

    RETURN(v_result);
}
