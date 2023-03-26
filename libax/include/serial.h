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


#ifndef __SERIALH__
#define __SERIALH__

#include <arsenal.h>

#include <axsystem.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define SERIALPARAMSNAME_LEN        8
#define SERIALPARAMSNAME_SZ         (SERIALPARAMSNAME_LEN + 1)

#define SERIALTYPE_rs232            SERIALTYPE_primary

#define SERIALCLK_polarity_low      0
#define SERIALCLK_polarity_high     1
#define SERIALCLK_phase_1st_edge    0
#define SERIALCLK_phase_2nd_edge    1


/*
#define SERIALFL_BITS_MASK      0x001C0000
#define SERIALFL_BITS_OFFSET    18
#define SERIALFL_BITS_4         0x00000000
#define SERIALFL_BITS_5         0x00040000
#define SERIALFL_BITS_6         0x00080000
#define SERIALFL_BITS_7         0x000C0000
#define SERIALFL_BITS_8         0x00100000

#define SERIALFL_PAR_MASK       0x00600000
#define SERIALFL_PAR_NONE       0x00000000
#define SERIALFL_PAR_EVEN       0x00200000
#define SERIALFL_PAR_ODD        0x00400000

#define SERIALFL_STOP_MASK      0x01800000
#define SERIALFL_STOP_1         0x00000000
#define SERIALFL_STOP_15        0x00800000
#define SERIALFL_STOP_2         0x01000000

#define SERIALFL_FCTRL_MASK     0x0F000000
#define SERIALFL_FCTRL_NONE     0x00000000
#define SERIALFL_FCTRL_X        0x02000000
#define SERIALFL_FCTRL_DTRDSR   0x04000000
#define SERIALFL_FCTRL_RTSCTS   0x08000000

#define SERIALFL_MODEM          0x10000000  // Use modem controls
// #define SERIALFL_IRDA           0x20000000  // Port is an Infra-red T/R
// #define SERIALFL_USB            0x40000000  // Port is an USB device
#define SERIALFL_NOINIT         0x80000000  // Do not init connected equipment


#define SERIALFL_RW8N1          (AXDEVFL_READ    | AXDEVFL_WRITE         |   \
                                 AXDEVFL_NBIO    | SERIALFL_FCTRL_NONE   |   \
                                 SERIALFL_BITS_8 | SERIALFL_PAR_NONE     |   \
                                 SERIALFL_STOP_1)

#define SERIALFL_BITS_VALUE(a)  (4 + (((a) & SERIALFL_BITS_MASK) >> SERIALFL_BITS_OFFSET))
*/

// For Linux
#define SERIALFLAG_notty            1

// For STM32
#define SERIALFLAG_lowpower         1
#define SERIALFLAG_pushpull         2

#define SERIALPROPS_BAUDRATE(br)    (br > 1000000 ? br / 1000 | 1 : br / 100)
#define SERIALPROPS_UNBAUDRATE(br)  (br & 1 ? (br & ~1) * 1000 : br * 100)

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// ENUMERATION
//      SERIALTYPE
// PURPOSE
//      Serial device type
// ***************************************************************************
typedef enum
{
    SERIALTYPE_primary      = 0,
    SERIALTYPE_secondary    = 1,
    SERIALTYPE_irda         = 2,
    SERIALTYPE_usb          = 3,
    SERIALTYPE_acm          = 4,
    SERIALTYPE_pts          = 5,
    SERIALTYPE_pipe         = 6,
    SERIALTYPE_file         = 7,
    SERIALTYPE_smartcard    = 8, // SmartCard
    SERIALTYPE_sc_secondary = 9, // SmartCard secondary
} SERIALTYPE;


typedef enum
{
    SERIALPARITY_none       = 0,
    SERIALPARITY_odd        = 1,
    SERIALPARITY_even       = 2,
    SERIALPARITY_forced     = 3,
    SERIALPARITY_forced1    = 4
} SERIALPARITY;

typedef enum {
    SERIALSTOP_one,
    SERIALSTOP_one_and_half,
    SERIALSTOP_two,
    SERIALSTOP_half
} SERIALSTOP;


typedef enum {
    SERIALFLOW_none,
    SERIALFLOW_soft,
    SERIALFLOW_rts,
    SERIALFLOW_cts,
    SERIALFLOW_rtscts
} SERIALFLOW;

typedef enum {
    SERIALBITS_4,
    SERIALBITS_5,
    SERIALBITS_6,
    SERIALBITS_7,
    SERIALBITS_8,
    SERIALBITS_9
} SERIALBITS;


// ***************************************************************************
// STRUCTURE
//      SERIALPROPS
// PURPOSE
//      Serial port properties structure
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) _tag_SERIALPROPS
{
    U32                     u_port      :  5;       // Started from 0 for all platfors, i.e. USART1 is 0
    U32                     u_baudrate  : 14;       // Devided by 100, or devided by 1000 | 1

    U32                     u_flags     :  3;       // Depends to platform

    U32                     u_bits      :  3;       // plus four

    U32                     b_parity    :  2;       // SERIALPARITY
    U32                     u_stop      :  2;       // SERIALSTOP
    U32                     flow        :  3;       // SERIALFLOW
} SERIALPROPS, * PSERIALPROPS;
#pragma pack()

#pragma pack(1)
typedef AXPACKED(struct) _tag_SERIALPROPSX
{
    SERIALPROPS             p;

    U32                     prescaller  :  5;       // prescaller value
    U32                     clkPolarity :  1;       // 0 - Low, 1 - High
    U32                     clkPhase    :  1;       // 0 - first edge, 1 - second edge
    U32                     reserved    : 17;
    U32                     extra       :  8;       // Type dependent value, for example low part of baudrate for smartcards
} SERIALPROPSX, * PSERIALPROPSX;
#pragma pack()


// ***************************************************************************
// STRUCTURE
//      SERIALPROPST
// PURPOSE
//      Big serial port properties structure
// ***************************************************************************
typedef struct _tag_SERIALPROPST
{
    SERIALPROPS             base;
    UINT                    type;
} SERIALPROPST, * PSERIALPROPST;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

// serailparams.c

BOOL                serial_props_parse          (PSERIALPROPS       pst_props,
                                                 PSTR               psz_props,
                                                 PUINT              pu_type);


// $(PALATFORM)/serial.c

INT                 _serial_write               (PAXDEV             dev,
                                                 PCVOID             data,
                                                 INT                size,
                                                 UINT               TO);

INT                 _serial_read                (PAXDEV             dev,
                                                 PVOID              data,
                                                 INT                size,
                                                 UINT               TO);

INT                 _serial_get_pending_tx      (PAXDEV             dev);

PAXDEV              serial_close                (PAXDEV             pst_dev);

PAXDEV              serial_open                 (PSERIALPROPS       pst_props,
                                                 UINT               type);

PAXDEV              serial_open_x               (PSERIALPROPS       pst_props,
                                                 UINT               type,
                                                 PCSTR              name);

BOOL                serial_set_baudrate         (PAXDEV             dev,
                                                 UINT               baudrate);

void                serial_set_dtr              (PAXDEV             dev,
                                                 BOOL               value);

void                serial_set_rts              (PAXDEV             dev,
                                                 BOOL               value);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif  // #ifndef __SERIALH__

