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



#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_conf.h>
#include <HalPlatformDefs.h>
#include <serial.h>
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx.h"

//#define DISABLE_UART_DMA

// ***************************************************************************
// NOTE
//      Для UART реализован неблокирующий IT-режим, для SmartCard
//      режим блокирующий
// ***************************************************************************

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define UARTDEFS_NUM        (sizeof(globalUarts) / sizeof(UARTDEF))


#ifdef HAL_UART_MODULE_ENABLED

#include <ringbuff.h>


#include <serial.h>
#include <arsenal.h>
#include <stdint.h>

#include <axdata.h>
#include <axtime.h>

#include <customboard.h>
#include <stm32f7xx_hal_conf.h>

#include <FreeRTOS.h>
#include <task.h>


#ifndef CBRD_UART_IN_BUFF_SZ
#define CBRD_UART_IN_BUFF_SZ    256
#endif

//#include <stm32f7xx_ll_lpuart.h>
#include <stm32f7xx_hal_uart.h>
#include <stm32f7xx_hal_smartcard.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


#define MODE_UART               0
#define MODE_SMARTCARD          1

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------


typedef struct tag_UARTDEF
{
    USART_TypeDef *         addr;
    PRINGBUFF               buff;
    UINT                    mode;
    UART_HandleTypeDef      def;
    SMARTCARD_HandleTypeDef scdef;
    DMA_Stream_TypeDef*     dmaStream;
    UINT                    dmaIrqN;
    UINT                    dmaChannel;
    DMA_HandleTypeDef       dmarx;
    PU8                     dmarxbuff;
    UINT                    old_pos;
    TaskHandle_t            xTaskToNotify;
} UARTDEF, * PUARTDEF;

static UARTDEF globalUarts[  ] =
{
    { USART1, nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },

#if (!defined(DISABLE_UART_DMA))
    { USART2, nil, 0,  { 0 }, { 0 }, DMA1_Stream5, DMA1_Stream5_IRQn, DMA_CHANNEL_4, { 0 }, 0, 0, 0 },
#else
     { USART2, nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },
#endif

#ifdef USART3
#if (!defined(DISABLE_UART_DMA))
    { USART3, nil, 0,  { 0 }, { 0 }, DMA1_Stream1, DMA1_Stream1_IRQn, DMA_CHANNEL_4, { 0 }, 0, 0, 0 },
#else
    { USART3, nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },
#endif
#else
    { nil, nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },
#endif

#ifdef UART4
#if (!defined(DISABLE_UART_DMA))
    { UART4,  nil, 0,  { 0 }, { 0 }, DMA1_Stream2, DMA1_Stream2_IRQn, DMA_CHANNEL_4, { 0 }, 0, 0, 0 },
#else
    { UART4,  nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },
#endif
#else
    { nil, nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },
#endif

#ifdef UART5
#if (!defined(DISABLE_UART_DMA))
    { UART5,  nil, 0,  { 0 }, { 0 }, DMA1_Stream0, DMA1_Stream0_IRQn, DMA_CHANNEL_4, { 0 }, 0, 0, 0 },
#else
    { UART5,  nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },
#endif
#else
    { nil, nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },
#endif

    { USART6, nil, 0,  { 0 }, { 0 }, 0, 0, 0, { 0 }, 0, 0, 0 },
};

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


PUARTDEF getUartDefByDma(DMA_HandleTypeDef * dmahf)
{
    PUARTDEF result = nil;

    for (int i = 0; !result && (i < UARTDEFS_NUM); i++)
    {
        if (&globalUarts[i].dmarx == dmahf)
        {
            result = &globalUarts[i];
        }
    }

    return result;
}

PUARTDEF getUartDefByHandle(UART_HandleTypeDef * huart)
{
    PUARTDEF result = nil;

    for (int i = 0; !result && (i < UARTDEFS_NUM); i++)
    {
        if (&globalUarts[i].def.Instance == huart->Instance)
        {
            result = &globalUarts[i];
        }
    }

    return result;
}

