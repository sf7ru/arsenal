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

#ifndef __AXHTTPH__
#define __AXHTTPH__

#include <axdata.h>
#include <AXBuffer.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define AXHTTP_DEF_DOC_TYPE HTTPCT_text_html

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef enum
{
    HTTPRC_OK               = 200,  // OK
    HTTPRC_PARTIAL          = 206,  // Partial content

    HTTPRC_MOVED            = 301,  // Moved permanently
    HTTPRC_FOUND            = 302,  // Found
    HTTPRC_REDIRECT         = 307,  // Temporary redirect


    HTTPRC_NOTFOUND         = 404,  // File not found
    HTTPRC_NOTALLOWED       = 405,  // Method Not Allowed

    HTTPRC_NORANGE          = 416,

    HTTPRC_INTERROR         = 500,  // Internal error
    HTTPRC_NOTIMPLEMENTED   = 501,  // Not implemented
    HTTPRC_NOTAVAILABLE     = 503,  // Service Unavailable
    HTTPRC_NOTSUPPORTED     = 505,  // Version Not Supported
    HTTPRC_FATAL            = 590,
    HTTPRC_ERROR            = 591,
    HTTPRC_NOCONNECT        = 592,
    HTTPRC_NOCONTENT        = 593,


    HTTPRC_PARSING_ERROR    = 2000
} HTTPRC;

typedef enum
{
    HTTPM_GET               = 0,
    HTTPM_POST
} HTTPM;

typedef enum
{
    HTTPV_1_1               = 0,
    HTTPV_1_0, // Must stay last
} HTTPV;


// AXHttpReq.cpp g_apsz_ctypes[]
typedef enum
{
    HTTPCT_MANUAL_SET       = 0,
    HTTPCT_app_urlencoded,
    HTTPCT_app_json,
    HTTPCT_multipart,

    HTTPCT_text_html,
    HTTPCT_text_xml,
    HTTPCT_text_plain       // Must stay last (for text array handing)
} HTTPCT;

class AXHttpReq
{
friend  class           AXHttp;
friend  class           AXHttpClient;
friend  class           AXHttpServer;

protected:

        CHAR            tmpBuff     [ AXLMED ];

        HTTPRC          result;
        HAXSTR          extra;
        HTTPCT          ctype;
        HTTPM           method;
        HTTPV           version;

        PCSTR           agentname;
        PCSTR           agentver;
        PSTR            accept;

        PSTR            urn;
        PSTR            params;
        UINT            paramsSize;
        PSTR            host;
        PSTR            body;

        PCSTR           charset;

        struct
        {
            BOOL        keepAlive   : 1;
        } flag;

        struct
        {
            BOOL        urn         : 1;
            BOOL        host        : 1;
            BOOL        body        : 1;
        } copied;

        ULONG           lenght;

        ULONG           dataOffset;
        ULONG           dataSize;

        void            resetRequest            (void);

        BOOL            _addConnection          (HAXSTR         req,
                                                 BOOL           keepAlive);

        BOOL            _standardHeader         (HAXSTR         header);

        HAXSTR          getRequestHeader        (void);

        HAXSTR          getAnswerHeader         (HTTPRC         result);

        BOOL            clearForAnswer          (void);

        PSTR            encode                  (PCSTR          source);

        PSTR            decode                  (PCSTR          source);

        void            prepareParams           ();

        INT             paramInt                (PCSTR          name);

        LLONG           paramLong               (PCSTR          name);

        double          paramDouble             (PCSTR          name);

        PCSTR           paramStr                (PCSTR          name);

public:

                        AXHttpReq               (void);

virtual                 ~AXHttpReq              (void);

virtual HTTPRC          headerField             (PSTR           field,
                                                 PSTR           value);

virtual BOOL            saveData                (PVOID          data,
                                                 INT            size);

virtual BOOL            loadData                (ULONG          offset,
                                                 PVOID          data,
                                                 INT            size);

virtual AXHttpReq *     give                    (HTTPRC &       result)
                        { result = HTTPRC_OK; return new AXHttpReq; };

        void            setLenght               (ULONG          newLenght)
                        { lenght = newLenght; }

        BOOL            copyUrn                 (PCSTR          newUri,
                                                 BOOL           incoming = false);

        BOOL            setUrn                  (PCSTR          newUri,
                                                 BOOL           incoming = false);

        void            clearUrn                (void);

        BOOL            setName                 (PCSTR          newUri);

