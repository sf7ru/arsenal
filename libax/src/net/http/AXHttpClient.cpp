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

//#define APPTRACE

#include <AXHttp.h>
#include <axstring.h>
#include <stdlib.h>
#include <stdio.h>

#include <arsenal/libax/src/net/http/http.agi>
#include <string.h>

//#define AXTRACE printf

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      default_app_trace
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_place --
//      PCSTR psz_msg   --
//            ...       --
// RESULT
//      void --
// ***************************************************************************
void app_trace(PCSTR    psz_place,
                       PCSTR    psz_msg,
                       ...)
{
   va_list             st_va_list;

   va_start(st_va_list, psz_msg);

   printf("%s: ", psz_place);
   vprintf(psz_msg, st_va_list);
   printf("\n");

   va_end(st_va_list);
}

// ***************************************************************************
// FUNCTION
//      AXHttpClient::AXHttpClient
// PURPOSE
//
// PARAMETERS
//      PVOID   buff --
//      UINT    size --
// RESULT
//        --
// ***************************************************************************
AXHttpClient::AXHttpClient(void)
{
    ENTER(true);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXHttpClient::~AXHttpClient
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXHttpClient::~AXHttpClient(void)
{
    ENTER(true);

    buff.close();

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXHttpClient::sendRequest
// PURPOSE
//
// PARAMETERS
//      AXHttpReq * req --
//      UINT        TO  --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttpClient::sendRequest(AXHttpReq *    req,
                                 UINT           TO)
{
    HTTPRC          rc       = HTTPRC_ERROR;
    HAXSTR          header;

    ENTER(true);

    if ((header = req->getRequestHeader()) != NULL)
    {
        AXTRACE("  HTTP REQ HEADER = '%s'", axstr_c(header));

        if (buff.write(axstr_c(header), axstr_len(header), TO) > 0)
        {
            rc = HTTPRC_OK;
        }

        axstr_destroy(header);
    }

    RETURN(rc);
}
// ***************************************************************************
// FUNCTION
//      AXHttpClient::readHttpHeader
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
HTTPRC AXHttpClient::readAnswer(AXHttpReq *    req,
                                UINT            TO)
{
    HTTPRC          rc          = HTTPRC_ERROR;
    UINT            len;
    PSTR            onV;
    PSTR            onR;
    PSTR            onC;
    UINT            version;

    ENTER(true);

    if (buff.gets(TO) > 0)
    {
        onV = buff.getStrz();
        len = strlen(onV);

        if ((onV = strz_substrs_get_u(onV, &len, ' ')) != NULL)
        {
            if ((onR = strz_substrs_get_u(onV, &len, ' ')) != NULL)
            {
                if ((onC = strz_substrs_get_u(onR, &len, '\r')) != NULL)
                {
                    version = axslut_find(lut, onV);

                    if (  (  (version > LUTID__SPC_version_begin)   &&
                             (version < LUTID__SPC_version_end)     )   )
                    {
                        req->version = (HTTPV)(version - LUTID_HTTP_SL_1_DOT_0);
                        req->method  = HTTPM_GET;
                        req->result  = (HTTPRC)atoi(onR);
                        rc           = HTTPRC_OK;

                        AXTRACE("  HTTP RESULT CODE = %d (%s)", req->result, onC);
                    }
                    else
                        rc = HTTPRC_NOTSUPPORTED;
                }
            }
        }
    }

    RETURN(rc);
}
// ***************************************************************************
// FUNCTION
//      AXHttpClient::get
// PURPOSE
//
// PARAMETERS
//      PCSTR   uri --
//      UINT    TO  --
// RESULT
//      UINT  --
// ***************************************************************************
HTTPRC  AXHttpClient::get(AXHttpReq *       req,
                          UINT              TO)
{
    HTTPRC          rc      = HTTPRC_ERROR;

    ENTER(req);

//    if (req->method == HTTPM_POST)
//    {
//        req->ctype = HTTPCT_urlencoded;
//    }

    if (  ((rc     = sendRequest(req, TO))  == HTTPRC_OK)   &&
          ((rc     = sendBody(req,    TO))  == HTTPRC_OK)   )
    {
        req->clearForAnswer();

        if (  ((rc     = readAnswer(req,  TO))  == HTTPRC_OK)   &&
              ((rc     = readHeader(req,  TO))  == HTTPRC_OK)   &&
              ((rc     = readBody(req,    TO))  == HTTPRC_OK)   )
        {
            rc = req->result;
        }
    }

    RETURN(rc);
}