void process_dma_data(PUARTDEF def, const void* data, size_t len)
{
    if (def->buff)
    {
        ringbuff_write(def->buff, data, len);
    }
}

BOOL usart_rx_check(PUARTDEF def)
{
    BOOL    result  = false;
    size_t pos;

    /* Calculate current position in buffer */ //
    pos = CBRD_UART_IN_BUFF_SZ - __HAL_DMA_GET_COUNTER(def->def.hdmarx);

    if (pos != def->old_pos)
    {
        if (pos > def->old_pos)
        {
            // Current position is over previous one
            // We are in "linear" mode
            // Process data directly by subtracting "pointers"
            // process_dma_data(&usart_rx_dma_buffer[old_pos], pos - old_pos);
            process_dma_data(def, &def->dmarxbuff[def->old_pos], pos - def->old_pos);
        }
        else
        {
            // We are in "overflow" mode
            // First process data to the end of buffer
            process_dma_data(def, &def->dmarxbuff[def->old_pos], CBRD_UART_IN_BUFF_SZ - def->old_pos);
            /* Check and continue with beginning of buffer */
            if (pos)
            {
                process_dma_data(def, &def->dmarxbuff[0], pos);
            }
        }

        result = true;
    }

    def->old_pos = pos;                              // Save current position as old

    /* Check and manually update if we reached end of buffer */
    if (def->old_pos == CBRD_UART_IN_BUFF_SZ)
    {
        def->old_pos = 0;
    }

    return result;
}

// USART2_RX
void DMA1_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&globalUarts[1].dmarx);
    usart_rx_check(&globalUarts[1]);
}
// USART3_RX
void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&globalUarts[2].dmarx);
    usart_rx_check(&globalUarts[2]);
}
// USART4_RX
void DMA1_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&globalUarts[3].dmarx);
    usart_rx_check(&globalUarts[3]);
}
// USART5_RX
void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&globalUarts[4].dmarx);
    usart_rx_check(&globalUarts[4]);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    PUARTDEF d = getUartDefByHandle(huart);


}

