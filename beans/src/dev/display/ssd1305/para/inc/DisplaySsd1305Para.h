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

#ifndef __DisplaySsd1305Para_H__
#define __DisplaySsd1305Para_H__

#include <arsenal.h>

#include <Hal.h>

#include <customboard.h>

#include <DisplaySsd1305.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplaySsd1305Para: public DisplaySsd1305
{
protected:

        int             read                    (PU8            data,
                                                 INT            size);

        int             write                   (PU8            data,
                                                 INT            size);


public:
                        DisplaySsd1305Para      ();

        BOOL            dispInit                ();
};

#endif
