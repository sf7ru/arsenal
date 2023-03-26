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

#ifndef __ARSENAL_LIBUSB_H__
#define __ARSENAL_LIBUSB_H__

#include <arsenal.h>
#include <libusb-1.0/libusb.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

PSTR                libusb_find_dev_sys_path    (libusb_context*context,
                                                 UINT         idVendor,
                                                 UINT         idProduct,
                                                 PCSTR        manufacturer,
                                                 PCSTR        product,
                                                 PCSTR        interface);

PSTR                libusb_find_dev_dev_path    (libusb_context*context,
                                                 PCSTR        baseType,
                                                 PCSTR        baseName,
                                                 UINT         idVendor,
                                                 UINT         idProduct,
                                                 PCSTR        manufacturer,
                                                 PCSTR        product,
                                                 PCSTR        interface);

PSTR                libusb_find_dev_bus_path    (libusb_context*context,
                                                 UINT         idVendor,
                                                 UINT         idProduct);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif //  #ifndef __ARSENAL_LIBUSB_H__

