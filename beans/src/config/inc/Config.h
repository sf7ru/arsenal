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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <arsenal.h>
#include <Storage.h>
#include <app.h>
#include <customboard.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Config
{
protected:
        UINT            profileSize;

virtual BOOL            checkIntegrity          (PVOID          blob)   { return true; };

virtual BOOL            adoptBlob               (PVOID          blob)   { return true; };

virtual UINT            extractBlob             (PVOID          blob)       = 0;

virtual PVOID           getBufferForBlob        ()                          = 0;

virtual void            freeBufferForBlob       (PVOID          blob)   {};

public:

                        Config                  ();
virtual                 ~Config                 ();

        BOOL            load                    (UINT           profile = 0);

        BOOL            save                    (UINT           profile = 0);

virtual void            setDefaultValues        ()                          = 0;

};

#endif //  #ifndef __CONFIG_H__
