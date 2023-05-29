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
#include <string.h>
#include <cmsis_os.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct _tag_MYCANDESC
{
    CAN_HandleTypeDef       h;
    CAN_FilterTypeDef       f; //declare CAN filter structure
    CAN_RxHeaderTypeDef     rxHeader;
    U8                      rxData     [ 8 ];
    CAN_RxHeaderTypeDef     rxHeaderTmp;
    U8                      rxDataTmp  [ 8 ];
volatile BOOL               received;
volatile BOOL               ready;
//    UINT                    lost;
    TaskHandle_t            xTaskToNotify;
} MYCANDESC, * PMYCANDESC;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static  MYCANDESC           descs           [ CAN_MAX_IFACES ] = { 0 };
static  CAN_TypeDef *       canDefs         [ CAN_MAX_IFACES ] = { CAN1, CAN2 };


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

static void CAN1_RXN_IRQHandler(int index)
{
    BaseType_t      xHigherPriorityTaskWoken    = pdFALSE;
    TaskHandle_t    x;

    if (descs[index].ready)
    {
        HAL_CAN_IRQHandler(&descs[index].h);

        HAL_CAN_GetRxMessage(&descs[index].h, CAN_RX_FIFO0, &descs[index].rxHeaderTmp, descs[index].rxDataTmp);

        if (!descs[index].received)
        {
            memcpy(&descs[index].rxHeader, &descs[index].rxHeaderTmp, sizeof(descs[index].rxHeader));
            memcpy(&descs[index].rxData[0], &descs[index].rxDataTmp[0], sizeof(descs[index].rxData));
            descs[index].received = true;
        }
        
        // notify anyway
        if ((x = descs[index].xTaskToNotify) != nil)
        {
            vTaskNotifyGiveFromISR(x, &xHigherPriorityTaskWoken);
            descs[index].xTaskToNotify = nil;
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
extern "C"
{
    void CAN1_TX_IRQHandler(void)
    {
        if (descs[0].ready)
            HAL_CAN_IRQHandler(&descs[0].h);
    }
    void CAN1_RX0_IRQHandler(void)
    {
        CAN1_RXN_IRQHandler(0);
    }
    void CAN1_RX1_IRQHandler(void)
    {
        CAN1_RXN_IRQHandler(1);
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

    if (!descs[ifaceNo].ready)
    {
        mMode = mode;

        if (initBase())
        {
            descs[ifaceNo].h.Instance = canDefs[ifaceNo];

            if (initMsp(&descs[ifaceNo].h))
            {
                descs[ifaceNo].h.Init.Prescaler = 54; // WRONG - HSI_VALUE / (mMode.baudrateKbps * 1000);
                descs[ifaceNo].h.Init.Mode = CAN_MODE_NORMAL;
                descs[ifaceNo].h.Init.SyncJumpWidth = CAN_SJW_1TQ;
                descs[ifaceNo].h.Init.TimeSeg1 = CAN_BS1_6TQ;
                descs[ifaceNo].h.Init.TimeSeg2 = CAN_BS2_1TQ;
                descs[ifaceNo].h.Init.TimeTriggeredMode = DISABLE;
                descs[ifaceNo].h.Init.AutoBusOff = ENABLE;
                descs[ifaceNo].h.Init.AutoWakeUp = DISABLE;
                descs[ifaceNo].h.Init.AutoRetransmission = ENABLE;
                descs[ifaceNo].h.Init.ReceiveFifoLocked = DISABLE;
                descs[ifaceNo].h.Init.TransmitFifoPriority = ENABLE;

                if (HAL_CAN_Init(&descs[ifaceNo].h) == HAL_OK)
                {
                    setFilterMask(0, 0);

                    HAL_CAN_Start(&descs[ifaceNo].h);
                    HAL_CAN_ActivateNotification(&descs[ifaceNo].h, CAN_IT_RX_FIFO0_MSG_PENDING);

                    descs[ifaceNo].ready    = true;
                    result                  = true;
                }
            }
        }
    }
    
    RETURN(result);
}
BOOL Can::setFilterMask(U32     id,
                        U32     mask)
{
    BOOL            result = false;
    
    ENTER(true);
    
    switch (ifaceNo)
    {
        case 0:
            descs[ifaceNo].f.FilterBank             = 0;

            descs[ifaceNo].f.FilterFIFOAssignment   = CAN_FILTER_FIFO0;
            descs[ifaceNo].f.FilterBank             = 0;
            descs[ifaceNo].f.FilterScale            = CAN_FILTERSCALE_32BIT;
            descs[ifaceNo].f.FilterActivation       = ENABLE;
            descs[ifaceNo].f.FilterMode             = CAN_FILTERMODE_IDMASK;
            descs[ifaceNo].f.FilterScale            = CAN_FILTERSCALE_32BIT;
            descs[ifaceNo].f.FilterIdHigh           = id << 5;
            descs[ifaceNo].f.FilterIdLow            = 0x0000;
            descs[ifaceNo].f.FilterMaskIdHigh       = mask << 5;
            descs[ifaceNo].f.FilterMaskIdLow        = 0x0000;
            descs[ifaceNo].f.FilterFIFOAssignment   = CAN_RX_FIFO0;
            descs[ifaceNo].f.FilterActivation       = ENABLE;
            result                                  = true;
            break;
        
        default:
            break;
    }

    if (result && (HAL_CAN_ConfigFilter(&descs[ifaceNo].h, &descs[ifaceNo].f) != HAL_OK))
    {
        result = false;
    }
        
    RETURN(result);
}
void Can::deinit()
{
    //deinitMsp();
}
INT Can::receive(PCANMESSAGE    msg,
                 UINT           TO)
{
    INT         result          = 0;

    if (!descs[ifaceNo].received && TO)
    {
        descs[ifaceNo].xTaskToNotify = xTaskGetCurrentTaskHandle();
        ulTaskNotifyTake(pdTRUE, TO);
    }

    if (descs[ifaceNo].received)
    {
        CAN_RxHeaderTypeDef *    h = &descs[ifaceNo].rxHeader;

        msg->id     = (h->IDE == CAN_ID_EXT) ? h->ExtId : h->StdId;
        msg->dlc    = MAC_MAX(h->DLC, 8);
  
        memcpy(msg->data, &descs[ifaceNo].rxData[0], msg->dlc);

        result                  = msg->dlc;
        descs[ifaceNo].received = false;
    }

    return result;
}
INT Can::send(PCANMESSAGE    msg,
              UINT           TO)
{
    INT         result          = -1;

    return result;
}
#endif // #if (defined(HAL_CAN_MODULE_ENABLED))
