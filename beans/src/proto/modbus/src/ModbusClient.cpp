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
#include <cstring>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


ModbusClient::ModbusClient()
{
}
ModbusClient::~ModbusClient()
{
}
PMODBUSREQ ModbusClient::readReq(UINT           TO)
{
    PMODBUSREQ  result          = 0;
    INT         sz              = sizeof(MODBUSREQ);
    INT         fsz             = sz + MODBUS_CS_SIZE;

    ENTER(true);

    if ((inBuff.read(TO, fsz)) >= fsz)
    {
        result       = (PMODBUSREQ)inBuff.getData();

        if (checkCs(result, sz))
        {
            lastRecvSize = fsz;

            //strz_dump_w_txt("GOT: ", result, fsz);

            if (myAddr != result->addr)
            {
                freeReq();

                result = nil;
            }
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
BOOL ModbusClient::report(UINT          fn,
                          PVOID          data,
                          UINT           size)
{
    PMODBUSREP  rep             = (PMODBUSREP)outBuffData;
    BOOL        result          = false;
    UINT        payloadSize     = sizeof(MODBUSREP) - 1 + size;

    ENTER(true);

    rep->addr = (U8)myAddr;
    rep->func = (U8)fn;
    rep->size = (U8)size;
    memcpy(&rep->data[0], data, size);

    addCs(rep, payloadSize);

    switchToTransmitting();
    serial.write(rep, payloadSize + MODBUS_CS_SIZE, MODBUS_TO);
    switchToReceiving();

    RETURN(result);
}
//BOOL ModbusClient::request(UINT           addr,
//                           UINT           func,
//                           UINT           reg,
//                           UINT           num)
//{
//#define BUFF_SIZE               (sizeof(MODBUSREQ) + MODBUS_CS_SIZE)
//
//    BOOL        result          = false;
//    U8          buff            [ BUFF_SIZE ] ;
//    PMODBUSREQ  req             = (PMODBUSREQ)buff;
//
//    ENTER(true);
//
//    req->addr   = (U8)addr;
//    req->func   = (U8)func;
//    req->reg    = MAC_BE_U16(reg);
//    req->num    = MAC_BE_U16(num);
//
//    addCs(req, sizeof(MODBUSREQ));
//
////    strz_dump("OUT: ", req, BUFF_SIZE);
//
//    hal->gpio.setPin(pinReDe, 1);
////    axsleep(1);
//    result = serial.write(req, BUFF_SIZE, MODBUS_TO);
//    axsleep(1);
//    hal->gpio.setPin(pinReDe, 0);
//
//    RETURN(result);
//
//#undef  BUFF_SIZE
//}
//PMODBUSREP ModbusClient::readResp(UINT         TO)
//{
//    PMODBUSREP  result          = 0;
//    INT         rd;
//
//    ENTER(true);
//
//    if ((rd = read(TO)) > 0)
//    {
//        result = (PMODBUSREP)inBuff.getData();
//
//        if ((rd - sizeof(MODBUSREP) - MODBUS_CS_SIZE + 1) != result->size)
//        {
//            printf("readResp: real size is different");
//            result = nil;
//
//            freeLastRecvd();
//        }
//    }
//
//    RETURN(result);
//}