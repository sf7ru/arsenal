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

#include <Config.h>
#include <string.h>
#include <axstring.h>
#include <stdio.h>
#include <math.h>
#include <axutils.h>

#include <axhashing.h>

#include <app.h>
#include <axdbg.h>

#include <customboard.h>
#include <Storage.h>
#include SUBSYS_INCLUDE(STORAGE)
#include <IntRtc.h>

//#undef AXTRACE
//#define AXTRACE     printf


#ifdef CONFIG_BOOTLD
#include <bootld.h>
#endif

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SETVAR(a,b,c)       case B4V_##b: a.c = value; result = true; break

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_HEADER
{
    U16                     checksum;
    U32                     magic;
    U16                     size;
} HEADER, * PHEADER;

typedef struct __tag_SAVEDCONFIG
{
//#ifdef CONFIG_BOOTLD
//    BOOTLDSIGN              sign;
//#endif

    HEADER                  header;


} SAVEDCONFIG, * PSAVEDCONFIG;


//STATIC_ASSERT(sizeof(SAVEDCONFIG) <= STORAGE_PAGE_SIZE, 1);

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

#if defined(CONFIG_HAVE_CC) || defined(CONFIG_HAVE_REPORTS)
    static INT ccVals          [ 8 ];
#endif

#ifdef CONFIG_HAVE_ADC
    static double doubleVals    [ 6 ];
