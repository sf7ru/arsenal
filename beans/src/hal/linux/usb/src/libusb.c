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

#include <libusb.h>
#include <axstring.h>
#include <string.h>
#include <stdio.h>

//#include <unistd.h>
#include <sys/stat.h>
//#include <sys/ioctl.h>

#include <axstring.h>
#include <axfile.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define USB_MAXDRIVERNAME 255

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

struct usb_getdriver {
        unsigned int interface;
        char driver[USB_MAXDRIVERNAME + 1];
};

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      libusb_find_dev_sys_path
// PURPOSE
//
// PARAMETERS
//            libusb_context*context --
//      UINT  idVendor               --
//      UINT  idProduct              --
//      PCSTR manufacturer           --
//      PCSTR product                --
// RESULT
//      PSTR --
// ***************************************************************************
PSTR libusb_find_dev_sys_path(libusb_context*context,
                              UINT         idVendor,
                              UINT         idProduct,
                              PCSTR        manufacturer,
                              PCSTR        product,
                              PCSTR        iface)
{
    PSTR                                result          = nil;
    HAXSTR                              resultBuff;
    size_t                              idx;
    libusb_device *                     device;
    struct libusb_device_descriptor     desc;
    UINT                                p;
    libusb_device **                    list            =  nil;
    UINT                                count;
    UINT                                sz;
    int                                 i, j;
    libusb_device_handle *              handle;
    UINT                                        foundIface;
    BOOL                                        ifaceFound      = false;
    struct libusb_config_descriptor *           config;
    const struct libusb_interface_descriptor *  interface;
    U8                                          ports           [ 10 ];
    CHAR                                        buff            [ AXLLONG ];

    ENTER(true);

    if ((count = libusb_get_device_list(context, &list)) > 0)
    {
        for (idx = 0; !result && (idx < count); ++idx)
        {
            device  = list[idx];
            //desc    = {0};

            if (libusb_get_device_descriptor(device, &desc) == LIBUSB_SUCCESS)
            {
                if ( (!idVendor  || (desc.idVendor  == idVendor))   &&
                     (!idProduct || (desc.idProduct == idProduct))   )
                {
                    if (libusb_open(device, &handle) == LIBUSB_SUCCESS)
                    {
                        libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, (PU8)buff, sizeof(buff));
                        libusb_get_string_descriptor_ascii(handle, desc.iProduct, (PU8)buff, sizeof(buff));

                        if (  (libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, (PU8)buff, sizeof(buff)) > 0) &&
                              (!strcmp(buff, manufacturer))                                                                 &&
                              (libusb_get_string_descriptor_ascii(handle, desc.iProduct,      (PU8)buff, sizeof(buff)) > 0) &&
                              (!strcmp(buff, product))                                                                      )
                        {
//                            if (  (-1 == configIndex)                                                       ?
//                                  (libusb_get_active_config_descriptor(device, &config) == LIBUSB_SUCCESS)  :
//                                  (libusb_get_config_descriptor(device, configIndex, &config) == LIBUSB_SUCCESS)    )

                            if (libusb_get_active_config_descriptor(device, &config) == LIBUSB_SUCCESS)
                            {
                                if ((resultBuff = axstr_create(100)) != nil)
                                {
                                    axstr_add_c(resultBuff, "/sys/bus/usb/devices/");

                                    snprintf(buff, sizeof(buff), "%d-", libusb_get_bus_number(device));
                                    axstr_add_c(resultBuff, buff);

                                    if ((sz = libusb_get_port_numbers(device, ports, sizeof(ports))) > 0)
                                    {
                                        for (p = 0; p < sz; ++p)
                                        {
                                            snprintf(buff, sizeof(buff), "%d", ports[p]);
                                            axstr_add_c(resultBuff, buff);

                                            if ((p + 1) < sz)
                                            {
                                                axstr_add_c(resultBuff, ".");
                                            }
                                        }
                                    }

                                    if (iface != nil)
                                    {
                                        for (i = 0; !ifaceFound && (i < config->bNumInterfaces); i++)
                                        {
                                            for (j = 0; !ifaceFound && (j < config->interface[i].num_altsetting); j++)
                                            {
                                                interface = &config->interface[i].altsetting[j];

                                                if (  (libusb_get_string_descriptor_ascii(handle, interface->iInterface, (PU8)buff, sizeof(buff)) > 0)  &&
                                                      (!strcmp(buff, iface))                                                                            )
                                                {
                                                    foundIface = i;
                                                    ifaceFound = true;
                                                }
                                            }
                                        }
                                    }

                                    snprintf(buff, sizeof(buff), ":%d.%d", config->bConfigurationValue, foundIface);

                                    axstr_add_c(resultBuff, buff);

                                    result = axstr_freeze_c(resultBuff);
                                }

                                libusb_free_config_descriptor(config);
                            }
                        }

                        libusb_close(handle);
                    }
                    else
                    {
                        perror("cannot open usb device: ");
                    }
                }
            }
        }

        libusb_free_device_list(list, count);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      libusb_find_dev_dev_path
