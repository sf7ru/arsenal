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

#ifndef __PORTPIN_H__
#define __PORTPIN_H__

#include <arsenal.h>

// use strz_from_GpioMode for text names
typedef enum
{
    GpioNone              = 0,
    GpioPullDown,
    GpioPullUp,
    GpioOpenDrain,
    GpioRepeater,
    GpioPushPull
} GpioMode;

typedef enum
{
    GpioDirIn             = 0,
    GpioDirOut
} GpioDir;

// Example:
//   #define XS2_01   1, 1
//   PORTPIN led1Pin = PP_PIN(PP_OUT, XS2_01);
//   PORTPIN led2Pin = PP_PIN(PP_OUT_MODE, XS2_01, 0);
#define PP_PIN(a,...)                              a(__VA_ARGS__)

#define PP_PORT_MAX                             0b111
#define PP_PIN_MAX                              0b11111

#define PP_IN(port,pin)                         ((PORTPIN){ GpioDirIn,  1, GpioNone, 0, port, pin })
#define PP_IN_NEG(port,pin)                     ((PORTPIN){ GpioDirIn,  0, GpioNone, 0, port, pin })
#define PP_IN_MODE(port,pin,mode)               ((PORTPIN){ GpioDirIn,  1, mode, 0, port, pin })
#define PP_IN_NEG_MODE(port,pin,mode)           ((PORTPIN){ GpioDirIn,  0, mode, 0, port, pin })
#define PP_OUT(port,pin)                        ((PORTPIN){ GpioDirOut, 1, GpioNone, 0, port, pin })
#define PP_OUT_NEG(port,pin)                    ((PORTPIN){ GpioDirOut, 0, GpioNone, 0, port, pin })
#define PP_OUT_MODE(port,pin,mode)              ((PORTPIN){ GpioDirOut, 1, mode, 0, port, pin })
#define PP_OUT_NEG_MODE(port,pin,mode)          ((PORTPIN){ GpioDirOut, 0, mode, 0, port, pin })
#define SET_PP(port,pin,one,dir,mode,fn)        ((PORTPIN){ dir, one, mode, fn, port, pin })
#define PP_NEXT_PIN(pp, add)                    ((PORTPIN){ pp.dir, pp.one, pp.mode, pp.fn, pp.port, pp.pin + add })

#define PP_INDEX_N(port, pin, sz)               ((port * sz) + pin)
#define PP_INDEX(pinname)                       PP_INDEX_N(pinname, 32)
#define PP_INDEX_PP(pp)                         PP_INDEX_N(pp.port, pp.pin, 32)
#define PP_INDEX_PP_S(pp, sz)                   PP_INDEX_N(pp.port, pp.pin, sz)

#define PP_BIT_PP(a)                            (1UL << PP_INDEX_PP(a))
#define PP_BIT_PP_S(a, sz)                      (1UL << PP_INDEX_PP_S(a, sz))
//    val one out
//    1   0 = 0
//    0   1 = 0
//    0   0 = 1
//    1   1 = 1
//   XNOR
#define PP_IS_ONE(pp,value)                     (((value & 1) ^ pp.one) ? 0 : 1)


#define PP_UNDEF                                { 0b1, 0b1, 0b111, 0b111, PP_PORT_MAX, PP_PIN_MAX }
#define PP_IS_UNDEF(pp)                         ((pp.pin == PP_PIN_MAX) && (pp.port == PP_PORT_MAX))
#define PP_IS_DEFINED(pp)                       (!PP_IS_UNDEF(pp))

typedef struct __tag_PORTPIN
{
    UINT    dir         : 1;
    UINT    one         : 1;
    UINT    mode        : 3;
    UINT    fn          : 3;
    UINT    port        : 3;
    UINT    pin         : 5;
} PORTPIN, * PPORTPIN;

typedef union _tag_PORTPINU
{
    UINT        value;
    PORTPIN     pp;
} PORTPINU, * PPORTPINU;


#endif // #ifndef __PORTPIN_H__

