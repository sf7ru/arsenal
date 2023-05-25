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

#include <HalPlatformDefs.h>


#if (defined(HAL_CAN_MODULE_ENABLED))

#include <Can.h>
#include <stdint.h>
#include <cmsis_os.h>
#include <string.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct _tag_MYCANDESC
{
    CAN_HandleTypeDef       h;
    CAN_RxHeaderTypeDef     pRxHeader;
    uint8_t                 r;
    CAN_FilterTypeDef       f; //declare CAN filter structure
    BOOL                    ready;
} MYCANDESC, * PMYCANDESC;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static  MYCANDESC           descs           [ CAN_MAX_IFACES ] = { 0 };
static  CAN_TypeDef *       canDefs         [ CAN_MAX_IFACES ] = { CAN1, CAN2 };


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern "C"
{
    void CAN1_TX_IRQHandler(void)
    {
        if (descs[0].ready)
            HAL_CAN_IRQHandler(&descs[0].h);
    }
    void CAN1_RX0_IRQHandler(void)
    {
        if (descs[0].ready)
        {
            HAL_CAN_IRQHandler(&descs[0].h);
            HAL_CAN_GetRxMessage(&descs[0].h, CAN_RX_FIFO0, &descs[0].pRxHeader, &descs[0].r);
        }
    }
}
Can::Can()
{
    constructClass();
}
Can::~Can()
{

}
BOOL Can::init(CANMODE  mode)
{
    BOOL        result          = false;

    ENTER(true);

    mMode = mode;

    if (initBase())
    {
        descs[ifaceNo].h.Instance = canDefs[ifaceNo];

        if (initMsp(&descs[ifaceNo].h))
        {
            descs[ifaceNo].h.Init.Prescaler = HSI_VALUE / (mMode.baudrateKbps * 1000);
            descs[ifaceNo].h.Init.Mode = CAN_MODE_NORMAL;
            descs[ifaceNo].h.Init.SyncJumpWidth = CAN_SJW_1TQ;
            descs[ifaceNo].h.Init.TimeSeg1 = CAN_BS1_1TQ;
            descs[ifaceNo].h.Init.TimeSeg2 = CAN_BS2_1TQ;
            descs[ifaceNo].h.Init.TimeTriggeredMode = DISABLE;
            descs[ifaceNo].h.Init.AutoBusOff = DISABLE;
            descs[ifaceNo].h.Init.AutoWakeUp = DISABLE;
            descs[ifaceNo].h.Init.AutoRetransmission = DISABLE;
            descs[ifaceNo].h.Init.ReceiveFifoLocked = DISABLE;
            descs[ifaceNo].h.Init.TransmitFifoPriority = DISABLE;

            descs[ifaceNo].f.FilterFIFOAssignment=CAN_FILTER_FIFO0; //set fifo assignment
            descs[ifaceNo].f.FilterIdHigh=0x245<<5; //the ID that the filter looks for (switch this for the other microcontroller)
            descs[ifaceNo].f.FilterIdLow=0;
            descs[ifaceNo].f.FilterMaskIdHigh=0;
            descs[ifaceNo].f.FilterMaskIdLow=0;
            descs[ifaceNo].f.FilterScale=CAN_FILTERSCALE_32BIT; //set filter scale
            descs[ifaceNo].f.FilterActivation=ENABLE;
            //descs[ifaceNo].f.FilterMode = CAN_FILTERMODE_IDMASK;

            HAL_CAN_ConfigFilter(&descs[ifaceNo].h, &descs[ifaceNo].f); //configure CAN filter
            HAL_CAN_Start(&descs[ifaceNo].h); //start CAN
            HAL_CAN_ActivateNotification(&descs[ifaceNo].h, CAN_IT_RX_FIFO0_MSG_PENDING); //enable interrupts

            descs[ifaceNo].ready    = true;
            result                  = true;
        }
    }

    RETURN(result);
}
void Can::deinit()
{
    //deinitMsp();
}
#endif // #if (defined(HAL_CAN_MODULE_ENABLED))
