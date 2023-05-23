// ***************************************************************************
// TITLE
//
// PROJECT
//     cart
// ***************************************************************************


#ifndef __R4850_H__
#define __R4850_H__

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#define MAX_CURRENT_MULTIPLIER		20

#define R48xx_DATA_INPUT_POWER		0x70
#define R48xx_DATA_INPUT_FREQ		0x71
#define R48xx_DATA_INPUT_CURRENT	0x72
#define R48xx_DATA_OUTPUT_POWER		0x73
#define R48xx_DATA_EFFICIENCY		0x74
#define R48xx_DATA_OUTPUT_VOLTAGE	0x75
#define R48xx_DATA_OUTPUT_CURRENT_MAX	0x76
#define R48xx_DATA_INPUT_VOLTAGE	0x78
#define R48xx_DATA_OUTPUT_TEMPERATURE	0x7F
#define R48xx_DATA_INPUT_TEMPERATURE	0x80
#define R48xx_DATA_OUTPUT_CURRENT	0x81
#define R48xx_DATA_OUTPUT_CURRENT1	0x82

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

struct RectifierParameters
{
	float input_voltage;
	float input_frequency;
	float input_current;
	float input_power;
	float input_temp;
	float efficiency;
	float output_voltage;
	float output_current;
	float max_output_current;
	float output_power;
	float output_temp;
	float amp_hour;
};


#endif // #define __R4850_H__