        BOOL            copyHost                (PCSTR          newUri);
        BOOL            setHost                 (PCSTR          newHost,
                                                 UINT           port);
        void            clearHost               (void);

        BOOL            copyBody                (PCSTR          newBody,
                                                 ULONG          newLenght);

        BOOL            setBody                 (PCSTR          newBody,
                                                 ULONG          newLenght   = 0,
                                                 BOOL           isStatic    = false);

        BOOL            setVersion              (HTTPV          v);

        BOOL            setCharset              (PCSTR          charset);

        BOOL            setAuth                 (PCSTR          username,
                                                 PCSTR          password);

        void            setType                 (HTTPCT         ctype)
        { this->ctype = ctype; }


        void            setAgent                (PCSTR          aname,
                                                 PCSTR          aversion)
        { agentname = aname; agentver = aversion; }


        void            clearBody               (void);

        BOOL            addField                (PCSTR          field,
                                                 PCSTR          value);

        void            clearFields             (void);

        void            setMethod               (HTTPM          newMethod)
                        { method = newMethod; }


        ULONG           getLenght               (void)
                        { return lenght; }

        PCSTR           getBody                 (void)
                        { return body; }

        PCSTR           getUrn                  (void)
                        { return urn; }

        HTTPRC          getResult               (void)
                        { return result; }

        void            setOffset               (ULONG          offset)
                        { dataOffset = offset; }

        void            setKeepAlive            (BOOL           enable)
                        { flag.keepAlive = enable; }

        void            setSize                 (ULONG          size)
                        { dataSize = size; }

virtual AXHttpReq *     clone                   (void)
                        { return new AXHttpReq; }
};

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class AXHttp
{
protected:
        HAXSLUT         lut;
        AXBuffer        buff;
        PVOID           sslContext;

        HTTPRC          readHeader              (AXHttpReq *    req,
                                                 UINT           TO);

        HTTPRC          readBody                (AXHttpReq *    req,
                                                 UINT           TO);

        HTTPRC          sendBody                (AXHttpReq *    req,
                                                 UINT           TO);

        HTTPRC          headerField             (AXHttpReq *    req);

public:
                        AXHttp                  (void);

        BOOL            addExtraField           (PCSTR          field,
                                                 PCSTR          value);

        BOOL            initHttp                (AXDevice *     dev,
                                                 UINT           buffSize);

        BOOL            setDevice               (AXDevice *     dev)
                        { buff.purge(-1); return buff.setDevice(dev); }

        void            closeDevice             ()
                        { buff.closeDevice(); }

                        ~AXHttp                 (void);
};

class AXHttpClient: public AXHttp
{
protected:

        HTTPRC          sendRequest             (AXHttpReq *    req,
                                                 UINT           TO);

        HTTPRC          readAnswer              (AXHttpReq *    req,
                                                 UINT           TO);

public:
                        AXHttpClient            (void);

                        ~AXHttpClient           (void);

        HTTPRC          get                     (AXHttpReq *    req,
                                                 UINT           TO);

};

class AXHttpServer: public AXHttp
{
protected:

        void            sendError               (HTTPRC         rc);

        HTTPRC          sendAnswer              (AXHttpReq *    req,
                                                 UINT           TO);

        HTTPRC          readRequest             (AXHttpReq *    req,
                                                 UINT           TO);

public:
                        AXHttpServer            (void);

                        ~AXHttpServer           (void);

        HTTPRC          give                    (AXHttpReq *    req,
                                                 UINT           TO);

};

class AXHttpSslWrapper: public AXDevice
{
//        BOOL            firstRead;
        PAXDEV          dev;
        PVOID           context;

                        AXHttpSslWrapper        ();

        BOOL            doAccept                ();

        BOOL            doConnect               ();

public:
                        ~AXHttpSslWrapper       (void);

        INT             read                    (PVOID          data,
                                                 INT            size,
                                                 UINT           TO);

        INT             write                   (PCVOID         data,
                                                 INT            size,
                                                 UINT           TO);

        INT             getDescriptor           ();

static  BOOL            globalInit              ();

static  void            globalDeinit            ();

static  AXHttpSslWrapper* accept                (PAXDEV         dev,
                                                 PVOID          certificate,
                                                 PVOID          privateKey);

static  AXHttpSslWrapper* connect               (PAXDEV         dev,
                                                 PVOID          certificate,
                                                 PVOID          privateKey);
};

#endif //  #ifndef __B3HTTPH__

