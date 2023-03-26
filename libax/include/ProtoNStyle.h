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


#include <stdarg.h>
#include <arsenal.h>
#include <axdata.h>
#include <AXBuffer.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class ProtoNStyle
{
protected:
        PSTR            outBuff;
        UINT            outBuffSize;
        UINT            sentenceLeft;

        AXBuffer *      inBuff;

        PCSTR           delimeter;

virtual INT             write                   (PCVOID         data,
                                                 INT            size) = 0;

virtual BOOL            lock                    ()  { return true; }

virtual void            unlock                  ()  {};

        UINT            addTailToBuff           (UINT           seq     = 0);
public:

static const int        PARAM_NIL               =  0;
static const int        PARAM_INT               =  1;
static const int        PARAM_UINT              =  2;
static const int        PARAM_DOUBLE            =  3;
static const int        PARAM_FLOAT             =  4;
static const int        PARAM_STR               =  5;
static const int        PARAM_QUOTEDSTR         =  6;
static const int        PARAM_DATE              =  7;
static const int        PARAM_HEX               =  8;
static const int        PARAM_BIN               =  9;
static const int        PARAM_UINT64            = 10;
static const int        PARAM_UINT_NZ           = 100 + PARAM_UINT;
static const int        PARAM_INT_NZ            = 100 + PARAM_INT;
static const int        PARAM_HEX_NZ            = 100 + PARAM_HEX;

                        ProtoNStyle             ();
virtual                 ~ProtoNStyle            ();

        BOOL            newSentence             (PCSTR          type);

        BOOL            addParam                (INT            paramType,
                                                 PCVOID         param,
                                                 INT            limit1,
                                                 INT            limit2);

        BOOL            putSentence             (UINT           seq             = 0);


        BOOL            postSentence            (PAXRINGBUFF    buff);

        INT             vput                    (PCSTR          type,
                                                 PCSTR          msg,
                                                 va_list        list);

        INT             put                     (PCSTR          type,
                                                 PCSTR          msg,
                                                 ...            );

        void            setDelimeter            (PCSTR          delimeter)
        {
            this->delimeter = delimeter;
        }
};
