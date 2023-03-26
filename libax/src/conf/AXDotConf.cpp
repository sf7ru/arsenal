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

#include <stdlib.h>
#include <AXConfig.h>
#include <axfile.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      AXDotConf::~AXDotConf
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXDotConf::~AXDotConf(void)
{
    ENTER(true);

    if(scriptText)
        FREE(scriptText);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXDotConf::confLoad
// PURPOSE
//
// PARAMETERS
//      PSTR   fileName --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXDotConf::confLoad(PSTR           fileName)
{
    BOOL            result        = false;
    UINT            size;

    ENTER(true);

    if ((scriptText = (PU8)axfile_load(fileName, &size, 1)) != NULL)
    {
        *(scriptText + size)   = 0;
        p_curr_position         = scriptText;
        result                = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXDotConf::confFree
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      void  --
// ***************************************************************************
void AXDotConf::confFree(void)
{
    ENTER(true);

    if (scriptText)
    {
        FREE(scriptText);
        scriptText = NULL;
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXDotConf::confParse
// PURPOSE
//
// PARAMETERS
//      UINT   paramSize --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXDotConf::confParse(UINT paramSize)
{
    BOOL            result        = false;
    PCSTR            onText;
    PSTR            onTmp;
    PSTR            paramValue;
    PSTR            paramTag;

    ENTER(true);

    if ((paramValue = new CHAR[paramSize]) != NULL)
    {
        if ((paramTag = new CHAR[AXCONF_TAG_SIZE]) != NULL)
        {
            result  = true;
            onText  = (PSTR)scriptText;

            while (  result                       &&
                     *(onText =
                        strz_notspace(onText, -1, false))   )
            {
                if (*onText != c_remark)
                {
// ----------------------------- Getting Tag ---------------------------------

                    onTmp = paramTag;

                    while (  *onText                                 &&
                             (*(PU8)onText != '\n')                &&
                             (*(PU8)onText != c_equation) )
                        *(onTmp++) = *(onText++);

                    *onTmp = 0;

                    strz_trim(paramTag);

                    if (*onText == c_equation)
                        onText++;

                    if ( (*(onText = strz_notspace(onText, -1, true)))  &&
                         (*onText != '\n')                                    )
                    {
// -------------------------- Getting Parameter ------------------------------

                        onTmp = paramValue;

                        while (  *onText                     &&
                                 (*(PU8)onText != '\n')    )
                            *(onTmp++) = *(onText++);

                        *onTmp = 0;

                        strz_trim(paramValue);
                    }
                    else
                        *paramValue  = 0;

                    result = confParameter(paramTag, paramValue);
                }
                else
                    onText = strz_eol(onText, -1);
            }

            delete[] paramTag;
        }

        delete[] paramValue;
    }

    RETURN(result);
}

