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

#include <stm32l4xx_hal.h>

#ifdef HAL_UART_MODULE_ENABLED

#include <ringbuff.h>


#include <serial.h>
#include <arsenal.h>
#include <stdint.h>

#include <axdata.h>
#include <axtime.h>

#include <customboard.h>
#include <stm32l4xx_hal_conf.h>

#include <stm32l4xx_ll_lpuart.h>

#ifndef CBRD_UART_IN_BUFF_SZ
#define CBRD_UART_IN_BUFF_SZ    128
#endif


typedef struct tag_UARTDEF
{
    USART_TypeDef *                 addr;
    PRINGBUFF                       buff;
    UART_HandleTypeDef              def;
} UARTDEF, * PUARTDEF;

static UARTDEF globalLpUarts[] =
{
   { LPUART1, nil, { 0 } },
};

static void USART_IRQHandler(PUARTDEF def)
{

    if (def->addr->ISR & USART_ISR_RXNE)
    {
        // Сбрасываем флаг прерывания
        def->addr->ISR &= ~USART_ISR_RXNE;

        if (def->buff)
        {
            ringbuff1_put(def->buff, def->addr->RDR);
        }
    }
}

void LPUART1_IRQHandler(void)
{
    USART_IRQHandler(&globalLpUarts[0]);
}

//void LPUART1_IRQHandler(void)
//{
//    CHAR ubReceivedChar;
//
//    /* Check WUF flag value in ISR register */
//    if(LL_LPUART_IsActiveFlag_WKUP(LPUART1) && LL_LPUART_IsEnabledIT_WKUP(LPUART1))
//    {
//        /* Configure LPUART1 transfer interrupts : */
//        /* Disable the UART Wake UP from stop mode Interrupt */
//        LL_LPUART_DisableIT_WKUP(LPUART1);
//
//        /* WUF flag clearing */
//        LL_LPUART_ClearFlag_WKUP(LPUART1);
//
//        /* Read Received character. RXNE flag is cleared by reading of RDR register */
//        ubReceivedChar = LL_LPUART_ReceiveData8(LPUART1);
//
////        /* Check if received value is corresponding to specific one : S or s */
////        if ((ubReceivedChar == 'S') || (ubReceivedChar == 's'))
////        {
////            /* Turn LED2 On : Expected character has been received */
////            LED_On();
////
////            /* End of program : set boolean for main loop exit */
////            ubFinalCharReceived = 1;
////        }
////
////        /* Echo received character on TX */
////        LL_LPUART_TransmitData8(LPUART1, ubReceivedChar);
//    }
//    else
//    {
//        /* Call Error function */
//        //Error_Callback();
//    }
//
//}


static BOOL _MspInit(UINT port, SERIALTYPE type)
{
    BOOL        result = true;

    GPIO_InitTypeDef GPIO_InitStruct;

    switch (port)
    {
        case 0:
            switch (type)
            {
                case SERIALTYPE_primary:
                    // Peripheral clock enable
                    __HAL_RCC_LPUART1_CLK_ENABLE();

                    // LPUART1 GPIO Configuration
                    // PC0     ------> LPUART1_RX
                    // PC1     ------> LPUART1_TX

                    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
                    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                    break;

                case SERIALTYPE_secondary: // LPUART1
                    break;

                default:
                    result = false;
                    break;
            }

            if (result)
            {
                NVIC_SetPriority(LPUART1_IRQn, 0);
                NVIC_EnableIRQ(LPUART1_IRQn);

//                HAL_NVIC_SetPriority(LPUART1_IRQn, 5, 0);
//                HAL_NVIC_EnableIRQ(LPUART1_IRQn);
//                LL_LPUART_SetWKUPType(LPUART1, LL_LPUART_WAKEUP_ON_RXNE);
                LL_LPUART_Enable(LPUART1);

            }
            break;


        default:
            result = false;
            break;
    }

    return  result;
}
/*
PVOID _serial_close(PVOID def)
{
    PUARTDEF    tmp         = (PUARTDEF)def;

    SAFEDESTROY(ringbuff1, tmp->buff);

    return nil;
}
 */

extern PVOID _serial_close(PVOID def);

PVOID _lpuart_open(PSERIALPROPS       p,
                   UINT               type,
                   PUINT              irqn)
{
    PVOID       result     = nil;
    PUARTDEF    tmp         = &globalLpUarts[p->u_port];

    if (!tmp->buff && ((tmp->buff = ringbuff1_create(CBRD_UART_IN_BUFF_SZ)) != nil))
    {
        if (_MspInit(p->u_port, type))
        {
            tmp->def.Instance          = tmp->addr;
            tmp->def.Init.BaudRate     = p->u_baudrate * 100;
            tmp->def.Init.WordLength   = UART_WORDLENGTH_8B;
            tmp->def.Init.StopBits     = UART_STOPBITS_1;
            tmp->def.Init.Parity       = UART_PARITY_NONE;
            tmp->def.Init.Mode         = UART_MODE_TX_RX;
            tmp->def.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
            tmp->def.Init.OverSampling = UART_OVERSAMPLING_16;
            tmp->def.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
            tmp->def.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

            //if (HAL_MultiProcessor_Init(&tmp->def, 0, UART_WAKEUPMETHOD_IDLELINE) == HAL_OK)
            if (HAL_UART_Init(&tmp->def) == HAL_OK)
            //if (HAL_MultiProcessor_Init(&tmp->def, 0, UART_WAKEUPMETHOD_IDLELINE) != HAL_OK)
            {
                result = tmp;

                __HAL_UART_ENABLE_IT(&tmp->def, UART_IT_RXNE);
            }
        }
        else
            _serial_close(tmp);
    }

    return result;
}
/*
INT __serial_write(PVOID              internal,
                   PCVOID             data,
                   INT                size,
                   UINT               TO)
{
    INT         result  = -1;
    PUARTDEF    d       = (PUARTDEF)internal;

    LL_LPUART_TransmitData8(LPUART1, ubReceivedChar);

    if (HAL_UART_Transmit(&d->def, (uint8_t*)data, (uint16_t)size, TO) == HAL_OK)
    {
        result = size;
    }

    return result;
}

INT __serial_read(PVOID              internal,
                  PCVOID             data,
                  INT                size,
                  UINT               TO)
{
    INT         result  = 0;
    uint8_t *   on      = (uint8_t *)data;
    PUARTDEF    d       = (PUARTDEF)internal;

    do
    {
        while ((result < size) && ringbuff1_get(d->buff, on))
        {
            on++;
            result++;
        }

        if (!result)
        {
            axsleep(1);

            if (TO)
                TO--;
        }

    } while (!result && TO);

    return result;
}
*/
#endif
