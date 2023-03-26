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
#include <axstring.h>
#include <AXConfig.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      AXDotIni::confParameter
// PURPOSE
//
// PARAMETERS
//      PSTR   paramTag   --
//      PSTR   paramValue --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXDotIni::confParameter(PSTR   paramTag,
                             PSTR   paramValue)
{
    BOOL            b_result        = false;
    UINT            len;
    UINT            param;

    ENTER(true);

    if ((len = strlen(paramTag)) > 0)
    {
        if ( (*paramTag             == DEFSCR_DEF_CHAPOPEN)     &&
             (*(paramTag + len - 1) == DEFSCR_DEF_CHAPCLOSE)    )
        {
            *(paramTag + len - 1)   = 0;
            b_result                = true;

            strz_cpy(chapter, paramTag + 1, AXCONF_TAG_SIZE);

            if (  slut                                                  &&
                  ((chapterId = axslut_find(slut, chapter)) != AXII)  )
            {
                b_result = iniChapter(chapterId);
            }
            else
            {
                b_result = iniChapter(chapter);
            }
        }
        else
        {
            if (  slut                                                  &&
                  ((param = axslut_find(slut, paramTag)) != AXII)     )
            {
                b_result = iniParameter(chapterId, param, paramValue);
            }
            else
            {
                b_result = iniParameter(chapter, paramTag, paramValue);
            }
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXDotIni::iniParse
// PURPOSE
//
// PARAMETERS
//      PLA6SLUT   slut      --
//      UINT       paramSize --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXDotIni::iniParse(HAXSLUT       lut,
                        UINT           paramSize)
{
    BOOL            b_result        = false;

    ENTER(true);

    slut        = lut;
    b_result    = confParse(paramSize);

    RETURN(b_result);
}

