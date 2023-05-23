// ***************************************************************************
// TITLE
//      Telematics Private Header
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: telematics.h,v 1.5 2003/09/27 10:07:40 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#ifndef __AXNAVH__
#define __AXNAVH__

#include <arsenal.h>
#include <axtime.h>

#define SATELLITES_NUM      32

// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

//
//             N
//            000
//     NW 340     020 NE
//       290       070
//    W 270         090 E
//       250       110
//     SW 200     160 SE
//            180
//             S
//
#define MAC_COURSE_TO_HEADING(a)(a >= 340 ? "N"  :           \
                                (a >= 290 ? "NW" :           \
                                (a >= 250 ? "W"  :           \
                                (a >= 200 ? "SW" :           \
                                (a >= 160 ? "S"  :           \
                                (a >= 110 ? "SE" :           \
                                (a >=  70 ? "E"  :           \
                                (a >=  20 ? "NE" : "N"))))))))

#define DEF_PI                  3.1415926535897932384626433832795
#define DEF_KNOTSTOKMHFACTOR    1.853       // See http://www.qualimetrics.com/html/reference.html
#define DEF_KMHTOKNOTSFACTOR    0.540       // See http://www.qualimetrics.com/html/reference.html
#define MAC_QBR_TO_DEGREES(a)	(((((double)(a) / 1e8) * 180)) / DEF_PI)
#define MAC_DEGREES_TO_QBR(a)	((INT32)(((((double)(a) * DEF_PI) / 180)) * 1e8))
#define MAC_KMH_TO_MSX(a)       (((a) / 3.6) * 1e2)             // double kmeter/hour to int meter/sec
#define MAC_MSX_TO_KMH(a)       (((double)(a) * 3.6) / 1e2)     // int meter/sec to double kmeter/hour

// Minutes to hundredths
//#define MAC_CVT_MINS2HUNDS(a,b) ((a) += (modf((b), &(a)) / .60))
//#define MAC_CVT_MINS2HUNDS(a,b,c) { b = (modf(c, &a) / .60); a += b; }
#define MAC_CVT_MINS2HUNDS(a,b,c) { b = (modf(c, &a) / .60); b += a; }

// Hundredths to minutes
//#define MAC_CVT_HUNDS2MINS(a,b) ((a) += (modf((b), &(a)) * .60)
#define MAC_CVT_HUNDS2MINS(a,b,c) { b = (modf(c, &a) * .60); a += b; }


#define DEF_GRS80               6378137         // Use GRS 80 spheroid

#define MAC_RAD2DEG(r)          ((r) * 57.2957795131)
#define MAC_DEG2RAD(d)          ((d) / 57.2957795131)

#define DEF_GPS_YEAR            1980
#define DEF_GPS_ROLLOVER        1023
#define DEF_CORR_TO_ZERO        1

// Location type
typedef enum
{
    LOCT_unknown            = 0,
    LOCT_none,
    LOCT_error,
    LOCT_incomplete,
    LOCT_rmc,
    LOCT_date,
    LOCT_sats,
    LOCT_dop,
    LOCT_azimuth
} LOCT;

// Location source
typedef enum
{
    LOCSRC_unknown            = 0,
    LOCSRC_gps,
    LOCSRC_glonass,
    LOCSRC_mixed,
    LOCSRC_gsm,
    LOCSRC_net,
    LOCSRC_galileo,
    LOCSRC_beidou
} LOCSRC;

typedef struct __tag_MTKSENSENCES
{
    UINT    GLL  : 1;
    UINT    RMC  : 1;
    UINT    VTG  : 1;
    UINT    GGA  : 1;
    UINT    GSA  : 1;
    UINT    GSV  : 1;
    UINT    GRS  : 1;
    UINT    GST  : 1;
    UINT    TXT  : 1;
    UINT    ZDA  : 1;
    UINT    rcvd1: 3;
    UINT    MALM : 1;
    UINT    MEPH : 1;
    UINT    MDGP : 1;
    UINT    MDBG : 1;
} MTKSENSENCES;

#define SENSENCES_RMC_ONLY { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }


// ***************************************************************************
// STRUCTURE
//      LOCATION
// PURPOSE
//      Geolocation structure
// ***************************************************************************
typedef struct __tag_LOCATION
{
    BOOL            valid;                  // location validity
    AXDATE          date;                   // Date/time
    double          latitude;               // Latitude
    double          longitude;              // Longitude
    double          altitude;               // Altitude
    float           radius;                 // Radius in meters
    float           speed;                  // Speed in km/h
    float           course;                 // Course in degrees

    float           pdop;                   // Position Dilution of Precision
    float           hdop;                   // Horizontal Dilution of Precision
    float           vdop;                   // Vertical Dilution of Precision
    LOCSRC          source;                 // Source of information (navigation system)
} LOCATION, * PLOCATION;