#endif

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      Config::Config
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
Config::Config()
{

}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      Config::~Config
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
Config::~Config()
{

}
// ***************************************************************************
// FUNCTION
//      Config::load
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL  Config::load(UINT           profile)
{
    BOOL            result          = false;
    SUBSYS_STORAGE & stor            = SUBSYS_STORAGE::getInstance();
    StorageFile *   file;
    PVOID           blob;

    ENTER(true);

    if ((file = stor.open(StorageFileId_config)) != nil)
    {
        if ((blob = getBufferForBlob()) != nil)
        {
            if (file->read(profile * file->getSectorSize(), blob, profileSize))
            {
                if (adoptBlob(blob))
                {
                    result = true;
                }
            }

            freeBufferForBlob(blob);
        }

        stor.close(file);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Config::save
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Config::save(UINT profile)
{
    BOOL            result          = false;
    SUBSYS_STORAGE & stor            = SUBSYS_STORAGE::getInstance();
    StorageFile *   file;
    PVOID           blob;
    UINT            sz;

    ENTER(true);

    axsleep(50);

    if ((file = stor.open(StorageFileId_config, true)) != nil)
    {
        axsleep(50);

        if ((blob = getBufferForBlob()) != nil)
        {
            if ((sz = extractBlob(blob)) != nil)
            {
                if (file->write(0, blob, sz) > 0)
                {
                    result = true;
                }
            }

            freeBufferForBlob(blob);
        }

        stor.close(file);
    }


//        if ((saved = CREATE(SAVEDCONFIG)) != nil)
//        {
//#ifdef CONFIG_BOOTLD
//            saved->header.magic     |= CONFIG_HAVE_BOOTSIGN_FLAG;
//            saved->sign.signKey      = sign.isSigned ? 1 : -1;
//#endif
//
//#ifdef CONFIG_HAVE_ID
//            saved->header.magic     |= CONFIG_HAVE_ID_FLAG;
//            saved->model             = base.model;
//            saved->id                = base.id;
//#endif
//
//#ifdef CONFIG_HAVE_BUTTONS
//            saved->header.magic     |= CONFIG_HAVE_BUTTONS_FLAG;
//            saved->buttonTo1_1       = base.buttonTo1_1;
//            saved->buttonTo1_2       = base.buttonTo1_2;
//            saved->buttonTo2         = base.buttonTo2;
//            saved->buttonTo3         = base.buttonTo3;
//#endif
//
//#ifdef CONFIG_HAVE_REPORTS
//            saved->header.magic     |= CONFIG_HAVE_REPORTS_FLAG;
//            saved->periodLocalReports    = base.periodLocalReports;
//            saved->periodRemoteReports   = base.periodRemoteReports;
//#endif
//
//#ifdef CONFIG_HAVE_CC
//            saved->header.magic     |= CONFIG_HAVE_CC_FLAG;
//            saved->fan1On       = cc.fan1On;
//            saved->fan1Off      = cc.fan1Off;
//
//            saved->fan2On       = cc.fan2On;
//            saved->fan2Off      = cc.fan2Off;
//
//            saved->mtaxOn       = cc.mtaxOn;
//
//            saved->easOn        = cc.easOn;
//
//            saved->heaterOn     = cc.heaterOn;
//            saved->heaterOff    = cc.heaterOff;
//#endif
//
//#ifdef CONFIG_HAVE_VEC
//            saved->header.magic     |= CONFIG_HAVE_VEC_FLAG;
//            memcpy(&saved->accCalibData, &vec.accCalibData, sizeof(SAVEDCALIB));
//            memcpy(&saved->magCalibData, &vec.magCalibData, sizeof(SAVEDCALIB));
//    //        memcpy(&saved->ext1CalibData1, &vecext1.calibData1, sizeof(SAVEDCALIB));
//    //        memcpy(&saved->ext1CalibData2, &vecext1.calibData2, sizeof(SAVEDCALIB));
//    //        memcpy(&saved->ext2CalibData1, &vecext2.calibData1, sizeof(SAVEDCALIB));
//    //        memcpy(&saved->ext2CalibData2, &vecext2.calibData2, sizeof(SAVEDCALIB));
//#endif
//
//#ifdef CONFIG_HAVE_ADC
//            saved->header.magic     |= CONFIG_HAVE_ADC_FLAG;
//            saved->vRef          = adc.vRef   * 10000;
//            saved->divF1         = adc.divF1  * 10000;
//            saved->divF2         = adc.divF2  * 10000;
//            saved->vZeroPoint1   = adc.vZeroPoint1 * 1000;
//            saved->vZeroPoint2   = adc.vZeroPoint2 * 1000;
//            saved->vZeroPoint3   = adc.vZeroPoint3 * 1000;
//
//            saved->vCellMin      = adc.vCellMin * 10000;
//            saved->vCellMax      = adc.vCellMax * 10000;
//            saved->cellsNum      = adc.cellsNum;
//            saved->adcFlags      = adc.flags;
//#endif
//
//#ifdef CONFIG_HAVE_AUTH
//            saved->authValue1    = auth.value1;
//            saved->authValue2    = auth.value2;
//#endif
//
//#ifdef CONFIG_KEY_SZ
//            memcpy(&saved->keyValue, &key.value, CONFIG_KEY_SZ);
//#endif
//
//#ifdef CONFIG_USER_STRUCT
//            memcpy(&saved->usrBlob, &usr, sizeof(CONFIG_USER_STRUCT));
//#endif
//
//            saved->header.size = sizeof(SAVEDCONFIG);
//            int sz = saved->header.size - sizeof(HEADER);
//
//#ifdef CONFIG_BOOTLD
//            sz                 -= sizeof(BOOTLDSIGN);
//#endif
//
//            saved->header.checksum = fletcher16_calc((&saved->header + 1), sz);
//
////                    printf("\nSAVE 4: ");
////                    PU8 on = (PU8)saved;
////                    for (UINT i = 0; i < sizeof(SAVEDCONFIG); i++)
////                    {
////                        printf("%.2X ", *(on++));
////                    }
////                    printf("\n\n");
//
//                    AXTRACE("Config::save stage 3");
//
//            //for (int offset = 0; !result && (offset < CONFIG_COPIES); offset++)
//            {
//                //            AXTRACE("Config::save stage 4");
//
//                if (file->write(0, saved, sizeof(SAVEDCONFIG)) > 0)
//                {
//                    //                AXTRACE("Config::save stage 5");
//
//                    result = true;
//                }
//            }
//
//            FREE(saved);
//        }
//
//        stor.close(file);
//    }

    RETURN(result);
}
