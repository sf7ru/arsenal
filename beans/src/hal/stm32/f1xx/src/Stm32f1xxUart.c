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

#include <stm32f1xx_hal.h>

#include <ringbuff.h>

#ifdef HAL_UART_MODULE_ENABLED

#include <serial.h>
#include <arsenal.h>
#include <stdint.h>

#include <axdata.h>
#include <axtime.h>

#include <customboard.h>

#ifndef CBRD_UART_IN_BUFF_SZ
#define CBRD_UART_IN_BUFF_SZ    128
#endif


typedef struct tag_UARTDEF
{
    USART_TypeDef *         addr;
    PRINGBUFF               buff;
    UART_HandleTypeDef      def;
} UARTDEF, * PUARTDEF;

static UARTDEF globalUarts[] =
{
    { USART1, nil, { 0 } },
    { USART2, nil, { 0 } },
    { USART3, nil, { 0 } },
};

static void USART_IRQHandler(PUARTDEF def)
{

    if (def->addr->SR & USART_SR_RXNE)
    {
        // Сбрасываем флаг прерывания
        def->addr->SR &= ~USART_SR_RXNE;

        if (def->buff)
        {
            ringbuff1_put(def->buff, def->addr->DR);
        }
    }
}

void USART1_IRQHandler(void)
{
    USART_IRQHandler(&globalUarts[0]);
}

void USART2_IRQHandler(void)
{
    USART_IRQHandler(&globalUarts[1]);
}

void USART3_IRQHandler(void)
{
    USART_IRQHandler(&globalUarts[2]);
}

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
                    __HAL_RCC_USART1_CLK_ENABLE();

                    /**USART1 GPIO Configuration
                    PA9     ------> USART1_TX
                    PA10     ------> USART1_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_9;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_10;
                    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
                    break;

                default:
                    result = false;
                    break;
            }

            if (result)
            {
                HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(USART1_IRQn);
            }
            break;

        case 1:
            switch (type)
            {
                case SERIALTYPE_primary:
                    /* Peripheral clock enable */
                    __HAL_RCC_USART2_CLK_ENABLE();

                    /**USART2 GPIO Configuration
                    PA2     ------> USART2_TX
                    PA3     ------> USART2_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_2;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_3;
                    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
            switch (type)
            {
                case SERIALTYPE_primary:
                    // Peripheral clock enable
                    __HAL_RCC_USART3_CLK_ENABLE();

                    /**USART3 GPIO Configuration
                    PB10     ------> USART3_TX
                    PB11     ------> USART3_RX
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_10;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_11;
                    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
                    break;

                default:
                    result = false;
                    break;
            }

            if (result)
            {
                HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(USART3_IRQn);
            }
            break;

        default:
            result = false;
            break;
    }

    return  result;
}
PVOID _serial_close(PVOID def)
{
    PUARTDEF    tmp         = (PUARTDEF)def;

    SAFEDESTROY(ringbuff1, tmp->buff);

    return nil;
}
PVOID _serial_open(PSERIALPROPS       p,
                   UINT               type,
                   PUINT              irqn)
{
    PVOID       result     = nil;
    PUARTDEF    tmp         = &globalUarts[p->u_port];

    if (!tmp->buff && ((tmp->buff = ringbuff1_create(CBRD_UART_IN_BUFF_SZ)) != nil))
    {
        if (_MspInit(p->u_port, type))
        {
            tmp->def.Instance          = tmp->addr;
            tmp->def.Init.BaudRate     = SERIALPROPS_UNBAUDRATE(p->u_baudrate);
             tmp->def.Init.WordLength   = UART_WORDLENGTH_8B;
            tmp->def.Init.StopBits     = UART_STOPBITS_1;
            tmp->def.Init.Parity       = UART_PARITY_NONE;
            tmp->def.Init.Mode         = UART_MODE_TX_RX;
            tmp->def.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
            tmp->def.Init.OverSampling = UART_OVERSAMPLING_16;

            //if (HAL_MultiProcessor_Init(&tmp->def, 0, UART_WAKEUPMETHOD_IDLELINE) == HAL_OK)
            if (HAL_UART_Init(&tmp->def) == HAL_OK)
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
INT __serial_write(PVOID              internal,
                   PCVOID             data,
                   INT                size,
                   UINT               TO)
{
    INT         result  = -1;
    PUARTDEF    d       = (PUARTDEF)internal;

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

INT __serial_get_fd(PVOID internal)
{
    PUARTDEF    d       = (PUARTDEF)internal;
    //LPC_USART_T *   reqBase         = REQBASE(dev);

    //return (INT)((d->mode == MODE_UART) ? &d->def : &d->scdef) ;

        return -1;
}

#endif