// ***************************************************************************
// STRUCTURE
//      LOCATION
// PURPOSE
//      Geolocation structure
// ***************************************************************************
typedef struct __tag_SATELLITES
{
    BOOL            b_complete;
    LOCSRC          v_source;
    UINT            u_sats;
    UINT            u_asats;                // Active satellites number

    UINT            u_msgs;
    UINT            u_msg;

    struct
    {
        UINT        u_PRN;
        UINT        u_height;
        UINT        u_asimuth;
        INT         i_SNR;
        BOOL        active;
    } st_sat        [ SATELLITES_NUM ] ;

} SATELLITES, * PSATELLITES;

// ***************************************************************************
// ENUMIRATION
//      ORIAXIS
// PURPOSE
//      Aircraft principal axes
// ***************************************************************************
typedef enum
{
    ORIAXIS_roll     = 0,
    ORIAXIS_pitch,
    ORIAXIS_yaw,

    ORIAXIS_COUNT
} ORIAXIS;


// ***************************************************************************
// ENUMIRATION
//      ORIREFERRAL
// PURPOSE
//      Orientation direction
// ***************************************************************************
typedef enum
{
    ORIREFERRAL_unknown         = 0,
    ORIREFERRAL_tail_down,
    ORIREFERRAL_nose_down,
    ORIREFERRAL_left_down,
    ORIREFERRAL_right_down,
    ORIREFERRAL_downside_down,
    ORIREFERRAL_upside_down
} ORIREFERRAL;

//DETECT_ORIENTATION_TAIL_DOWN,
//DETECT_ORIENTATION_NOSE_DOWN,
//DETECT_ORIENTATION_LEFT,
//DETECT_ORIENTATION_RIGHT,
//DETECT_ORIENTATION_UPSIDE_DOWN,
//DETECT_ORIENTATION_RIGHTSIDE_UP,
//DETECT_ORIENTATION_ERROR


typedef struct _tag_SPATIALVECTOR
{
    double                  x;
    double                  y;
    double                  z;
    double                  w;
} SPATIALVECTOR, * PSPATIALVECTOR;

typedef struct _tag_SPATIALVECTORF
{
    float                   x;
    float                   y;
    float                   z;
    float                   w;
} SPATIALVECTORF, * PSPATIALVECTORF;

typedef struct _tag_SPATIALVECTORI
{
    INT                     x;
    INT                     y;
    INT                     z;
} SPATIALVECTORI, * PSPATIALVECTORI;


// ***************************************************************************
// STRUCTURE
//      ORIENTATION
// PURPOSE
//      Orientation structure
// ***************************************************************************
typedef struct __tag_ORIENTATION
{
    double          roll;   // x - plane rolls to left/to right (крен)
    double          pitch;  // y - plane nose up/down (тангаж)
    double          yaw;    // z - plane turns to left/to right (рысканье)
    double          heading;// magnetic azimuth
} ORIENTATION, * PORIENTATION;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


void                axnavi_loc_reset            (PLOCATION          pst_loc);

void                axnavi_sats_reset           (PSATELLITES        sats,
                                                 UINT               satsNum);

BOOL                nmea_validate               (PSTR               psz_string);

UINT                nmea_add_checksum           (PSTR               psz_string,
                                                 UINT               len);

BOOL                nmea_parse_GPRMC            (PLOCATION          pst_loc,
                                                 PSTR               psz_string,
                                                 UINT               u_len);

BOOL                nmea_parse_GPZDA            (PAXDATE            pst_time,
                                                 PSTR               psz_string,
                                                 UINT               u_len);

BOOL                nmea_parse_GPGSV            (PSATELLITES        pst_sats,
                                                 PSTR               psz_string,
                                                 UINT               u_len);

LOCT                nmea_parse                  (PLOCATION          pst_loc,
                                                 PAXDATE            pst_time,
                                                 PSATELLITES        pst_sats,
                                                 UINT               u_sats_num,
                                                 PSTR               psz_string);

double              range_between_locs          (PLOCATION          loc1,
                                                 PLOCATION          loc2);

// ----------------------- Protocol extension for UBX -----------------------

INT                 pubx_cmd_enable_sentence    (PSTR               buff,
                                                 UINT               size,
                                                 PCSTR              sentence,
                                                 BOOL               enable);

// ----------------------- Protocol extension for MTK -----------------------

INT                 pmtk_cmd_api_set_fix        (PSTR               buff,
                                                 UINT               size,
                                                 UINT               period);

INT                 pmtk_cmd_nmea_update_rate   (PSTR               buff,
                                                 UINT               size,
                                                 UINT               period);

INT                 pmtk_cmd_set_sentences      (PSTR               buff,
                                                 UINT               size,
                                                 MTKSENSENCES       set);

// ---------------------- Protocol extension for Neoway ----------------------

INT                 nwy_cmd_api_set_fix         (PSTR               buff,
                                                 UINT               size,
                                                 UINT               period);

INT                 nwy_cmd_set_sentences       (PSTR               buff,
                                                 UINT               size,
                                                 MTKSENSENCES       set,
                                                 UINT               index);



#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __AXNAVH__
