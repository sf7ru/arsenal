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
#include <stdio.h>
#include <string.h>

#include <AXHttp.h>
#include <axnet.h>
#include <axstring.h>

#include <arsenal/libax/src/net/http/http.agi>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static PCSTR gError  =
"HTTP/1.0 %d Error\r\nContent-Type: text/html\r\n\r\n<html>"
"<head><title>Error %d</title></head>"
"<body><h1>HTTP Error %d</h1></body>"
"</html>";


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXHttpServer::AXHttpServer
// PURPOSE
//      Class construction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXHttpServer::AXHttpServer(void)
{
#ifdef AXHTTP_SSL
        sslContext  = nil;
#endif
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXHttpServer::~AXHttpServer
// PURPOSE
//      Class destruction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXHttpServer::~AXHttpServer(void)
{
#ifdef AXHTTP_SSL
//        sslContextDestroy();
#endif
}
// ***************************************************************************
// FUNCTION
//      AXHttpServer::give
// PURPOSE
//
// PARAMETERS
//      AXHttpReq * req --
//      UINT        TO  --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttpServer::give(AXHttpReq *   req,
                          UINT          TO)
{
    HTTPRC          rc              = HTTPRC_INTERROR;
    HTTPRC          result;
    AXHttpReq *     answ;

    ENTER(true);

    if (  ((rc = readRequest(req, TO))   == HTTPRC_OK)  &&
          ((rc = readHeader(req,  TO))   == HTTPRC_OK)  &&
          ((rc = readBody(req,    TO))   == HTTPRC_OK)  )
    {
        if ((answ = req->give(result)) != NULL)
        {
            answ->result = result;

            if (  ((rc = sendAnswer(answ, TO)) == HTTPRC_OK)    &&
                  ((rc = sendBody(answ,   TO)) == HTTPRC_OK)    )
            {
                rc = answ->result;
            }

            delete answ;
        }
        else
        {
            sendError(rc);
        }
    }
    else
    {
        sendError(rc);
    }

    RETURN(rc);
}
// ***************************************************************************
// FUNCTION
//      AXHttpServer::readRequest
// PURPOSE
//
// PARAMETERS
//      AXHttpReq * req --
//      UINT        TO  --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttpServer::readRequest(AXHttpReq *    req,
                                 UINT           TO)
{
    HTTPRC          rc      = HTTPRC_INTERROR;
    PSTR            onM;
    PSTR            onU;
    PSTR            onV;
    UINT            len;
    UINT            method;
    UINT            version;

    ENTER(true);

    if (buff.gets(TO) > 0)
    {
        onM = buff.getStrz();
        len = strlen(onM);

        if ((onM = strz_substrs_get_u(onM, &len, ' ')) != NULL)
        {
            if ((onU = strz_substrs_get_u(onM, &len, ' ')) != NULL)
            {
                if ((onV = strz_substrs_get_u(onU, &len, ' ')) != NULL)
                {
                    method  = axslut_find(lut, onM);
                    version = axslut_find(lut, onV);

                    if (  (  (method  > LUTID__SPC_method_begin)   &&
                             (method  < LUTID__SPC_method_end)     )   &&
                          (  (version > LUTID__SPC_version_begin)  &&
                             (version < LUTID__SPC_version_end)    )   )
                    {
                        if (req->copyUrn(onU, true))
                        {
                            req->method  = (HTTPM)(method  - LUTID_GET);
                            req->version = (HTTPV)(version - LUTID_HTTP_SL_1_DOT_0);
                            rc           = HTTPRC_OK;
                        }
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
//      AXHttpServer::sendError
// PURPOSE
//
// PARAMETERS
//      HTTPRC rc --
// RESULT
//      void --
// ***************************************************************************
void AXHttpServer::sendError(HTTPRC         rc)
{
    PSTR        outbuff;
    UINT        len;

    ENTER(true);

    len  = strlen(gError);
    len += (len >> 1);

    if ((outbuff = (PSTR)MALLOC(len)) != NULL)
    {
        len = snprintf(outbuff, len, gError, rc, rc, rc);

        buff.write(outbuff, len, 0);
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXHttpServer::sendAnswer
// PURPOSE
//
// PARAMETERS
//      AXHttpReq * req --
//      UINT        TO  --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttpServer::sendAnswer(AXHttpReq *     req,
                                UINT            TO)
{
    HTTPRC          rc       = HTTPRC_ERROR;
    HAXSTR          header;

    ENTER(true);

    if ((header = req->getAnswerHeader(req->result)) != NULL)
    {
        AXTRACE("  HTTP ANSWER HEADER = '%s'", axstr_c(header));

        if (buff.write(axstr_c(header), axstr_len(header), TO) > 0)
        {
            rc = HTTPRC_OK;
        }

        axstr_destroy(header);
    }

    RETURN(rc);
}
