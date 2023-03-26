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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <axdata.h>
#include <AXHttp.h>
#include <math.h>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static PCSTR    g_apsz_ctypes[] =
{
     "<must be set manually>",
     "application/x-www-form-urlencoded",
     "application/json",
     "multipart/form-data",
     "text/html",
     "text/xml",
     "text/plain"
};

static PCSTR    g_apsz_versions[] =
{
     "HTTP/1.1",
     "HTTP/1.0"
};

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addVal
// PURPOSE
//
// PARAMETERS
//      HAXSTR    req   --
//      PCSTR     field --
//      PCSTR     value --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addVal(HAXSTR             req,
                    PCSTR              field,
                    PCSTR              value)
{
    BOOL            b_result        = false;
    HAXSTR          tmp;

    ENTER(true);

    if ((tmp = axstr_create(10)) != nil)
    {
        b_result =  axstr_add_c(tmp, field)     &&
                    axstr_add_c(tmp, ": ")      &&
                    axstr_add_c(tmp, value)     &&
                    axstr_add_c(tmp, "\r\n")    &&
                    axstr_add(req, tmp)         ;

        axstr_destroy(tmp);
    }

    RETURN(b_result);
}
// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXHttpReq::AXHttpReq
// PURPOSE
//      Class construction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXHttpReq::AXHttpReq(void)
{
    urn         = nil;
    host        = nil;
    body        = nil;
    agentname   = nil;
    agentver    = nil;
    accept      = nil;
    extra       = nil;
    charset     = nil;

    flag.keepAlive  = false;

    resetRequest();
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXHttpReq::~AXHttpReq
// PURPOSE
//      Class destruction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXHttpReq::~AXHttpReq(void)
{
    resetRequest();
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::resetRequest
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void AXHttpReq::resetRequest(void)
{
    ctype       = AXHTTP_DEF_DOC_TYPE;
    method      = HTTPM_GET;
    version     = HTTPV_1_0;

    lenght      = 0;
    dataOffset  = 0;
    dataSize    = 0;
    result      = HTTPRC_ERROR;

    clearUrn();
    clearHost();
    clearBody();
    clearFields();
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::clearForAnswer
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::clearForAnswer(void)
{
    BOOL            b_result        = true;

    ENTER(true);

    lenght      = 0;
    dataOffset  = 0;
    dataSize    = 0;
    ctype       = AXHTTP_DEF_DOC_TYPE;

    clearBody();

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::clearUrn
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void AXHttpReq::clearUrn(void)
{
    if (urn && copied.urn)
    {
        FREE(urn);
    }

    urn     = nil;
    params  = nil;
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::copyUrn
// PURPOSE
//
// PARAMETERS
//      PCSTR newUrn --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::copyUrn(PCSTR          newUrn,
                        BOOL           incoming)
{
    BOOL            b_result        = false;

    ENTER(true);

    clearUrn();

    if ((urn = strz_dup((PSTR)newUrn)) != nil)
    {
        copied.urn  = true;
        b_result    = true;

        if (incoming)
            prepareParams();
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::setUrn
// PURPOSE
//
// PARAMETERS
//      PCSTR newUrn --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::setUrn(PCSTR          newUrn,
                       BOOL           incoming)
{
    BOOL            b_result        = false;

    ENTER(true);

    clearUrn();

    urn         = (PSTR)newUrn;
    copied.urn  = false;
    b_result    = true;

    if (incoming)
        prepareParams();

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::prepareParams
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void AXHttpReq::prepareParams()
{
    PSTR        on;
    UINT        left;

    if ((on = strchr(urn, '?')) != nil)
    {
        *on         = 0;
        params      = ++on;

        if ((left = strlen(params)) > 0)
        {
            paramsSize = left + 1;

            for (; left; left--, on++)
            {
                if (*on == '&')
                {
                    *on = 0;
                }
            }
        }
        else
            paramsSize = 0;
    }
    else
        params  = nil;
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::setName
// PURPOSE
//
// PARAMETERS
//      PCSTR newName --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::setName(PCSTR          newName)
{
    BOOL            b_result        = false;

    ENTER(true);

    clearUrn();

    if ((urn = encode(newName)) != nil)
    {
        copied.urn  = true;
        b_result    = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::clearHost
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void AXHttpReq::clearHost(void)
{
    if (host && copied.host)
    {
        FREE(host);
    }

    host = nil;
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::copyHost
// PURPOSE
//
// PARAMETERS
//      PCSTR newHost --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::copyHost(PCSTR          newHost)
{
    BOOL            b_result        = false;

    ENTER(true);

    clearHost();

    if ((host = strz_dup((PSTR)newHost)) != nil)
    {
        copied.host = true;
        b_result    = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::setHost
// PURPOSE
//
// PARAMETERS
//      PCSTR newHost --
//      UINT  port    --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::setHost(PCSTR          newHost,
                        UINT           port)
{
    BOOL            b_result        = false;
    CHAR            strHost         [ AXLMEDIUM ];

    ENTER(true);

    clearHost();

    strz_sformat(strHost, AXLMEDIUM, "%s:%d", newHost, port);

    if ((host = strz_dup(strHost)) != nil)
    {
        copied.host = true;
        b_result    = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::clearBody
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void AXHttpReq::clearBody(void)
{
    if (body && copied.body)
    {
        FREE(body);
    }

    body = nil;
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::copyBody
// PURPOSE
//
// PARAMETERS
//      PCSTR newBody   --
//      ULONG newLenght --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::copyBody(PCSTR          newBody,
                         ULONG          newLenght)
{
    BOOL            b_result        = false;

    ENTER(newLenght);

    clearBody();

    if ((body = (PSTR)MALLOC(newLenght + 1)) != nil)
    {
        memcpy(body, newBody, newLenght);
        *(body + newLenght) = 0;
        lenght              = newLenght;
        copied.body         = true;
        b_result            = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::setBody
// PURPOSE
//
// PARAMETERS
//      PCSTR newBody   --
//      ULONG newLenght --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::setBody(PCSTR          newBody,
                        ULONG          newLenght,
                        BOOL           isStatic)
{
    BOOL            b_result        = false;

    ENTER(newLenght);

    clearBody();

    body        = (PSTR)newBody;
    lenght      = newLenght ? newLenght : strlen(newBody);
    copied.body = !isStatic;
    b_result    = true;

    //AXTRACE("BODY '%s'", newBody);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::headerField
// PURPOSE
//
// PARAMETERS
//      PSTR field --
//      PSTR value --
// RESULT
//      HTTPRC --
// ***************************************************************************
HTTPRC AXHttpReq::headerField(PSTR           field,
                              PSTR           value)
{
    HTTPRC          rc      = HTTPRC_OK;

    ENTER(true);

    AXTRACE("unknown HTTP field '%s' = '%s'", field, value);

    RETURN(rc);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::addField
// PURPOSE
//
// PARAMETERS
//      PCSTR field --
//      PCSTR value --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::addField(PCSTR          field,
                         PCSTR          value)
{
    BOOL            b_result        = false;

    ENTER(true);

    if (extra || ((extra = axstr_create(10)) != nil))
    {
        _addVal(extra, field, value);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::clearFields
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void AXHttpReq::clearFields(void)
{
    ENTER(true);

    if (extra)
        extra = axstr_destroy(extra);


    SAFEFREE(accept);

    agentname   = nil;
    agentver    = nil;

    QUIT;
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addCrLf
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addCrLf(HAXSTR          header)
{
    return axstr_add_c(header, (PSTR)"\r\n");
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addVersion
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header  --
//      HTTPV     version --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addVersion(HAXSTR          header,
                        HTTPV           version)
{
    return axstr_add_c(header, g_apsz_versions[version]);
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addRange
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header   --
//      ULONG     u_offset --
//      ULONG     u_size   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addRange(HAXSTR           header,
                      ULONG            u_offset,
                      ULONG            u_size)
{
    BOOL            b_result        = true;
    CHAR            sz_string       [ 64 + 1 ];

    ENTER(true);

    if (u_offset || u_size)
    {
        if (u_size)
            snprintf(sz_string, 64, "Range: Bytes=%lu-%lu\r\n", u_offset, u_offset + u_size - 1);
        else
            snprintf(sz_string, 64, "Range: Bytes=%lu-\r\n", u_offset);

        b_result = axstr_add_c(header, sz_string);
    }

    RETURN(b_result);
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addLenght
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header   --
//      ULONG     u_lenght --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addLenght(HAXSTR           header,
                        ULONG           u_lenght)
{
    BOOL            b_result        = true;
    CHAR            sz_string       [ 64 + 1 ];

    ENTER(true);

    snprintf(sz_string, 64, "Content-Length: %lu\r\n", u_lenght);

    b_result = axstr_add_c(header, sz_string);

    RETURN(b_result);
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addCType
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
//      HTTPCT    ctype  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addCType(HAXSTR           header,
                      HTTPCT           ctype,
                      PCSTR            charset)
{
    BOOL            b_result        = true;
    CHAR            sz_charset      [ AXLSHORT + 1 ] = "";
    CHAR            sz_string       [ AXLSHORT + 1 ];

    ENTER(true);

    if (ctype != HTTPCT_MANUAL_SET)
    {
        if (charset)
        {
            strz_sformat(sz_charset, AXLSHORT, "; charset=%s", charset);
        }

        snprintf(sz_string, AXLSHORT, "Content-Type: %s%s\r\n",
             (ctype < HTTPCT_text_plain)        ?
             g_apsz_ctypes[ctype]               :
             g_apsz_ctypes[HTTPCT_text_plain]   ,
             sz_charset);

        //printf("_addCType stage TYPE = %s\n", sz_string);


        b_result = axstr_add_c(header, sz_string);
    }
    else
        b_result = true;

    RETURN(b_result);
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addSpace
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addSpace(HAXSTR           header)
{
    return axstr_add_c(header, (PSTR)" ");
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addHost
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
//      PCSTR     host   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addHost(HAXSTR         header,
                     PCSTR          host)
{
    return host ? _addVal(header, "Host", host) : true;
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addServer
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addServer(HAXSTR         header)
{
    return _addVal(header, "Server", "Built-in");
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addUserAgent
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
//      PCSTR     agent  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addUserAgent(HAXSTR           header,
                          PCSTR            agentname,
                          PCSTR            agentver)
{
    CHAR        tmp     [ AXLPATH ];

    snprintf(tmp, AXLPATH, "%s/%s",
             agentname ? agentname : "AXHttp-based",
             agentver  ? agentver  : "0.01");

    //printf("_addUserAgent stage User-Agent = %s\n", tmp);

    return _addVal(header, "User-Agent", tmp);
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addAccept
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
//      PCSTR     accept --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addAccept(HAXSTR           header,
                       PCSTR            accept)
{
    return _addVal(header, "Accept", accept ? accept : "*/*");
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addExtra
// PURPOSE
//
// PARAMETERS
//      HAXSTR    req   --
//      HAXSTR    extra --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addExtra(HAXSTR           req,
                      HAXSTR           extra)
{
    return extra ? axstr_add(req, extra) : true;
}
BOOL AXHttpReq::_addConnection(HAXSTR        req,
                               BOOL          keepAlive)
{
    //strz_sformat(tmpBuff, sizeof(tmpBuff), "%s: %s", "Connection", keepAlive ? "keep-alive" : "close");

    return _addVal(req, "Connection", keepAlive ? "keep-alive" : "close");
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::_standardHeader
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::_standardHeader(HAXSTR         header)
{
    BOOL            b_result        = false;

    ENTER(true);

    b_result = _addRange(header, dataOffset, dataSize)      &&
               _addHost(header, host)                       &&
               _addCType(header, ctype, charset)            &&
//               _addUserAgent(header, agentname, agentver)   &&
               _addAccept(header, accept)                   &&
               _addConnection(header, flag.keepAlive)       &&
               _addExtra(header, extra)                     &&
               _addLenght(header, lenght)                   &&
               _addCrLf(header)                             ;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::getRequestHeader
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      HAXSTR    --
// ***************************************************************************
HAXSTR    AXHttpReq::getRequestHeader(void)
{
    HAXSTR          header      = nil;

    ENTER(true);

    if ((header = axstr_dup_c((PSTR)(method == HTTPM_POST ?
                        "POST /" : "GET /"))) != nil)
    {
        if ( !( axstr_add_c(header, (PSTR)urn)  &&
                _addSpace(header)               &&
                _addVersion(header, version)    &&
                _addCrLf(header)                &&
                _standardHeader(header)         ))
        {
            axstr_destroy(header);
        }
    }

    RETURN(header);
}
// ***************************************************************************
// TYPE
//      inline
// FUNCTION
//      _addRc
// PURPOSE
//
// PARAMETERS
//      HAXSTR    header --
//      HTTPRC    result --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
inline BOOL _addRc(HAXSTR         header,
                   HTTPRC         result)
{
    BOOL            b_result        = true;
    CHAR            sz_string       [ 64 + 1 ];

    ENTER(true);

    snprintf(sz_string, 64, " %d Result code %d\r\n", result, result);

    b_result = axstr_add_c(header, sz_string);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::getAnswerHeader
// PURPOSE
//
// PARAMETERS
//      HTTPRC result --
// RESULT
//      HAXSTR    --
// ***************************************************************************
HAXSTR    AXHttpReq::getAnswerHeader(HTTPRC         result)
{
    HAXSTR          header      = nil;

    ENTER(true);

    if ((header = axstr_create(10)) != nil)
    {
        if ( !( _addVersion(header, version)    &&
                _addRc(header, result)          &&
                _addServer(header)              &&
                _standardHeader(header)         ))
        {
            axstr_destroy(header);
        }
    }

    RETURN(header);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::saveData
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      INT   size --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::saveData(PVOID          data,
                         INT            size)
{
    BOOL            b_result        = false;
    PSTR            newBody;
    ULONG           newLenght;

    ENTER(true);

    if (body)
    {
        if (copied.body)
        {
            newLenght = dataOffset + size;

            if ((newBody = (PSTR)REALLOC(body, newLenght + 1)) != nil)
            {
                body     = newBody;
                b_result = true;

                memcpy(body + dataOffset, data, size);

                *(body + newLenght) = 0;
            }
        }
    }
    else
        b_result = copyBody((PCSTR)data, size);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::loadData
// PURPOSE
//
// PARAMETERS
//      ULONG offset --
//      PVOID data   --
//      INT   size   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::loadData(ULONG          offset,
                         PVOID          data,
                         INT            size)
{
    BOOL            b_result        = false;

    ENTER(true);

    if (body && ((offset + size) <= lenght))
    {
        memcpy(data, body + offset, size);
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::encode
// PURPOSE
//
// PARAMETERS
//      PCSTR source --
// RESULT
//      PSTR --
// ***************************************************************************
PSTR AXHttpReq::encode(PCSTR source)
{
#define NIB2C(a)    ((a) + (((a) > 9) ? 'a' : '0'))

    PSTR        result      = nil;
    PCSTR       ons;
    PSTR        ont;

    if ((result = ont = (PSTR)MALLOC((strlen(source) * 3) + sizeof(CHAR))) != nil)
    {
        for (ons = source; *ons; ons++)
        {
            if (' ' == *ons)
            {
                *(ont++) = '%';
                *(ont++) = NIB2C((*ons >> 4) & 0xf);
                *(ont++) = NIB2C(*ons & 0xf);
            }
            else
                *(ont++) = *ons;
        }

        *ont = 0;
    }

    return result;

#undef  NIB2C
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::decode
// PURPOSE
//
// PARAMETERS
//      PCSTR source --
// RESULT
//      PSTR --
// ***************************************************************************
PSTR AXHttpReq::decode(PCSTR source)
{
    PSTR    result = nil;
    UINT    len;

    len     = strlen(source);

    if ((result = (PSTR)MALLOC(len + sizeof(CHAR))) != nil)
    {
        strz_url_decode(result, len + sizeof(CHAR), source);
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::paramStr
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR AXHttpReq::paramStr(PCSTR          name)
{
    PCSTR           result          = nil;
    UINT            left;
    UINT            valSize;
    PSTR            on;
    UINT            nameSize;

    ENTER(true);

    if ((params != nil) && paramsSize && (name != nil))
    {
        on          = params;
        left        = paramsSize;
        nameSize    = strlen(name);

        while (!result && left)
        {
            valSize = strz_size(on, left);

            if (  (valSize > nameSize)          &&
                  (!memcmp(name, on, nameSize)) &&
                  (*(on + nameSize) == '=')     )
            {
                result = (on + nameSize + 1);
            }
            else
            {
                left -= valSize;
                on   += valSize;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::paramInt
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      INT --
// ***************************************************************************
INT AXHttpReq::paramInt(PCSTR          name)
{
    UINT            result          = 0;
    PCSTR           val;

    ENTER(true);

    if ((val = paramStr(name)) != nil)
    {
        result = atoi(val);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::paramLong
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      LLONG --
// ***************************************************************************
LLONG AXHttpReq::paramLong(PCSTR          name)
{
    LLONG           result          = 0;
    PCSTR           val;

    ENTER(true);

    if ((val = paramStr(name)) != nil)
    {
        result = atoll(val);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::paramLong
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      LLONG --
// ***************************************************************************
double AXHttpReq::paramDouble(PCSTR          name)
{
    double          result          = NAN;
    PCSTR           val;

    ENTER(true);

    if ((val = paramStr(name)) != nil)
    {
        result = atof(val);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::setAuth
// PURPOSE
//
// PARAMETERS
//      PCSTR username --
//      PCSTR password --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::setAuth(PCSTR          username,
                        PCSTR          password)
{
    BOOL            result          = false;
    UINT            size;
    CHAR            couple          [ AXLSHORT ];
    CHAR            encoded         [ AXLSHORT + 1 ];
    CHAR            auth            [ AXLMEDIUM ];

    ENTER(true);

    strz_sformat(couple, AXLLONG, "%s:%s", username, password);

    if ((size = strz_base64_encode(encoded, AXLSHORT, couple, strlen(couple))) > 0)
    {
        *(encoded + size) = 0;

        strz_sformat(auth, AXLMEDIUM, "Basic %s", encoded);

        addField("Authorization", auth);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::setVersion
// PURPOSE
//
// PARAMETERS
//      HTTPV version --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::setVersion(HTTPV version)
{
    BOOL            result          = false;

    ENTER(true);

    if (version <= HTTPV_1_0)
    {
        this->version   = version;
        result          = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpReq::setCharset
// PURPOSE
//
// PARAMETERS
//      PCSTR charset --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXHttpReq::setCharset(PCSTR    charset)
{
    BOOL            result          = false;

    ENTER(true);

    this->charset = charset;

    RETURN(result);
}
