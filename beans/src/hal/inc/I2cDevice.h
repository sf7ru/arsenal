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

#ifndef __I2CDEVICE_H__
#define __I2CDEVICE_H__

#include <arsenal.h>
//#include <I2c.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class I2c;

class I2cDevice
{
        friend class    I2c;

protected:
        UINT            mDevAddr;
        I2c *           mIface;

        INT             mDevFd;

public:
                        I2cDevice               ();

virtual                 ~I2cDevice              ();

        void            setAddr                 (UINT           addr)
        {
                mDevAddr = addr;
        }

        BOOL            init                    (UINT           addr,
                                                 UINT           ifaceNo  = 0,
                                                 UINT           speed    = 100000);

        INT             transaction             (INT            wr_len,
                                                 PU8            wr_data,
                                                 INT            rd_len,
                                                 PU8            rd_data);

//        INT             deviceRead              (PVOID          data,
//                                                 INT            size,
//                                                 BOOL           repeated = true);
//
//        INT             deviceWrite             (PVOID          data,
//                                                 INT            size);
//
        INT             registerWrite           (U8             reg_addr,
                                                 U8             data);

        INT             registerWrite           (U8             reg_addr,
                                                 PVOID          data,
                                                 INT            size);

        INT             registerRead            (U8             reg_addr,
                                                 PVOID          data,
                                                 INT            len);

        U8              registerRead            (U8             reg_addr);

        INT             registerWriteChecked    (UINT           reg,
                                                 U8             value,
                                                 UINT           tries);

        INT             registerModifyChecked   (UINT           reg,
                                                 U8             clearbits,
                                                 U8             setbits,
                                                 UINT           tries);

        INT             registerModify          (UINT           reg,
                                                 U8             clearbits,
                                                 U8             setbits);

};

#endif // #ifndef __I2CDEVICE_H__