static void MY_USART_IRQHandler(PUARTDEF def)
{
    BaseType_t  xHigherPriorityTaskWoken    = pdFALSE;
    BOOL        doNotify                    = false;
    uint32_t    isrflags;
    uint32_t    errorflags;

        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);

    switch (def->mode)
    {
#ifdef HAL_SMARTCARD_MODULE_ENABLED
        case MODE_SMARTCARD:
            HAL_SMARTCARD_IRQHandler(&def->scdef);
            break;
#endif
        default:

            isrflags = def->addr->ISR;

//            if (SR & USART_SR_TC)
//            {
//                def->addr->SR &= ~USART_SR_TC;
//
//            }

            if (isrflags & USART_ISR_RXNE)
            {
                // Сбрасываем флаг прерывания
                def->addr->ISR &= ~USART_ISR_RXNE;

//                if (def->addr == USART1)
//                {
//                    //__asm__ __volatile__ ("bkpt #0");
//                    tmp =
//                }

                if (def->buff)
                {
                    U8 ch = def->addr->RDR;
                    ringbuff_write(def->buff, &ch, 1);
                    doNotify = true;
                }
            }

            // Check for IDLE line interrupt
            if (LL_USART_IsActiveFlag_IDLE(def->addr))
            {
                LL_USART_ClearFlag_IDLE(def->addr);        // Clear IDLE line flag

                if (LL_USART_IsEnabledIT_IDLE(def->addr))
                {
                    if (def->dmarxbuff)
                    {
                        doNotify = usart_rx_check(def);                       // Check for data to process
                    }
                }
            }

            if (isrflags & USART_ISR_FE)
            {
                __HAL_UART_CLEAR_FEFLAG(&def->def);
            }

            if (isrflags & USART_ISR_PE)
            {
                __HAL_UART_CLEAR_PEFLAG(&def->def);
            }

            if (isrflags & USART_ISR_NE)
            {
                __HAL_UART_CLEAR_NEFLAG(&def->def);
            }
#ifdef USART_ISR_LBD
            // Special case, LIN break detection.
            if (isrflags & USART_ISR_LBD)
            {
                def->addr->ISR &= ~USART_ISR_LBD;
            }
#endif            

            if (isrflags & USART_ISR_ORE)
            {
                __HAL_UART_CLEAR_OREFLAG(&def->def);
            }

            break;
    }

    if (doNotify && def->xTaskToNotify)
    {
        vTaskNotifyGiveFromISR(def->xTaskToNotify, &xHigherPriorityTaskWoken);
        def->xTaskToNotify = nil;
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void USART1_IRQHandler(void)
{
    MY_USART_IRQHandler(&globalUarts[0]);
}

void USART2_IRQHandler(void)
{
    MY_USART_IRQHandler(&globalUarts[1]);
}

void USART3_IRQHandler(void)
{
    MY_USART_IRQHandler(&globalUarts[2]);
}
#ifndef SKIP_USE_UART4
void UART4_IRQHandler(void)
{
    MY_USART_IRQHandler(&globalUarts[3]);
}
#endif
void UART5_IRQHandler(void)
{
    MY_USART_IRQHandler(&globalUarts[4]);
}

void USART6_IRQHandler(void)
{
    MY_USART_IRQHandler(&globalUarts[5]);
}
static BOOL _MspInit(UINT port, SERIALTYPE type, UINT flags)
{
#define PP_OR_OD        (flags & SERIALFLAG_pushpull ? GPIO_MODE_AF_PP : GPIO_MODE_AF_OD)

    BOOL        result = true;

    GPIO_InitTypeDef GPIO_InitStruct;

    switch (port)
    {
        case 0:
            switch (type)
            {
                case SERIALTYPE_primary:
                    // Peripheral clock enable
                    __HAL_RCC_GPIOA_CLK_ENABLE();
                    __HAL_RCC_USART1_CLK_ENABLE();

                    /**USART1 GPIO Configuration
                    PA9     ------> USART1_TX
                    PA10     ------> USART1_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
                    break;

                case SERIALTYPE_secondary:
                    // Peripheral clock enable
                    __HAL_RCC_GPIOB_CLK_ENABLE();
                    __HAL_RCC_USART1_CLK_ENABLE();

                    /**USART1 GPIO Configuration
                    PB6     ------> USART1_TX
                    PB7     ------> USART1_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_6;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
                    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_7;
                    GPIO_InitStruct.Mode = PP_OR_OD;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
                    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
                    break;

                case SERIALTYPE_sc_secondary:
                    __HAL_RCC_GPIOB_CLK_ENABLE();
                    __HAL_RCC_USART1_CLK_ENABLE();
//                    __HAL_RCC_AFIO_CLK_ENABLE();
//                    __HAL_AFIO_REMAP_USART1_ENABLE();

                    /**USART1 GPIO Configuration
                    PB5     ------> USART1_CK
                    PB6     ------> USART1_TX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_5;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
                    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_6;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
                    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
                    break;

                default:
                    result = false;
                    break;
            }

            if (result)
            {
                HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
                HAL_NVIC_EnableIRQ(USART1_IRQn);
            }
            break;

        case 1:
            switch (type)
            {
                case SERIALTYPE_primary:

                    __HAL_RCC_GPIOA_CLK_ENABLE();
                    __HAL_RCC_USART2_CLK_ENABLE();

                    /**USART2 GPIO Configuration
                    PA2     ------> USART2_TX
                    PA3     ------> USART2_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
                    break;

                case SERIALTYPE_sc_secondary:
                    __HAL_RCC_GPIOD_CLK_ENABLE();
                    __HAL_RCC_USART2_CLK_ENABLE();

                    /**USART2 GPIO Configuration
                    PD5     ------> USART2_TX
                    PD7     ------> USART2_CK
                    */
//                    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
//                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//                    GPIO_InitStruct.Pull = GPIO_PULLUP;
//                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//                    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
//                    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_5;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
                    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_7;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
                    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

                    break;

                case SERIALTYPE_secondary:
                    __HAL_RCC_GPIOD_CLK_ENABLE();
                    __HAL_RCC_USART2_CLK_ENABLE();

                    /**USART2 GPIO Configuration
                    PD5     ------> USART2_TX
                    PD6     ------> USART2_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
                    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

                    break;

                default:
                    result = false;
                    break;
            }

            if (result)
            {
                HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(USART2_IRQn);
            }
            break;

        case 2:
#ifdef USART3
            switch (type)
            {
                case SERIALTYPE_primary:

                    __HAL_RCC_GPIOD_CLK_ENABLE();
                    __HAL_RCC_USART3_CLK_ENABLE();

                    /**USART3 GPIO Configuration
                    PD8     ------> USART3_TX
                    PD9     ------> USART3_RX
                    */
                    GPIO_InitStruct.Pin       = GPIO_PIN_8 | GPIO_PIN_9;
                    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull      = GPIO_NOPULL;
                    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
                    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

                    result = true;
                    break;

                default:
                    break;
            }

            if (result)
            {
                HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(USART3_IRQn);
            }
#endif
            break;

        case 3:
#ifdef UART4
            switch (type)
            {
                case SERIALTYPE_primary:

                    __HAL_RCC_GPIOC_CLK_ENABLE();
                    __HAL_RCC_UART4_CLK_ENABLE();

                    /**UART4 GPIO Configuration
                    PC10     ------> UART4_TX
                    PC11     ------> UART4_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
                    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                    result = true;
                    break;

                case SERIALTYPE_secondary:

                    __HAL_RCC_GPIOA_CLK_ENABLE();
                    __HAL_RCC_GPIOC_CLK_ENABLE();
                    __HAL_RCC_UART4_CLK_ENABLE();

                    /**UART4 GPIO Configuration
                    PA0      ------> UART4_TX
                    PA1      ------> UART4_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_0;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_10;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
                    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                    result = true;
                    break;

                default:
                    break;
            }

            if (result)
            {
                HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(UART4_IRQn);
            }
#endif
            break;

        case 4:
#ifdef UART5
            switch (type)
            {
                case SERIALTYPE_primary:

                    __HAL_RCC_GPIOC_CLK_ENABLE();
                    __HAL_RCC_UART5_CLK_ENABLE();

                    /**UART5 GPIO Configuration
                    PC12     ------> UART5_TX
                    PD2     ------> UART5_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_12;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
                    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_2;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
                    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

                    result = true;
                    break;

                default:
                    break;
            }

            if (result)
            {
                HAL_NVIC_SetPriority(UART5_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(UART5_IRQn);
            }
#endif
            break;

        case 5:
            switch (type)
            {
                case SERIALTYPE_primary:
                    // Peripheral clock enable
                    __HAL_RCC_GPIOC_CLK_ENABLE();
                    __HAL_RCC_USART6_CLK_ENABLE();

                    /**USART6 GPIO Configuration
                    PC6     ------> USART6_TX
                    PC7     ------> USART6_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
                    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
                    break;

                case SERIALTYPE_smartcard:
                    // Peripheral clock enable
                    __HAL_RCC_GPIOC_CLK_ENABLE();
                    __HAL_RCC_USART6_CLK_ENABLE();

                    /**USART6 GPIO Configuration
                    PC6     ------> USART6_TX
                    PC8     ------> USART6_CK
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_6;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
                    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_8;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
                    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
                    break;

                default:
                    result = false;
                    break;
            }

            if (result)
            {
                HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(USART6_IRQn);
            }
            break;

        default:
            result = false;
            break;
    }

    return  result;

#undef PP_OR_OD
}
PVOID _serial_close(PVOID def)
{
    PUARTDEF    tmp         = (PUARTDEF)def;

    SAFEDESTROY(ringbuff, tmp->buff);

    return nil;
}
static void __XferCpltCallback(DMA_HandleTypeDef *      dmahf)
{
    usart_rx_check(getUartDefByDma(dmahf));
}

static PUARTDEF setUartDmaDef(PSERIALPROPS       p,
                              PUARTDEF           tmp)
{
    PUARTDEF        result          = nil;

    ENTER(true);

    tmp->mode                            = MODE_UART;
    tmp->def.Instance                    = tmp->addr;
    tmp->def.Init.BaudRate               = SERIALPROPS_UNBAUDRATE(p->u_baudrate);


    switch (p->u_bits + 4)
    {
#ifdef UART_WORDLENGTH_7B
        case 7:     tmp->def.Init.WordLength = UART_WORDLENGTH_7B;  break;
#endif
        case 9:     tmp->def.Init.WordLength = UART_WORDLENGTH_9B;  break;
        default:    tmp->def.Init.WordLength = UART_WORDLENGTH_8B;  break;
    }

    switch (p->u_stop)
    {
#ifdef UART_STOPBITS_1_5
        case SERIALSTOP_one_and_half:   tmp->def.Init.StopBits  = UART_STOPBITS_1_5;    break;
#endif
        case SERIALSTOP_two:            tmp->def.Init.StopBits  = UART_STOPBITS_2;      break;
#ifdef UART_STOPBITS_0_5
            case SERIALSTOP_half:           tmp->def.Init.StopBits  = UART_STOPBITS_0_5;    break;
#endif
        default:                        tmp->def.Init.StopBits  = UART_STOPBITS_1;      break;
    }

    switch (p->b_parity)
    {
        case SERIALPARITY_even:         tmp->def.Init.Parity    = UART_PARITY_EVEN;     break;
        case SERIALPARITY_odd:          tmp->def.Init.Parity    = UART_PARITY_ODD;      break;
        default:                        tmp->def.Init.Parity    = UART_PARITY_NONE;     break;
    }

    tmp->def.Init.Mode                   = UART_MODE_TX_RX;
    tmp->def.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    tmp->def.Init.OverSampling           = UART_OVERSAMPLING_16;
//    tmp->def.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
//    tmp->def.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    __HAL_RCC_DMA1_CLK_ENABLE();

    HAL_NVIC_SetPriority(tmp->dmaIrqN, 5, 0);
    HAL_NVIC_EnableIRQ(tmp->dmaIrqN);

    tmp->dmarx.Instance         = tmp->dmaStream;
    tmp->dmarx.Init.Channel     = tmp->dmaChannel;
    tmp->dmarx.Init.Direction   = DMA_PERIPH_TO_MEMORY;
    tmp->dmarx.Init.PeriphInc   = DMA_PINC_DISABLE;
    tmp->dmarx.Init.MemInc      = DMA_MINC_ENABLE;
    tmp->dmarx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    tmp->dmarx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    tmp->dmarx.Init.Mode        = DMA_CIRCULAR;
    tmp->dmarx.Init.Priority    = DMA_PRIORITY_MEDIUM;
    tmp->dmarx.Init.FIFOMode    = DMA_FIFOMODE_DISABLE;

    if ((tmp->dmarxbuff = (PU8)MALLOC(CBRD_UART_IN_BUFF_SZ)) != nil)
    {
        if (HAL_DMA_Init(&tmp->dmarx) != HAL_OK)
        {
            //_Error_Handler(__FILE__, __LINE__);
        }

        if (HAL_UART_Init(&tmp->def) == HAL_OK)
        {
            result = tmp;

            __HAL_LINKDMA(&tmp->def, hdmarx, tmp->dmarx);

            LL_USART_EnableIT_IDLE(tmp->addr);
            // FOR L4 and F0
//            LL_USART_EnableRxTimeout(tmp->addr);
//            LL_USART_SetRxTimeout(tmp->addr, 115);
//            LL_USART_EnableIT_RTO(tmp->addr);

//            tmp->dmarx.XferHalfCpltCallback = __XferCpltCallback;
//            tmp->dmarx.XferCpltCallback     = __XferCpltCallback;
            HAL_UART_Receive_DMA(&tmp->def, tmp->dmarxbuff, CBRD_UART_IN_BUFF_SZ);
        }
    }

    RETURN(result);
}
static PUARTDEF setUartDef(PSERIALPROPS       p,
                           PUARTDEF           tmp)
{
    PUARTDEF        result          = nil;

    ENTER(true);

    tmp->mode                            = MODE_UART;
    tmp->def.Instance                    = tmp->addr;
    tmp->def.Init.BaudRate               = SERIALPROPS_UNBAUDRATE(p->u_baudrate);


    switch (p->u_bits + 4)
    {
#ifdef UART_WORDLENGTH_7B
        case 7:     tmp->def.Init.WordLength = UART_WORDLENGTH_7B;  break;
#endif
        case 9:     tmp->def.Init.WordLength = UART_WORDLENGTH_9B;  break;
        default:    tmp->def.Init.WordLength = UART_WORDLENGTH_8B;  break;
    }

    switch (p->u_stop)
    {
#ifdef UART_STOPBITS_1_5
        case SERIALSTOP_one_and_half:   tmp->def.Init.StopBits  = UART_STOPBITS_1_5;    break;
#endif
        case SERIALSTOP_two:            tmp->def.Init.StopBits  = UART_STOPBITS_2;      break;
#ifdef UART_STOPBITS_0_5
        case SERIALSTOP_half:           tmp->def.Init.StopBits  = UART_STOPBITS_0_5;    break;
#endif
        default:                        tmp->def.Init.StopBits  = UART_STOPBITS_1;      break;
    }

    switch (p->b_parity)
    {
        case SERIALPARITY_even:         tmp->def.Init.Parity    = UART_PARITY_EVEN;     break;
        case SERIALPARITY_odd:          tmp->def.Init.Parity    = UART_PARITY_ODD;      break;
        default:                        tmp->def.Init.Parity    = UART_PARITY_NONE;     break;
    }

    tmp->def.Init.Mode                   = UART_MODE_TX_RX;
    tmp->def.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    tmp->def.Init.OverSampling           = UART_OVERSAMPLING_16;
//    tmp->def.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
//    tmp->def.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&tmp->def) == HAL_OK)
    {
        result = tmp;

        LL_USART_DisableIT_IDLE(tmp->addr);
//        __HAL_UART_ENABLE_IT(&tmp->def, UART_IT_PE);
        __HAL_UART_ENABLE_IT(&tmp->def, UART_IT_ERR);
        __HAL_UART_ENABLE_IT(&tmp->def, UART_IT_RXNE);
        //__HAL_UART_ENABLE_IT(&tmp->def, UART_IT_TC);
    }

    RETURN(result);
}
static PUARTDEF setSmartCardDef(PSERIALPROPSX   p,
                                PUARTDEF        tmp)
{
    PUARTDEF        result          = nil;

#ifdef HAL_SMARTCARD_MODULE_ENABLED

    ENTER(true);

    tmp->mode                              = MODE_SMARTCARD;
    tmp->scdef.Instance                    = tmp->addr;

    tmp->scdef.Init.BaudRate               = SERIALPROPS_UNBAUDRATE(p->p.u_baudrate) + p->extra;

    //tmp->scdef.Init.BaudRate = 10850;

    //printf("tmp->scdef.Init.BaudRate = %d\n", tmp->scdef.Init.BaudRate);

    switch (p->p.u_bits + 4)
    {
        default:    tmp->scdef.Init.WordLength = SMARTCARD_WORDLENGTH_9B;  break;
    }

    switch (p->p.u_stop)
    {
        case SERIALSTOP_half:           tmp->scdef.Init.StopBits  = SMARTCARD_STOPBITS_0_5; break;
        default:                        tmp->scdef.Init.StopBits  = SMARTCARD_STOPBITS_1_5; break;
    }

    switch (p->p.b_parity)
    {
        case SERIALPARITY_odd:          tmp->scdef.Init.Parity    = SMARTCARD_PARITY_ODD;   break;
        default:                        tmp->scdef.Init.Parity    = SMARTCARD_PARITY_EVEN;  break;
    }

    tmp->scdef.Init.Mode                    = SMARTCARD_MODE_TX_RX;
    tmp->scdef.Init.CLKPolarity             = p->clkPolarity == SERIALCLK_polarity_high  ? SMARTCARD_POLARITY_HIGH : SMARTCARD_POLARITY_LOW;
    tmp->scdef.Init.CLKPhase                = p->clkPhase    == SERIALCLK_phase_2nd_edge ? SMARTCARD_PHASE_2EDGE   : SMARTCARD_PHASE_1EDGE;
    tmp->scdef.Init.CLKLastBit              = SMARTCARD_LASTBIT_ENABLE;
#ifdef SMARTCARD_ONE_BIT_SAMPLE_DISABLE
    tmp->scdef.Init.OneBitSampling          = SMARTCARD_ONE_BIT_SAMPLE_DISABLE;
#endif
    tmp->scdef.Init.Prescaler               = (uint8_t)p->prescaller;
    tmp->scdef.Init.GuardTime               = 16;
#if defined(STM32F707xx)
    tmp->scdef.Init.NACKState               = SMARTCARD_NACK_ENABLE;
#else
    tmp->scdef.Init.NACKEnable              = SMARTCARD_NACK_ENABLE;
    tmp->scdef.Init.TimeOutEnable           = SMARTCARD_TIMEOUT_DISABLE;
    tmp->scdef.Init.BlockLength             = 0;
    tmp->scdef.Init.AutoRetryCount          = 0;
    tmp->scdef.AdvancedInit.AdvFeatureInit  = SMARTCARD_ADVFEATURE_NO_INIT;
#endif

    if (HAL_SMARTCARD_Init(&tmp->scdef) == HAL_OK)
    {
        result = tmp;

//        __HAL_SMARTCARD_DMA_REQUEST_ENABLE(&tmp->scdef, SMARTCARD_DMAREQ_RX);

        __HAL_SMARTCARD_ENABLE_IT(&tmp->scdef, SMARTCARD_IT_RXNE);
    }
#else

#endif

    RETURN(result);
}
PVOID _serial_open(PSERIALPROPS       p,
                   UINT               type,
                   PUINT              irqn)
{
    PVOID       result     = nil;
    PUARTDEF    tmp         = &globalUarts[p->u_port];

    if (p->u_flags & SERIALFLAG_lowpower)
    {
#ifdef LPUART1
        extern PVOID _lpuart_open(PSERIALPROPS,
                                  UINT,
                                  PUINT);

        result = _lpuart_open(p, type, irqn);
#endif
    }
    else
    {
        if (!tmp->buff && ((tmp->buff = ringbuff_create(CBRD_UART_IN_BUFF_SZ)) != nil))
        {
            if (_MspInit(p->u_port, type, p->u_flags))
            {
                switch (type)
                {
                    case SERIALTYPE_smartcard:
                    case SERIALTYPE_sc_secondary:
                        // FIXME remove destroy when IT for smartcard is impletented
                        SAFEDESTROY(ringbuff, tmp->buff);
                        result = setSmartCardDef((PSERIALPROPSX)p, tmp);
                        break;

                    default:
                        if (tmp->dmaStream != nil)
                        {
                            result = setUartDmaDef(p, tmp);
                        }
                        else
                            result = setUartDef(p, tmp);
                        break;
                }
            }
            if (!result)
            {
                _serial_close(tmp);
            }
        }
    }

    return result;
}
INT __serial_write(PVOID              internal,
                   PCVOID             data,
                   INT                size,
                   UINT               TO)
{
    INT         result  = -1;
    INT         rd;
    PUARTDEF    d       = (PUARTDEF)internal;
    char        buff [ 25 ];

    if (d->mode == MODE_SMARTCARD)
    {
#ifdef HAL_SMARTCARD_MODULE_ENABLED
        if (HAL_OK == HAL_SMARTCARD_Transmit(&d->scdef, data, size, TO))
        {
            result = size;
        }
#endif
    }
    else
    {
        if (TO)
        {
            switch (rd = HAL_UART_Transmit(&d->def, (uint8_t *) data, (uint16_t) size, TO))
            {
                case HAL_OK:
                    result = size;
                    break;

                default:
                    //                sprintf(buff, "err = %d", rd);
                    //                HAL_UART_AbortTransmit(&d->def);
                    break;
            }
        }
        else if (d->def.gState == HAL_UART_STATE_READY)
        {
            if (HAL_UART_Transmit_IT(&d->def, (uint8_t *) data, (uint16_t) size) == HAL_OK)
            {
                result = size;
            }
        }
        else
            result = 0;
    }

    return result;
}

INT __serial_read(PVOID              internal,
                  PCVOID             data,
                  INT                size,
                  UINT               TO)
{
    INT         result  = 0;
//    uint8_t *   on      = (uint8_t *)data;
    int         rd;
    U16         rxXferCountWas;
    PUARTDEF    d       = (PUARTDEF)internal;

    if (d->def.Instance->ISR == 0xF8)
    {
        __HAL_UART_ENABLE_IT(&d->def, UART_IT_ERR);
        __HAL_UART_ENABLE_IT(&d->def, UART_IT_RXNE);
    }

//    printf("Stm32l4xxUart.c(522): stage TO = %d\n", TO);

//    if (flag)
//    {
//        HAL_UART_Receive_DMA(&globalUarts[1].def, &usart_rx_dma_buffer, sizeof(usart_rx_dma_buffer));
//        flag = false;
//    }

    if (d->mode == MODE_SMARTCARD)
    {
#ifdef HAL_SMARTCARD_MODULE_ENABLED
        if (HAL_OK == HAL_SMARTCARD_Receive_IT(&d->scdef, data, size))
        {
            rxXferCountWas = d->scdef.RxXferCount;

            while (TO && d->scdef.RxXferCount && (HAL_SMARTCARD_STATE_READY != d->scdef.RxState))
            {
                if (rxXferCountWas == d->scdef.RxXferCount)
                {
                    TO--;
                }
                else
                    rxXferCountWas = d->scdef.RxXferCount;

                axsleep(1);
            }

            result = size - d->scdef.RxXferCount;

            HAL_SMARTCARD_AbortReceive_IT(&d->scdef);
        }
#endif
    }
    else
    {
        if (((result = ringbuff_read(d->buff, data, size)) == 0) && TO)
        {
            d->xTaskToNotify = xTaskGetCurrentTaskHandle();
            ulTaskNotifyTake(pdTRUE, TO);

            result = ringbuff_read(d->buff, data, size);
        }
    }


    return result;
}

//void clearPort0()
//{
//    __HAL_UART_ENABLE_IT(&globalUarts[0].def, UART_IT_ERR);
//    __HAL_UART_ENABLE_IT(&globalUarts[0].def, UART_IT_RXNE);
//
////    HAL_UART_Abort(&globalUarts[0].def);
//}
INT __serial_get_fd(PVOID internal)
{
    PUARTDEF    d       = (PUARTDEF)internal;
    //LPC_USART_T *   reqBase         = REQBASE(dev);

    return (INT)((d->mode == MODE_UART) ? &d->def : &d->scdef) ;
}

#endif