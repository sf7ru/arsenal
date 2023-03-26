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

#ifdef HAL_RTC_MODULE_ENABLED

#include <IntRtc.h>
#include <string.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define TOTAL_SIZE          0

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static  RTC_HandleTypeDef   hrtc;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      IntRtc::Rtc
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
IntRtc::IntRtc()
{

}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      IntRtc::~Rtc
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
IntRtc::~IntRtc()
{

}
// ***************************************************************************
// FUNCTION
//      IntRtc::getDate
// PURPOSE
//
// PARAMETERS
//      PAXDATE date --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL IntRtc::getDate(PAXDATE        date)
{
    BOOL                result          = false;
    RTC_TimeTypeDef     t;
    RTC_DateTypeDef     d;

    ENTER(true);

    if (HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN) == HAL_OK)
    {
        date->hour      = t.Hours;
        date->minute    = t.Minutes;
        date->second    = t.Seconds;

        if (HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN) == HAL_OK)
        {
            date->year      = d.Year + 2000;
            date->month     = d.Month;
            date->day       = d.Date;

            result          = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IntRtc::setDate
// PURPOSE
//
// PARAMETERS
//      PAXDATE date --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL IntRtc::setDate(PAXDATE        date)
{
    BOOL                result          = false;
    RTC_TimeTypeDef     t;
    RTC_DateTypeDef     d;

    ENTER(true);

    memset(&t, 0, sizeof(t));
    memset(&d, 0, sizeof(d));

//    d.WeekDay = date->day_of_week;
    d.Year = date->year - 2000;
    d.Month = date->month;
    d.Date = date->day;
    t.Hours = date->hour;
    t.Minutes = date->minute;
    t.Seconds = date->second;
    t.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    t.StoreOperation = RTC_STOREOPERATION_RESET;

    result = (HAL_RTC_SetTime(&hrtc, &t, RTC_FORMAT_BIN) == HAL_OK) &&
             (HAL_RTC_SetDate(&hrtc, &d, RTC_FORMAT_BIN) == HAL_OK) ;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IntRtc::read
// PURPOSE
//
// PARAMETERS
//      PVOID buff   --
//      UINT  offset --
//      INT   size   --
// RESULT
//      INT  --
// ***************************************************************************
INT  IntRtc::read(PVOID          buff,
               UINT           offset,
               INT            size)
{
    INT             result          = -1;
    PU32            on;

    ENTER(true);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IntRtc::write
// PURPOSE
//
// PARAMETERS
//      PVOID buff   --
//      UINT  offset --
//      INT   size   --
// RESULT
//      INT --
// ***************************************************************************
INT IntRtc::write(PVOID          buff,
               UINT           offset,
               INT            size)
{
    INT             result          = -1;
    PU32            on;

    ENTER(true);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IntRtc::getSize
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      INT --
// ***************************************************************************
INT IntRtc::getSize()
{
    return TOTAL_SIZE;
}

extern "C"
{
    void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
    {
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
        if(hrtc->Instance==RTC)
        {
        /* USER CODE BEGIN RTC_MspInit 0 */

        /* USER CODE END RTC_MspInit 0 */
        /** Initializes the peripherals clock
         */
            PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
            PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
            if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
            {
                //Error_Handler();
            }
            
            /* Peripheral clock enable */
            __HAL_RCC_RTC_ENABLE();
        /* USER CODE BEGIN RTC_MspInit 1 */

        /* USER CODE END RTC_MspInit 1 */
        }
   
    } 
}

// ***************************************************************************
// FUNCTION
//      IntRtc::rtcInit
// PURPOSE
//
// PARAMETERS
//      UINT addr       --
//      UINT i2cIfaceNo --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL IntRtc::rtcInit(UINT           addr,
                     UINT           i2cIfaceNo)
{
    BOOL            result          = false;

    ENTER(true);

    __HAL_RCC_RTC_ENABLE();

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) == HAL_OK)
    {
        result = true;
    }

    RETURN(result);
}
PCSTR IntRtc::getName()
{ return "Internal RTC"; }


#endif // #ifdef HAL_TIM_MODULE_ENABLED