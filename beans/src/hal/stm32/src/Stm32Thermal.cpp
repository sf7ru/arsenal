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

#include <IntThermal.h>
#include <HalPlatformDefs.h>
#include <ThermalSensor.h>

#ifdef HAL_ADC_MODULE_ENABLED

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define tSlope  0.0025f    // Изменение напряжения (в вольтах) при изменении температуры на градус.
#define tMv_at_25                                                   0.760

//#define FACTORY_TSCALIB_MD_BASE         ((uint32_t)0x1FF80078)    /*!< Calibration Data Bytes base address for medium density devices*/
//#define FACTORY_TSCALIB_MDP_BASE        ((uint32_t)0x1FF800F8)    /*!< Calibration Data Bytes base address for medium density plus devices*/
//#define FACTORY_TSCALIB_MD_DATA         ((TSCALIB_TypeDef *) FACTORY_TSCALIB_MD_BASE)
//#define FACTORY_TSCALIB_MDP_DATA        ((TSCALIB_TypeDef *) FACTORY_TSCALIB_MDP_BASE)

#define TEMP_SENSOR_AVG_SLOPE_MV_PER_CELSIUS                        2.5f
#define TEMP_SENSOR_VOLTAGE_MV_AT_25                                760.0f
#define ADC_REFERENCE_VOLTAGE_MV                                    3300.0f
#define ADC_MAX_OUTPUT_VALUE                                        4095.0f

#ifdef STM32F4
#define TEMP110_CAL_VALUE                                           ((PU16)((U32)0x1FFF7A2E))
#define TEMP30_CAL_VALUE                                            ((PU16)((U32)0x1FFF7A2C))
#endif

#define TEMP110                                                     110.0f
#define TEMP30                                                      30.0f

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static  ADC_HandleTypeDef   hadc1;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------




IntThermal::IntThermal()
{
}
IntThermal::~IntThermal()
{
}
BOOL IntThermal::init(UINT           debounceRate)
{
    BOOL        result          = false;

    ENTER(true);


    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */
    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    __HAL_RCC_ADC1_CLK_ENABLE();

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(&hadc1) == HAL_OK)
    {
        sConfig.Channel      = ADC_CHANNEL_TEMPSENSOR;
        sConfig.Rank         = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;

        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) == HAL_OK)
        {

            result = true;
        }
    }

    RETURN(result);
}
float recalc_v1(U32 value)
{

    float mv = ((value * ADC_REFERENCE_VOLTAGE_MV) / ADC_MAX_OUTPUT_VALUE) / 1000;

    return ((mv - tMv_at_25) / tSlope) + 25 ;
}
float recalc_v2(U32 value)
{
    return ((TEMP110 - TEMP30) / ((float)(*TEMP110_CAL_VALUE) - (*TEMP30_CAL_VALUE)) * (value - (float)(*TEMP30_CAL_VALUE)) + TEMP30);
}
float IntThermal::readTemperature(UINT           TO)
{
    float           result          = TEMP_IMPOSIBLE;

    ENTER(true);

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, TO) == HAL_OK)
    {
#if (defined(STM32F4))
        result = recalc_v2(HAL_ADC_GetValue(&hadc1));
#else
        result = recalc_v1(HAL_ADC_GetValue(&hadc1));
#endif
    }
    HAL_ADC_Stop(&hadc1);

    RETURN(result);
}

#endif // HAL_ADC_MODULE_ENABLED