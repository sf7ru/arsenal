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

#include <string.h>

#include <I2cDevice.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      I2cDevice::registerWrite
// PURPOSE
//
// PARAMETERS
//      U8 reg_addr --
//      U8 data     --
// RESULT
//      INT --
// ***************************************************************************
INT I2cDevice::registerWrite(U8     reg_addr,
                             U8     data)
{
    int result = -1;

    U8 query[] = { reg_addr, data };

    //printf("writing %d bytes to 0x%x... ", 1, reg_addr);

    if ((result = transaction(sizeof(query), query, 0, nil)) > 0)
    {
        //printf("%x", data);
        result--; // minus reg_addr
    }

    //printf(".Done\n");

    return result;
}
// ***************************************************************************
// FUNCTION
//      I2cDevice::registerWrite
// PURPOSE
//
// PARAMETERS
//      U8  reg_addr --
//      PU8 data     --
//      INT size     --
// RESULT
//      INT --
// ***************************************************************************
INT I2cDevice::registerWrite(U8             reg_addr,
                             PVOID          data,
                             INT            size)
{
    int result = -1;
    U8 query[ size + 1 ];

    query[0] = reg_addr;
    memcpy(&query[1], data, size);

    //printf("writing %d bytes to 0x%x... ", size, reg_addr);

    if ((result = transaction(sizeof(query), query, 0, nil)) > 0)
    {
//        for (int i = 0; i < size; i++)
//       {
//            printf("%x ", *(((PU8)data) + i));
//        }


        result--; // minus reg_addr
    }

    //printf(".Done\n");

    return result;
}
// ***************************************************************************
// FUNCTION
//      I2cDevice::registerRead
// PURPOSE
//
// PARAMETERS
//      U8 reg_addr --
// RESULT
//      U8 --
// ***************************************************************************
U8 I2cDevice::registerRead(U8             reg_addr)
{
    U8              result          = 0;

    transaction(1, &reg_addr, 1, &result);

    //printf("reading 1 bytes from 0x%x...%x. Done\n", reg_addr, result);

    return result;
}
// ***************************************************************************
// FUNCTION
//      I2cDevice::registerRead
// PURPOSE
//
// PARAMETERS
//      U8  reg_addr --
//      PU8 data     --
//      INT len      --
// RESULT
//      INT --
// ***************************************************************************
INT I2cDevice::registerRead(U8             reg_addr,
                            PVOID          data,
                            INT            len)
{

    INT result = transaction(1, &reg_addr, len, (PU8)data);

//    printf("@@@@@@@@@@@@@@@@@@@@@@@@ reading %d bytes from 0x%x... result = %d\n", len, reg_addr, result);
//
//    for (int i = 0; i < result; i++)
//    {
//        printf("%x ", *(((PU8)data) + i));
//    }
//
//    printf(". Done\n");

    return result;
}
// ***************************************************************************
// FUNCTION
//      I2cDevice::registerWriteChecked
// PURPOSE
//
// PARAMETERS
//      UINT reg   --
//      U8   value --
//      UINT tries --
// RESULT
//      INT --
// ***************************************************************************
INT I2cDevice::registerWriteChecked(UINT        reg,
                                    U8          value,
                                    UINT        tries)
{
    int             result          = -1;
    U8              rd              = ~value;

//    spi.lockDevice(this);

    for (UINT i = 0; (i < tries) && (rd != value); i--)
    {
        if ((result = registerWrite(reg, value)) > 1)
        {
            rd = registerRead(reg);

            if (rd != value)
            {
                AXTRACE("I2c: err chk wr reg %u\n", reg);
            }
        }
        else
            AXTRACE("I2c: err wr reg %u\n", reg);
    }

//    spi.unlockDevice(this);

    return result;
}
// ***************************************************************************
// FUNCTION
//      I2cDevice::registerModifyChecked
// PURPOSE
//
// PARAMETERS
//      UINT reg       --
//      U8   clearbits --
//      U8   setbits   --
//      UINT tries     --
// RESULT
//      INT --
// ***************************************************************************
INT I2cDevice::registerModifyChecked(UINT           reg,
                                     U8             clearbits,
                                     U8             setbits,
                                     UINT           tries)
{
    int             result          = -1;
//    Spi &           spi             = Spi::getInstance();
    U8              value;
    U8              rd;

    ENTER(true);

//    spi.lockDevice(this);

    value  = registerRead(reg);
    value &= ~clearbits;
    value |= setbits;
    rd     = ~value;

    for (UINT i = 0; (i < tries) && (rd != value); i--)
    {
        if ((result = registerModify(reg, clearbits, setbits)) > 1)
        {
            rd = registerRead(reg);

            if (rd != value)
            {
                AXTRACE("Spi: err chk mod reg %u", reg);
            }
        }
        else
            AXTRACE("Spi: err mod reg %u", reg);
    }

//    spi.unlockDevice(this);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      I2cDevice::registerModify
// PURPOSE
//
// PARAMETERS
//      UINT reg       --
//      U8   clearbits --
//      U8   setbits   --
// RESULT
//      INT --
// ***************************************************************************
INT I2cDevice::registerModify(UINT reg,
                              U8   clearbits,
                              U8   setbits)
{
    U8 val;

    val = registerRead(reg);
    val &= ~clearbits;
    val |= setbits;

    return registerWrite(reg, val);
}
