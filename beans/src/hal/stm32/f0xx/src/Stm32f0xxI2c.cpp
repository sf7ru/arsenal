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

#include <I2c.h>

#include <I2cDevice.h>

#include <customboard.h>

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_i2c.h"


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _get_i2c_handle
// PURPOSE
//
// PARAMETERS
//      UINT iface --
// RESULT
//      I2C_HandleTypeDef --
// ***************************************************************************
static I2C_HandleTypeDef * _get_i2c_handle(UINT iface)
{
    I2C_HandleTypeDef *     result          = nil;

    ENTER(true);

#ifdef CONF_I2C0
extern  I2C_HandleTypeDef hi2c1;
    if (iface == 0)
    {
        result = &hi2c1;
    }
    else
#endif

#ifdef CONF_I2C1
extern  I2C_HandleTypeDef hi2c2;
    if (iface == 1)
    {
        result = &hi2c2;
    }
    else
#endif
    {
        result = nil;
    }

    RETURN(result);
}
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
//    for (int i = 0; i < I2C_MAX_IFACES; i++)
//    {
//        inited[i] = false;
//    }
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
// ***************************************************************************
// FUNCTION
//      I2c::init
// PURPOSE
//
// PARAMETERS
//      I2cDevice *              dev --
//      UINT           ifaceNo = 0   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL I2c::init(I2cDevice *    dev,
               UINT           ifaceNo,
               UINT           speed)
{
    BOOL                result          = false;

     ENTER(true);

//     if (!inited)
     {
         if ((dev->control = _get_i2c_handle(ifaceNo)) != nil)
         {
             result = true;
         }
     }

     RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      I2c::deinit
// PURPOSE
//
// PARAMETERS
//      UINT ifaceNo --
// RESULT
//      void --
// ***************************************************************************
void I2c::deinit(UINT           ifaceNo)
{
    ENTER(true);

    QUIT;
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      i2c_state_handling
// PURPOSE
//
// PARAMETERS
//      I2C_ID_T id --
// RESULT
//      void --
// ***************************************************************************
//static void i2c_state_handling(I2C_ID_T id)
//{
//        if (Chip_I2C_IsMasterActive(id)) {
//                Chip_I2C_MasterStateHandler(id);
//        } else {
//                Chip_I2C_SlaveStateHandler(id);
//        }
//}
extern "C"
{
    void I2Cx_EV_IRQHandler(void)
    {
#ifdef CONF_I2C0
extern  I2C_HandleTypeDef hi2c1;
        HAL_I2C_EV_IRQHandler(&hi2c1);
#endif

#ifdef CONF_I2C1
extern  I2C_HandleTypeDef hi2c2;
        HAL_I2C_EV_IRQHandler(&hi2c2);
#endif
    }

    void I2Cx_ER_IRQHandler(void)
    {
#ifdef CONF_I2C0
extern  I2C_HandleTypeDef hi2c1;
        HAL_I2C_ER_IRQHandler(&hi2c1);
#endif
#ifdef CONF_I2C1
extern  I2C_HandleTypeDef hi2c2;
        HAL_I2C_ER_IRQHandler(&hi2c2);
#endif
    }
}
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
                    INT            size,
                    UINT           ifaceNo)
{
    INT                 result          = -1;
    I2C_HandleTypeDef * h;

    ENTER(true);

    if ((h = _get_i2c_handle(ifaceNo)) != nil)
    {
        switch (HAL_I2C_Master_Receive_IT(h, (uint16_t)addr, (uint8_t*)buff, size))
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
                     INT            size,
                     UINT           ifaceNo)
{
    INT                 result          = -1;
    I2C_HandleTypeDef * h;

    ENTER(true);

    if ((h = _get_i2c_handle(ifaceNo)) != nil)
    {
        switch (HAL_I2C_Master_Transmit_IT(h, (uint16_t)addr, (uint8_t*)buff, size))
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
