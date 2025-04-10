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

#include <string.h>

#include <SnPacket.h>
#include <SnEth.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


// ***************************************************************************
// FUNCTION
//      snpacket_reset
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
// RESULT
//      void --
// ***************************************************************************
void snpacket_reset(PSNPACKET          pkt)
{
    ENTER(true);

    memset(pkt, 0, sizeof(SNPACKET) - sizeof(PVOID) - sizeof(PVOID) - sizeof(UINT));

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      snpacket_create
// PURPOSE
//
// PARAMETERS
//      UINT size --
// RESULT
//      PSNPACKET --
// ***************************************************************************
PSNPACKET snpacket_create(UINT           size)
{
    PSNPACKET       result          = nil;
    UINT            buffSize;

    ENTER(true);

    buffSize = (size ? size : SNETH_MAX_FRAME_SZ) + SNETH_HDR_Q_SZ;

    if ((result = CREATE_X(SNPACKET, buffSize)) != nil)
    {
        result->buffSize    = buffSize;
        result->buffOffset  = SNETH_HDR_Q_SZ;
        result->data        = ((PU8)result) + sizeof(SNPACKET) + SNETH_HDR_Q_SZ;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      snpacket_destroy
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
// RESULT
//      PSNPACKET --
// ***************************************************************************
PSNPACKET snpacket_destroy(PSNPACKET    pkt)
{
    PSNPACKET           result          = nil;

    ENTER(true);

    if (pkt->buff)
        FREE(pkt->buff);

    FREE(pkt);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      snpacket_fill
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt        --
//      PVOID     data       --
//      UINT      size       --
//      UINT      buffSize   --
//      UINT      buffOffset --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL snpacket_fill(PSNPACKET          pkt,
                   PVOID              data,
                   UINT               size,
                   UINT               buffSize,
                   UINT               buffOffset)
{
    BOOL            result          = true;

    ENTER(true);

    pkt->buff       = nil;
    pkt->buffSize   = (buffSize ? buffSize : size) + buffOffset;
    pkt->buffOffset = buffOffset;
    pkt->data       = data;
    pkt->size       = size;
    result          = true;

    RETURN(result);
}