// PURPOSE
//
// PARAMETERS
//            libusb_context*context --
//      PCSTR baseName               --
//      UINT  idVendor               --
//      UINT  idProduct              --
//      PCSTR manufacturer           --
//      PCSTR product                --
// RESULT
//      PSTR --
// ***************************************************************************
PSTR libusb_find_dev_dev_path(libusb_context*context,
                              PCSTR        baseType,
                              PCSTR        baseName,
                              UINT         idVendor,
                              UINT         idProduct,
                              PCSTR        manufacturer,
                              PCSTR        product,
                              PCSTR        iface)
{
    PSTR            result          = nil;
    PSTR            sysPath;
    UINT            idx;
    struct stat     st;
    CHAR            path            [ AXLPATH ];
    CHAR            buff            [ AXLPATH ];

    ENTER(true);

    if ((sysPath = libusb_find_dev_sys_path(context, idVendor, idProduct, manufacturer, product, iface)))
    {
        snprintf(buff, sizeof(buff), "%s/%s", sysPath, baseType);

//        printf("test %s\n", buff);

        if (!stat(buff, &st))
        {
            snprintf(path, sizeof(path), "%s/%s", sysPath, baseType);
        }
        else
            strcpy(path, sysPath);

        for (idx = 0; !result && (idx < 20); ++idx)
        {
            snprintf(buff, sizeof(buff), "%s/%s%s%u", path, baseType, baseName, idx);

            if (!stat(buff, &st))
            {
                snprintf(buff, sizeof(buff), "/dev/%s%s%u", baseType, baseName, idx);
                result = strz_dup(buff);
            }
        }

        FREE(sysPath);
    }

    RETURN(result);
}
PSTR libusb_find_dev_bus_path(libusb_context*context,
                              UINT         idVendor,
                              UINT         idProduct)
{
    PSTR                                result          = nil;
    HAXSTR                              resultBuff;
    size_t                              idx;
    libusb_device *                     device;
    struct libusb_device_descriptor     desc;
    libusb_device **                    list            =  nil;
    UINT                                count;
    libusb_device_handle *              handle;
    struct libusb_config_descriptor *   config;
    CHAR                                buff            [ AXLLONG ];

    ENTER(true);

    if ((count = libusb_get_device_list(context, &list)) > 0)
    {
        for (idx = 0; !result && (idx < count); ++idx)
        {
            device  = list[idx];
            //desc    = {0};

            if (libusb_get_device_descriptor(device, &desc) == LIBUSB_SUCCESS)
            {
                if ( (!idVendor  || (desc.idVendor  == idVendor))   &&
                     (!idProduct || (desc.idProduct == idProduct))   )
                {
                    if (libusb_open(device, &handle) == LIBUSB_SUCCESS)
                    {
                        if (libusb_get_active_config_descriptor(device, &config) == LIBUSB_SUCCESS)
                        {
                            if ((resultBuff = axstr_create(100)) != nil)
                            {
                                axstr_add_c(resultBuff, "/dev/bus/usb/");

                                snprintf(buff, sizeof(buff), "%.3d/%.3d",
                                         libusb_get_bus_number(device),
                                         libusb_get_device_address(device));
                                axstr_add_c(resultBuff, buff);

                                result = axstr_freeze_c(resultBuff);
                            }

                            libusb_free_config_descriptor(config);
                        }

                        libusb_close(handle);
                    }
                    else
                    {
                        perror("cannot open usb device: ");
                    }
                }
            }
        }

        libusb_free_device_list(list, count);
    }

    RETURN(result);
}
