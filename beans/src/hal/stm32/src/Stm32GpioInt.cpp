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

#include <Hal.h>
#include <HalPlatformDefs.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define GPIOINT_LINES_COUNT 16

// -------------------------------------------  --------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_PGPIOINTLINE
{
    PORTPIN                 pp;
    PFNGPIOINTCB            cb;
    PVOID                   param;
} PGPIOINTLINE, * PPGPIOINTLINE;


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

extern  GPIO_TypeDef *      globalGpioByIndex       [];
static  PGPIOINTLINE        lines                   [ GPIOINT_LINES_COUNT ] = { 0 };


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern "C"
{
    void _HAL_GPIO_EXTI_IRQHandler(int pinNo)
    {
        PPGPIOINTLINE   line    = &lines[pinNo];
        UINT            value;

        if(__HAL_GPIO_EXTI_GET_IT(1 << pinNo) != RESET)
        {
            if (line->cb)
            {
                value = PP_IS_ONE(line->pp, HAL_GPIO_ReadPin(globalGpioByIndex[line->pp.port], 1 << line->pp.pin));

                (*line->cb)(line->param, line->pp.port, line->pp.pin, value ? SETVECTOR_EDGE_RISE : SETVECTOR_EDGE_FALL);
            }

            __HAL_GPIO_EXTI_CLEAR_IT(1 << pinNo);
        }
    }
    void EXTI0_IRQHandler(void)
    {
        _HAL_GPIO_EXTI_IRQHandler(0);
    }

    void EXTI1_IRQHandler(void)
    {
        _HAL_GPIO_EXTI_IRQHandler(1);
    }

    void EXTI2_IRQHandler(void)
    {
        _HAL_GPIO_EXTI_IRQHandler(2);
    }

    void EXTI3_IRQHandler(void)
    {
        _HAL_GPIO_EXTI_IRQHandler(3);
    }

    void EXTI4_IRQHandler(void)
    {
        _HAL_GPIO_EXTI_IRQHandler(4);
    }

    void EXTI9_5_IRQHandler(void)
    {
        _HAL_GPIO_EXTI_IRQHandler(5);
        _HAL_GPIO_EXTI_IRQHandler(6);
        _HAL_GPIO_EXTI_IRQHandler(7);
        _HAL_GPIO_EXTI_IRQHandler(8);
        _HAL_GPIO_EXTI_IRQHandler(9);
    }

    void EXTI15_10_IRQHandler(void)
    {
        _HAL_GPIO_EXTI_IRQHandler(10);
        _HAL_GPIO_EXTI_IRQHandler(11);
        _HAL_GPIO_EXTI_IRQHandler(12);
        _HAL_GPIO_EXTI_IRQHandler(13);
        _HAL_GPIO_EXTI_IRQHandler(14);
        _HAL_GPIO_EXTI_IRQHandler(15);
    }
}

//
//
//extern "C"
//{
//    void EINT3_IRQHandler(void)
//    {
//        int  call = 0;
//
//        for (INT i = 0; i < count; i++)
//        {
//            call            = false;
//            PPGPIOINTLINE p   = &lines[i];
//
//            switch (p->port)
//            {
//                case 0:
////                    if (LPC_GPIOINT->IO0.STATR & p->checkValue)
////                    {
////                        LPC_GPIOINT->IO0.CLR = p->checkValue;
////                        call = SETVECTOR_EDGE_RISE;
////                    }
////                    else if (LPC_GPIOINT->IO0.STATF & p->checkValue)
////                    {
////                        LPC_GPIOINT->IO0.CLR = p->checkValue;
////                        call = SETVECTOR_EDGE_FALL;
////                    }
//                    break;
//
//                case 2:
////                    if (LPC_GPIOINT->IO2.STATR & p->checkValue)
////                    {
////                        LPC_GPIOINT->IO2.CLR = p->checkValue;
////                        call = SETVECTOR_EDGE_RISE;
////                    }
////                    else if (LPC_GPIOINT->IO2.STATF & p->checkValue)
////                    {
////                        LPC_GPIOINT->IO2.CLR = p->checkValue;
////                        call = SETVECTOR_EDGE_FALL;
////                    }
//                    break;
//
//                default:
//                    break;
//            }
//
//            if (call)
//            {
//                (*p->cb)(p->param, p->port, p->pin, call);
//            }
//        }
//    }
//}
//static PPGPIOINTLINE findUnset()
//{
//    PPGPIOINTLINE result = nil;
//
//    for (int i = 0; !result && (i < count); i++)
//    {
//        if ((UINT)-1 == lines[i].port)
//        {
//            result = &lines[i];
//        }
//    }
//
//    return result;
//}
HAXHANDLE Hal::GpioInt::setVector(PORTPIN        pp,
                                  PFNGPIOINTCB   cb,
                                  PVOID          param,
                                  UINT           edges)
{
    PPGPIOINTLINE       result          = nil;

    ENTER(true);

    if (pp.pin < GPIOINT_LINES_COUNT)
    {
        if (nil == lines[pp.pin].cb)
        {
            result = &lines[pp.pin];

            result->pp      = pp;
            result->param   = param;

            Hal::getInstance().gpio.setMode(pp, edges << 1);

            result->cb      = cb;
        }
    }

    RETURN((HAXHANDLE)result);
}
HAXHANDLE Hal::GpioInt::unsetVector(HAXHANDLE      handler)
{
    PPGPIOINTLINE         c;

    if (handler)
    {
        c       = (PPGPIOINTLINE)handler;
        c->cb   = nil;
    }

    return nil;
}