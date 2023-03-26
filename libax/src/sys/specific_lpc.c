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

#include <chip.h>

//#include <bootld.h>

#define NVIC_RAM_VECTOR_ADDRESS   (0x10000000)  // Location of vectors in RAM
#define NVIC_FLASH_VECTOR_ADDRESS (0x0)         // Initial vector position in flash

#define NVIC_NUM_VECTORS      (16 + 33)
#define NVIC_USER_IRQ_OFFSET  16

//#ifndef BOOTLD
#define VECTOR_ADDRESS      NVIC_RAM_VECTOR_ADDRESS
//#else
//#define VECTOR_ADDRESS      BOOTLD_APP_ADDR
//#endif

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      NVIC_SetVector
// PURPOSE
//
// PARAMETERS
//      IRQn_Type IRQn   --
//      uint32_t  vector --
// RESULT
//      void --
// ***************************************************************************
void NVIC_SetVector(IRQn_Type   IRQn,
                    uint32_t    vector)
{
    uint32_t *vectors = (uint32_t*)SCB->VTOR;
    uint32_t i;

    // Copy and switch to dynamic vectors if the first time called
    if (SCB->VTOR < VECTOR_ADDRESS)
    {
        uint32_t *old_vectors = vectors;
        vectors = (uint32_t*)VECTOR_ADDRESS;

        for (i=0; i<NVIC_NUM_VECTORS; i++)
        {
            vectors[i] = old_vectors[i];
        }

        SCB->VTOR = (uint32_t)VECTOR_ADDRESS;
    }

    vectors[IRQn + NVIC_USER_IRQ_OFFSET] = vector;
}
// ***************************************************************************
// FUNCTION
//      NVIC_GetVector
// PURPOSE
//
// PARAMETERS
//      IRQn_Type IRQn --
// RESULT
//      uint32_t --
// ***************************************************************************
uint32_t NVIC_GetVector(IRQn_Type IRQn)
{
    uint32_t *vectors = (uint32_t*)SCB->VTOR;
    return vectors[IRQn + NVIC_USER_IRQ_OFFSET];
}

