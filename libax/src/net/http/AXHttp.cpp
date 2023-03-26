// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      
// HISTORY
//      
// ***************************************************************************

#include <stdlib.h>
#include <AXHttp.h>
#include <axstring.h>

#define __LASLUTTABLES__                  // for SLUT body
#include <arsenal/libax/src/net/http/http.agi>
#include <stdio.h>
#include <signal.h>
#include <string.h>

//#undef A7TRACE
//#define A7TRACE(a)  {printf a; printf("\n"); }

#if (TARGET_FAMILY == __AX_unix__)
#include <signal.h>
#endif

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXHttp::AXHttp
// PURPOSE
//      Class construction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXHttp::AXHttp(void)
{
    lut         = nil;
    sslContext  = nil;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXHttp::~AXHttp
// PURPOSE
//      Class destruction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXHttp::~AXHttp(void)
{
    buff.close();

    SAFEDESTROY(axslut, lut);
}
// ***************************************************************************
// FUNCTION
//      AXHttp::initHttp
// PURPOSE
//
// PARAMETERS
//      AXDevice * dev      --
//      UINT       buffSize --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttp::initHttp(AXDevice *    dev,
                      UINT          buffSize)
{
    BOOL            b_result        = false;

    ENTER(true);

#if (TARGET_FAMILY == __AX_unix__)
#warning "SIGPIPE WILL BE IGNORED"
    signal( SIGPIPE, SIG_IGN );
#endif

    if ((lut = axslut_conv_prepared(table_lut, digtable_lut)) != NULL)
    {
        b_result = buff.open(dev, buffSize);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttp::readHeader
// PURPOSE
//
// PARAMETERS
//      AXHttpReq * req --
//      UINT        TO  --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttp::readHeader(AXHttpReq *   req,
                          UINT          TO)
{
    HTTPRC          rc              = HTTPRC_INTERROR;

    ENTER(true);

    req->lenght = (UINT)0;

    while ((buff.gets(TO) > 0) && *(PSTR)buff.getStrz())
    {
        if ((rc = headerField(req)) != HTTPRC_OK)
        {
            break;
        }
    }

    RETURN(rc);
}
// ***************************************************************************
// FUNCTION
//      AXHttp::readBody
// PURPOSE
//
// PARAMETERS
//      AXHttpReq * req --
//      UINT        TO  --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttp::readBody(AXHttpReq *    req,
                        UINT           TO)
{
    HTTPRC          rc      = HTTPRC_OK;
    int             amount;

    ENTER(true);

    req->dataOffset = 0;

    //printf("AXHttp::readBody stage 1 req->lenght = %d\n", req->lenght);

    if (req->lenght)
    {
        do
        {
            if ((amount = buff.read(TO)) > 0)
            {
                //printf("AXHttp::readBody stage 2 amount = %d\n", amount);
                //strz_dump_w_txt((PSTR)"HTTP DATA ", (PVOID)buff.getData(), buff.getSize());

                if (req->saveData((PVOID) buff.getData(), amount))
                {
                    req->dataOffset += amount;
                }
                else
                    rc = HTTPRC_ERROR;

                buff.purge(amount);
            }

            //printf("AXHttp::readBody stage 3 req->dataOffset = %d\n", req->dataOffset);

        } while (amount && (HTTPRC_OK == rc) && (req->dataOffset < req->lenght));
    }

    req->lenght = req->dataOffset;

    RETURN(rc);
}
// ***************************************************************************
// FUNCTION
//      AXHttp::headerField
// PURPOSE
//
// PARAMETERS
//      AXHttpReq * req --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttp::headerField(AXHttpReq *   req)
{
    HTTPRC          rc      = HTTPRC_INTERROR;
    PSTR            field;
    PSTR            value;
    UINT            len;

    ENTER(true);

    field   = (PSTR)buff.getData();
    len     = strlen(field);

    if ((field = strz_substrs_get_u(field, &len, ':')) != NULL)
    {
        if ((value = strz_substrs_get_u(field, &len, '\n')) != NULL)
        {
            value = strz_clean(value);

            //printf("IN HEADER FIELD '%s' = '%s'\n", field, value);

            switch(axslut_find(lut, field))
            {
                case LUTID_Host:
                    if (req->copyHost(value))
                    {
                        rc      = HTTPRC_OK;
                    }
                    break;

                case LUTID_Content_MNS_Type:
                    rc = HTTPRC_OK;
                    break;

                case LUTID_Content_MNS_Length:
                    req->lenght = atol(value);
                    rc          = HTTPRC_OK;
                    break;

                case LUTID_Connection:
                    // ' = 'keep-alive'
                case LUTID_Keep_MNS_Alive:
                case LUTID_Accept_MNS_Encoding:
                case LUTID_Accept:
                case LUTID_Accept_MNS_Language:
                case LUTID_Accept_MNS_Charset:
                case LUTID_User_MNS_Agent:
                    rc = HTTPRC_OK;
                    break;

                default:
                    rc = req->headerField(field, value);
                    break;
            }
        }
    }

    RETURN(rc);
}
// ***************************************************************************
// FUNCTION
//      AXHttp::sendBody
// PURPOSE
//
// PARAMETERS
//      AXHttpReq * req --
//      UINT        TO  --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttp::sendBody(AXHttpReq *   req,
                        UINT          TO)
{
    HTTPRC          rc      = HTTPRC_OK;
    UINT            amount;
    ULONG           left;

    ENTER(true);

    req->dataOffset = 0;
    left        = req->lenght;
    amount      = buff.getTotalSize();

    if (amount > 1024)
    {
        amount = 1500;
    }

    while (left)
    {
        if (amount > left)
        {
            amount = left;
        }

        if (  (req->loadData(req->dataOffset, (PVOID)buff.getData(), amount))    &&
              (buff.write(NULL, amount, TO) > 0)            )
        {
            left        -= amount;
            req->dataOffset += amount;
        }
        else
        {
            rc = HTTPRC_ERROR;
            break;
        }
    }

    buff.purge(buff.getSize());

    RETURN(rc);
}
