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

#define OPENSSL_THREADS

#include <AXHttp.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>

#include <openssl/rsa.h>       // SSLeay stuff
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/opensslconf.h>

#include <pvt_net.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define MUTEX_TYPE       pthread_mutex_t
#define MUTEX_SETUP(x)   pthread_mutex_init(&(x), NULL)
#define MUTEX_CLEANUP(x) pthread_mutex_destroy(&(x))
#define MUTEX_LOCK(x)    pthread_mutex_lock(&(x))
#define MUTEX_UNLOCK(x)  pthread_mutex_unlock(&(x))
#define THREAD_ID        pthread_self()


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_SSLCONTEXT
{
        const SSL_METHOD *  meth;
        SSL_CTX*            ctx;
        SSL*                ssl;
        int                 fd;
} SSLCONTEXT, * PSSLCONTEXT;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static MUTEX_TYPE *mutex_buf= NULL;



// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      locking_function
// PURPOSE
//
// PARAMETERS
//      int          mode --
//      int          n    --
//      const char * file --
//      int          line --
// RESULT
//      void --
// ***************************************************************************
static void locking_function(int mode, int n, const char * file, int line)
{
  if(mode & CRYPTO_LOCK)
    MUTEX_LOCK(mutex_buf[n]);
  else
    MUTEX_UNLOCK(mutex_buf[n]);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      id_function
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      unsigned long --
// ***************************************************************************
static unsigned long id_function(void)
{
  return ((unsigned long)THREAD_ID);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSslWrapper::accept
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev         --
//      PVOID  certificate --
//      PVOID  privateKey  --
// RESULT
//      AXHttpSslWrapper * --
// ***************************************************************************
AXHttpSslWrapper * AXHttpSslWrapper::accept(PAXDEV         dev,
                                            PVOID          certificate,
                                            PVOID          privateKey)
{
    AXHttpSslWrapper *  result          = nil;
    PSSLCONTEXT         c;

    ENTER(true);

    if ((result = new AXHttpSslWrapper) != nil)
    {
        if ((c = CREATE(SSLCONTEXT)) != nil)
        {
            result->context  = c;
            result->dev         = dev;

            c->meth = SSLv23_server_method();

            if ((c->ctx = SSL_CTX_new (c->meth)) != nil)
            {
                if (  !certificate ||
                      (SSL_CTX_use_certificate_file(c->ctx, (PCSTR)certificate, SSL_FILETYPE_PEM) > 0))
                {
                    if (  !privateKey ||
                          ( (SSL_CTX_use_PrivateKey_file(c->ctx, (PCSTR)privateKey, SSL_FILETYPE_PEM) > 0)  &&
                            (SSL_CTX_check_private_key(c->ctx)  > 0)                                        ))
                    {
                        if (!result->doAccept())
                        {
                            ERR_print_errors_fp(stderr);
                            SAFEDELETE(result);
                        }
                    }
                    else
                    {
                        ERR_print_errors_fp(stderr);
                        SAFEDELETE(result);
                    }
                }
                else
                {
                    ERR_print_errors_fp(stderr);
                    SAFEDELETE(result);
                }
            }
            else
            {
                ERR_print_errors_fp(stderr);
                SAFEDELETE(result);
            }
        }
        else
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSslWrapper::doAccept
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXHttpSslWrapper::doAccept()
{
    BOOL            result      = false;
    PSSLCONTEXT     c           = (PSSLCONTEXT)context;
    X509*               client_cert;
//    PSTR                str;

    ENTER(true);

    if ((c->ssl = SSL_new (c->ctx)) != nil)
    {
        c->fd = getDescriptor();

        if (SSL_set_fd(c->ssl, c->fd) > 0)
        {
            if (SSL_accept(c->ssl) > 0)
            {
                //printf ("SSL connection using %s\n", SSL_get_cipher (c->ssl));

                // Get client's certificate (note: beware of dynamic allocation)
                if ((client_cert = SSL_get_peer_certificate (c->ssl)) != nil)
                {
//                    printf ("Client certificate:\n");
//                    if ((str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0)) != nil)
//                    {
//                          printf ("\t subject: %s\n", str);
//                          OPENSSL_free (str);
//                    }
//
//                    if ((str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0)) != nil)
//                    {
//                          printf ("\t issuer: %s\n", str);
//                          OPENSSL_free (str);
//                    }

                    X509_free(client_cert);
                }
                else
                {
                    // printf("NO CLIENT CERT!!!!\n");
                }

                result = true;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSslWrapper::globalInit
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXHttpSslWrapper::globalInit()
{
    BOOL            result          = false;
    int             i;

    ENTER(true);

    SSL_load_error_strings();
    ERR_load_crypto_strings();

    OpenSSL_add_all_algorithms();
    SSLeay_add_ssl_algorithms();

//    OPENSSL_config(NULL);

    if ((mutex_buf = (MUTEX_TYPE*)malloc(CRYPTO_num_locks() * sizeof(MUTEX_TYPE))) != nil)
    {
        for(i = 0;  i < CRYPTO_num_locks();  i++)
        {
            MUTEX_SETUP(mutex_buf[i]);
        }

        CRYPTO_set_id_callback(id_function);
        CRYPTO_set_locking_callback(locking_function);
        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttp::sslGlobalDeinit
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void AXHttpSslWrapper::globalDeinit()
{
    int             i;

    ENTER(true);

    if(mutex_buf)
    {
        CRYPTO_set_id_callback(NULL);
        CRYPTO_set_locking_callback(NULL);

        for(i = 0;  i < CRYPTO_num_locks();  i++)
        {
          MUTEX_CLEANUP(mutex_buf[i]);
        }

        free(mutex_buf);
        mutex_buf = NULL;
    }

    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();

    QUIT;
}
//
//
//void handle_error(const char *file, int lineno, const char *msg)
//{
//  fprintf(stderr, "** %s:%d %s\n", file, lineno, msg);
//  ERR_print_errors_fp(stderr);
//  /* exit(-1); */
//}
AXHttpSslWrapper::~AXHttpSslWrapper(void)
{
    PSSLCONTEXT     c           = (PSSLCONTEXT)context;

    if (c)
    {
        if (c->ssl)
        {
            SSL_free (c->ssl);
        }

        if (c->ctx)
        {
            SSL_CTX_free(c->ctx);
        }

        FREE(c);
    }

    if (dev)
    {
        axdev_close(dev, (UINT)-1);
    }
}
// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXHttpSslWrapper::AXHttpSslWrapper
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
AXHttpSslWrapper::AXHttpSslWrapper()
{
    dev         = nil;
//    firstRead   = true;
    context     = nil;
}
// ***************************************************************************
// FUNCTION
//      AXHttpSslWrapper::read
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      INT   size --
//      UINT  TO   --
// RESULT
//      INT --
// ***************************************************************************
//INT AXHttpSslWrapper::read(PVOID          data,
//                           INT            size,
//                           UINT           TO)
//
//{
//    INT             result          = -1;
//    PSSLCONTEXT     c           = (PSSLCONTEXT)context;
//    UINT            cd          = 100; // (TO / 10) | 1;
//    INT             toRd        = 0;
//
//    ENTER(true);
//
//    if (firstRead)
//    {
//        toRd        = size;
//        firstRead   = false;
//    }
//    else
//    {
//        while (cd-- && ((toRd = SSL_pending(c->ssl)) == 0))
//        {
//            axsleep(10);
//        }
//    }
//
//    result = toRd ? SSL_read(c->ssl, data, MAC_MIN(toRd, size)) : 0;
//
//    if (result > 0)
//    {
//        *(((PU8)data) + result) = 0;
//
//        printf("SSL_read: '%s'\n", (PSTR)data);
//    }
//
//    RETURN(result);
//}
INT AXHttpSslWrapper::read(PVOID          data,
                           INT            size,
                           UINT           TO)

{
    INT             result          = -1;
    PSSLCONTEXT     c           = (PSSLCONTEXT)context;
    INT             toRd        = 0;

    ENTER(true);

    if ((TO == (UINT)-1) || _axsocket_select(c->fd, SELECT_READ, TO))
    {
        toRd = size;
    }

    result  = toRd ? SSL_read(c->ssl, data, MAC_MIN(toRd, size)) : 0;

    RETURN(result);
}

// ***************************************************************************
// FUNCTION
//      AXHttpSslWrapper::write
// PURPOSE
//
// PARAMETERS
//      PCVOID data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT --
// ***************************************************************************
INT AXHttpSslWrapper::write(PCVOID         data,
                            INT            size,
                            UINT           TO)
{
    PSSLCONTEXT     c           = (PSSLCONTEXT)context;

    return SSL_write(c->ssl, data, size);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSslWrapper::getDescriptor
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      INT --
// ***************************************************************************
INT AXHttpSslWrapper::getDescriptor()
{
    return axdev_ctl(dev, AXDEVCTL_GETFD, 0, 0);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSslWrapper::connect
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev         --
//      PVOID  certificate --
//      PVOID  privateKey  --
// RESULT
//      AXHttpSslWrapper * --
// ***************************************************************************
AXHttpSslWrapper * AXHttpSslWrapper::connect(PAXDEV         dev,
                                             PVOID          certificate,
                                             PVOID          privateKey)
{
    AXHttpSslWrapper *  result          = nil;
    PSSLCONTEXT         c;

    ENTER(true);

    if ((result = new AXHttpSslWrapper) != nil)
    {
        if ((c = CREATE(SSLCONTEXT)) != nil)
        {
            result->context  = c;
            result->dev         = dev;

            c->meth = SSLv23_client_method();

            if ((c->ctx = SSL_CTX_new (c->meth)) != nil)
            {
//                if (  !certificate ||
//                      (SSL_CTX_use_certificate_file(c->ctx, (PCSTR)certificate, SSL_FILETYPE_PEM) > 0))
//                {
//                    if (  !privateKey ||
//                          ( (SSL_CTX_use_PrivateKey_file(c->ctx, (PCSTR)privateKey, SSL_FILETYPE_PEM) > 0)  &&
//                            (SSL_CTX_check_private_key(c->ctx)  > 0)                                        ))
//                    {
                        if (!result->doConnect())
                        {
                            ERR_print_errors_fp(stderr);
                            SAFEDELETE(result);
                        }
//                    }
//                    else
//                    {
//                        ERR_print_errors_fp(stderr);
//                        SAFEDELETE(result);
//                    }
//                }
//                else
//                {
//                    ERR_print_errors_fp(stderr);
//                    SAFEDELETE(result);
//                }
            }
            else
            {
                ERR_print_errors_fp(stderr);
                SAFEDELETE(result);
            }
        }
        else
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSslWrapper::doConnect
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXHttpSslWrapper::doConnect()
{
    BOOL            result      = false;
    PSSLCONTEXT     c           = (PSSLCONTEXT)context;
    X509*               server_cert;
//    PSTR                str;

    ENTER(true);

    if ((c->ssl = SSL_new (c->ctx)) != nil)
    {
        c->fd = getDescriptor();

        if (SSL_set_fd(c->ssl, c->fd) > 0)
        {
            if (SSL_connect(c->ssl) > 0)
            {
                //printf ("SSL connection using %s\n", SSL_get_cipher (c->ssl));

                // Get client's certificate (note: beware of dynamic allocation)
                if ((server_cert = SSL_get_peer_certificate (c->ssl)) != nil)
                {
//                    printf ("Client certificate:\n");
//                    if ((str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0)) != nil)
//                    {
//                          printf ("\t subject: %s\n", str);
//                          OPENSSL_free (str);
//                    }
//
//                    if ((str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0)) != nil)
//                    {
//                          printf ("\t issuer: %s\n", str);
//                          OPENSSL_free (str);
//                    }

                    X509_free(server_cert);
                }
                else
                {
                    // printf("NO CLIENT CERT!!!!\n");
                }

                result = true;
            }
        }
    }

    RETURN(result);
}
