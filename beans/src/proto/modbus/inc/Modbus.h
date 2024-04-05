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

#ifndef __MODBUS_H__
#define __MODBUS_H__

#include <arsenal.h>

#include <AXSerial.h>
#include <AXBuffer.h>
#include <Hal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define MODBUS_CS_SIZE      sizeof(U16)
#define MODBUS_TO           1000

#define MODBUS_FN_RD_DO     0x01    // Read digital output / Read Coil Status
#define MODBUS_FN_RD_DI     0x02    // Read digital input
#define MODBUS_FN_RD_AO     0x03    // Read analog output / Read Holding Registers
#define MODBUS_FN_RD_AI     0x04    // Read analog input

#define MODBUS_FN_WR_DO     0x05    // Write digatal output / Force Single Coil
#define MODBUS_FN_WR_AO     0x06    // Write analog output / Read Holding Registers
#define MODBUS_FN_WR_MD     0x0F    // Write multiple coils
#define MODBUS_FN_WR_MA     0x10    // Write multiple holding registers

#define MODBUS_INBUF_SZ     128
#define MODBUS_OUTBUF_SZ    128

#define MODBUS_SC_OFF       0x0000
#define MODBUS_SC_ON        0xFF00

#define MODBUS_SC_IS_ON(a)  (0xFF00 == MAC_BE_U16(a))


#define MODBUSDEF_IO_PORT_SERIAL        31
#define MODBUSDEF_IO_PIN_DTR            1
#define MODBUSDEF_IO_PIN_RTS            2

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef AXPACKED(struct) __tag_MODBUSREQ
{
    U8                      addr;       // Address of device
    U8                      func;       // Function number
    U16                     reg;        // Register address
    U16                     num;        // number of registers requested
} MODBUSREQ, * PMODBUSREQ;

typedef AXPACKED(struct) __tag_MODBUSREP
{
    U8                      addr;
    U8                      func;
    U8                      size;
    U8                      data [ 1 ];
} MODBUSREP, * PMODBUSREP;

typedef AXPACKED(struct) __tag_MODBUSRESP
{
    U8                      addr;
    U8                      func;
    U16                     reg;
    U8                      data [ 1 ];
} MODBUSRESP, * PMODBUSRESP;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Modbus
{
protected:

        AXSerial        serial;
        AXBuffer        inBuff;
        Hal *           hal;
        PORTPIN         pinReDe;

        U8              myAddr;

        U8              inBuffData              [ MODBUS_INBUF_SZ ];

        U8              outBuffData             [ MODBUS_OUTBUF_SZ ];


        INT             lastRecvSize;

        void            addCs                   (PVOID          data,
                                                 UINT           size);

        BOOL            checkCs                 (PCVOID         data,
                                                 UINT           size);

        void            switchToTransmitting    ();

        void            switchToReceiving       ();

        void            freeLastRecvd           ()
        { if (lastRecvSize) { inBuff.purge(lastRecvSize); lastRecvSize = 0; } }

public:
                        Modbus                  ();

virtual                 ~Modbus                 ();

        BOOL            init                    (U8             myAddr,
                                                 PORTPIN        pinReDe,
                                                 UINT           port,
                                                 UINT           baudrate,
                                                 SERIALTYPE     type  = SERIALTYPE_primary);

        void            sendTest                ();


        void            setAddr                 (U8             newAddr)
        { myAddr = newAddr; }

};

class ModbusMaster: public Modbus
{
        BOOL            request                 (UINT           addr,
                                                 UINT           func,
                                                 UINT           reg,
                                                 UINT           num);

public:
                        ModbusMaster            ();
                        ~ModbusMaster            ();

        BOOL            requestDi               (UINT           addr,
                                                 UINT           reg,
                                                 UINT           num)
        { return request(addr, MODBUS_FN_RD_DI, reg, num); }

        BOOL            requestDo               (UINT           addr,
                                                 UINT           reg,
                                                 BOOL           on)
        { return request(addr, MODBUS_FN_WR_DO, reg, on ? MODBUS_SC_ON : MODBUS_SC_OFF); }

        BOOL            requestAo               (UINT           addr,
                                                 UINT           reg,
                                                 UINT           num)
        { return request(addr, MODBUS_FN_RD_AO, reg, num); }

        BOOL            requestAi               (UINT           addr,
                                                 UINT           reg,
                                                 UINT           num)
        { return request(addr, MODBUS_FN_RD_AI, reg, num); }


        PMODBUSREP      readResp                (UINT           TO);

        void            freeResp                ()
        { freeLastRecvd(); }
};

class ModbusClient: public Modbus
{
        BOOL            report                  (UINT           fn,
                                                 PVOID          data,
                                                 UINT           size);

        BOOL            response                (UINT           fn,
                                                 UINT           reg,
                                                 PVOID          data,
                                                 UINT           size);


public:
        ModbusClient            ();
        ~ModbusClient            ();

//        BOOL            requestDi               (UINT           addr,
//                                                 UINT           reg,
//                                                 UINT           num)
//        { return request(addr, MODBUS_FN_RD_DI, reg, num); }
//
//        BOOL            requestAo               (UINT           addr,
//                                                 UINT           reg,
//                                                 UINT           num)
//        { return request(addr, MODBUS_FN_RD_AO, reg, num); }
//
//        BOOL            requestAi               (UINT           addr,
//                                                 UINT           reg,
//                                                 UINT           num)
//        { return request(addr, MODBUS_FN_RD_AI, reg, num); }
//
//

        PMODBUSREQ      readReq                 (UINT           TO);

        void            freeReq                 ()
        { freeLastRecvd(); }

        BOOL            reportDo                (PVOID          data,
                                                 UINT           size)
        { return report(MODBUS_FN_RD_DO, data, size); }

        BOOL            reportDi                (PVOID          data,
                                                 UINT           size)
        { return report(MODBUS_FN_RD_DI, data, size); }

        BOOL            reportAi                (PVOID          data,
                                                 UINT           size)
        { return report(MODBUS_FN_RD_AI, data, size); }

        BOOL            reportAo                (PVOID          data,
                                                 UINT           size)
        { return report(MODBUS_FN_RD_AO, data, size); }

        BOOL            respAo                  (UINT           reg,
                                                 PVOID          data,
                                                 UINT           size)
        { return response(MODBUS_FN_WR_AO, reg, data, size); }

//        BOOL            sendEcho                ()
//        { return serial.write(inBuff.getData(), lastRecvSize , MODBUS_TO) > 0; }
};

#endif // __MODBUS_H__