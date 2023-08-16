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

#include <Modbus.h>
#include <axstring.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


ModbusMaster::ModbusMaster()
{
}
ModbusMaster::~ModbusMaster()
{

}
BOOL ModbusMaster::request(UINT           addr,
                           UINT           func,
                           UINT           reg,
                           UINT           num)
{
#define BUFF_SIZE               (sizeof(MODBUSREQ) + MODBUS_CS_SIZE)

    BOOL        result          = false;
    PMODBUSREQ  req             = (PMODBUSREQ)outBuffData;

    ENTER(true);

    req->addr       = (U8)addr;
    req->func       = (U8)func;
    req->reg        = MAC_BE_U16(reg);
    req->num        = MAC_BE_U16(num);

    addCs(req, sizeof(MODBUSREQ));

    //strz_dump("OUT: ", req, BUFF_SIZE);

    switchToTransmitting();
    result = serial.write(req, BUFF_SIZE, MODBUS_TO);
    switchToReceiving();

    RETURN(result);

#undef  BUFF_SIZE
}
PMODBUSREP ModbusMaster::readResp(UINT         TO)
{
    PMODBUSREP  result          = 0;
    INT        sz              = sizeof(MODBUSREP);
    INT        fsz             = sz + MODBUS_CS_SIZE;

    ENTER(true);

    if ((inBuff.read(TO, MODBUS_INBUF_SZ)) >= fsz)
    {
        result  = (PMODBUSREP)inBuff.getData();

        if (result->size > 1)
        {
            INT diff     = result->size - 1;
            sz          += diff;
            fsz         += diff;
        }

        if (checkCs(inBuff.getData(), sz))
        {
            lastRecvSize = fsz;
        }
        else
        {
            //printf("\nBAD CRC!!\n");
            inBuff.purgeAll();
            result = nil;
        }
    }

    RETURN(result);
}