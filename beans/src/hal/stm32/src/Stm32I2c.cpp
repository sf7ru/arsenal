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

#include <stdio.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------



#ifdef HAL_I2C_MODULE_ENABLED

#include <I2c.h>

#include <I2cDevice.h>

#include <customboard.h>
#include <string.h>
#include <stdint.h>
#include <axutils.h>
#include <HalPlatformDefs.h>


#define recvTo              100
#define sendTo              100

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------



        BOOL                hi2csReady      [ I2C_MAX_IFACES ] = { 0 };
        I2C_HandleTypeDef   hi2cs           [ I2C_MAX_IFACES ] = { 0 };

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      I2c::I2c
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
I2c::I2c()
{
    ifaceNo = -1;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      I2c::~I2c
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
I2c::~I2c()
{

}
extern "C"
{
    void I2C1_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&hi2cs[0]);
    }

    void I2C1_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&hi2cs[0]);
    }

    void I2C2_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&hi2cs[1]);
    }

    void I2C2_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&hi2cs[1]);
    }
}

extern void myprintf(PCSTR msg, ...);

// ***************************************************************************
// FUNCTION
//      I2c::masterRead
// PURPOSE
//
// PARAMETERS
//      UINT  addr    --
//      PVOID buff    --
//      INT   size    --
//      UINT  ifaceNo --
// RESULT
//      INT --
// ***************************************************************************
INT I2c::masterRead(UINT           addr,
                    PVOID          buff,
                    INT            size)
{
    INT                 result          = -1;
    I2C_HandleTypeDef * h;

    ENTER(true);

    if ((h = &hi2cs[ifaceNo]) != nil)
    {
        switch (HAL_I2C_Master_Receive(h, ((uint16_t)addr << 1), (uint8_t*)buff, size, recvTo))
        {
            case HAL_OK:
                //printf("HAL_OK\n");
                result = size;
                break;

            case HAL_TIMEOUT:
            case HAL_BUSY:
                //printf("HAL_BUSY\n");
                result = 0;
                break;

            case HAL_ERROR:
                //printf("HAL_ERROR\n");
                result = HAL_I2C_GetError(h) == HAL_I2C_ERROR_AF ? 0 : -1;
                break;
        }
    }

//    myprintf("I2c::masterRead addr = %d result = %d\n", addr, result);

    RETURN(result);
}

// ***************************************************************************
// FUNCTION
//      I2c::masterWrite
// PURPOSE
//
// PARAMETERS
//      UINT                     addr --
//      PVOID                    buff --
//      INT                      size --
//      UINT           ifaceNo = 0    --
// RESULT
//      INT --
// ***************************************************************************
INT I2c::masterWrite(UINT           addr,
                     PCVOID         buff,
                     INT            size)
{
    INT                 result          = -1;
    I2C_HandleTypeDef * h;

    ENTER(true);

    if ((h = &hi2cs[ifaceNo]) != nil)
    {
        switch (HAL_I2C_Master_Transmit(h, ((uint16_t)addr << 1), (uint8_t*)buff, size, sendTo))
        {
            case HAL_OK:
                result = size;
                break;

            case HAL_TIMEOUT:
            case HAL_BUSY:
                result = 0;
                break;

            case HAL_ERROR:
                result = HAL_I2C_GetError(h) == HAL_I2C_ERROR_AF ? 0 : -1;
                break;
        }
    }

    RETURN(result);
}
/*
INT I2c::masterTransaction(UINT           addr,
                           PCVOID         outBuff,
                           INT            outSize,
                           PVOID          inBuff,
                           INT            inSize)
{
    INT                 result          = -1;
    I2C_HandleTypeDef * h;

    ENTER(true);

    if ((h = &hi2cs[ifaceNo]) != nil)
    {
        if (outBuff && outSize)
        {
            if (HAL_I2C_Master_Transmit(h, ((uint16_t)addr << 1), outBuff, outSize, 500) == HAL_OK)
            {
                result = outSize;
            }
        }


        if (inBuff && inSize)
        {
            if (HAL_I2C_Master_Receive(h, ((uint16_t) addr << 1), inBuff, inSize, 500) == HAL_OK)
            {
                result = inSize;
            }
        }
    }

    RETURN(result);
}
*/
INT I2c::masterTransaction(UINT           addr,
                           PCVOID         outBuff,
                           INT            outSize,
                           PVOID          inBuff,
                           INT            inSize)
{
    INT                 result          = -1;
    I2C_HandleTypeDef * h;

    ENTER(true);

    if ((h = &hi2cs[ifaceNo]) != nil)
    {
        if (outBuff && inBuff)
        {
            if ((UINT)outSize <= sizeof(uint16_t))
            {
                // FIXM : for STM0900 driver (LE) must be set value true, of 16bit registers are turns upside down
                uint16_t outAddr = read_int(outBuff, outSize, true);

                if (HAL_I2C_Mem_Read(h, ((uint16_t) addr << 1), outAddr, outSize, (uint8_t *)inBuff, (uint16_t)inSize, 500) == HAL_OK)
                {
                    result = inSize;
                }
            }
        }
        else if (outBuff)
        {
            result = masterWrite(addr, outBuff, outSize);
        }
        else if (inBuff)
        {
            result = masterRead(addr, inBuff, inSize);
        }
    }

    RETURN(result);
}

void HAL_GPIO_WRITE_ODR(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    // Check the parameters
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    GPIOx->ODR |= GPIO_Pin;
}

void I2c::recover()
{
//    printf("I2C Recover !!!\n");

//    __HAL_RCC_I2C1_FORCE_RESET();
//    HAL_Delay(10);
//    __HAL_RCC_I2C1_RELEASE_RESET();
//

    __HAL_I2C_DISABLE(&hi2cs[ifaceNo]);


    // 13
// FIXME
//    hi2cs[ifaceNo].Instance->CR1 |= I2C_CR1_SWRST;

    HAL_Delay(2);

    // 14
// FIXME
//    hi2cs[ifaceNo].Instance->CR1 ^= I2C_CR1_SWRST;

    __HAL_I2C_ENABLE(&hi2cs[ifaceNo]);


    hi2csReady[ifaceNo] = false;
    init(mSpeed);

/*
//1. Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register.
//2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level
//(Write 1 to GPIOx_ODR).
//3. Check SCL and SDA High level in GPIOx_IDR.
//4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to
//GPIOx_ODR).
//5. Check SDA Low level in GPIOx_IDR.
//6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to
//GPIOx_ODR).
//7. Check SCL Low level in GPIOx_IDR.
//8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to
//GPIOx_ODR).
//9. Check SCL High level in GPIOx_IDR.
//10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to
//GPIOx_ODR).
//11. Check SDA High level in GPIOx_IDR.
//12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
//13. Set SWRST bit in I2Cx_CR1 register.
//14. Clear SWRST bit in I2Cx_CR1 register.
//15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register.

//    void HAL_I2C_ClearBusyFlagErrata_2_14_7(I2C_HandleTypeDef *hi2c) {
//
        static uint8_t resetTried = 0;
        if (resetTried == 1)
        {
            return ;
        }

        uint32_t SDA_PIN = NUCLEO_I2C_EXPBD_SDA_PIN;
        uint32_t SCL_PIN = NUCLEO_I2C_EXPBD_SCL_PIN;
        GPIO_InitTypeDef GPIO_InitStruct;

        // 1
        __HAL_I2C_DISABLE(hi2cs[ifaceNo]);

        // 2
        GPIO_InitStruct.Pin = SDA_PIN|SCL_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_GPIO_WRITE_ODR(GPIOB, SDA_PIN);
        HAL_GPIO_WRITE_ODR(GPIOB, SCL_PIN);

        // 3
        GPIO_PinState pinState;
        if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_RESET) {
            for(;;){}
        }
        if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_RESET) {
            for(;;){}
        }

        // 4
        GPIO_InitStruct.Pin = SDA_PIN;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_GPIO_TogglePin(GPIOB, SDA_PIN);

        // 5
        if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_SET) {
            for(;;){}
        }

        // 6
        GPIO_InitStruct.Pin = SCL_PIN;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_GPIO_TogglePin(GPIOB, SCL_PIN);

        // 7
        if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_SET) {
            for(;;){}
        }

        // 8
        GPIO_InitStruct.Pin = SDA_PIN;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_GPIO_WRITE_ODR(GPIOB, SDA_PIN);

        // 9
        if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_RESET) {
            for(;;){}
        }

        // 10
        GPIO_InitStruct.Pin = SCL_PIN;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_GPIO_WRITE_ODR(GPIOB, SCL_PIN);

        // 11
        if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_RESET) {
            for(;;){}
        }

        // 12
        GPIO_InitStruct.Pin = SDA_PIN|SCL_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Alternate = NUCLEO_I2C_EXPBD_SCL_SDA_AF;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        // 13
        hi2c->Instance->CR1 |= I2C_CR1_SWRST;

        // 14
        hi2c->Instance->CR1 ^= I2C_CR1_SWRST;

        // 15
        __HAL_I2C_ENABLE(hi2c);

        resetTried = 1;
    }
*/
}

#endif
