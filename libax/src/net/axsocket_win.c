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

#include <signal.h>
#include <pvt_net.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifndef __GNUC__
static WSADATA      st_WSA_data;
#endif

// ***************************************************************************
// FUNCTION
//      a7socket_start
// PURPOSE
//      Start Sockets support
// PARAMETERS
//      none
// RESULT
//      TRUE if all is ok or FALSE if error has occured
// ***************************************************************************
BOOL a7socket_start(void)
{
#ifndef __GNUC__
    return !WSAStartup(MAKEWORD( 2, 2 ), &st_WSA_data);
#else
    return TRUE;
#endif
}
// ***************************************************************************
// FUNCTION
//      a7socket_stop
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      void  --
// ***************************************************************************
void a7socket_stop(void)
{
#ifndef __GNUC__
    WSACleanup();
#endif
}
// ***************************************************************************
// FUNCTION
//      _a7ssocket_connect_nb
// PURPOSE
//
// PARAMETERS
//      PSSOCKDEV           pst_dev   --
//      struct sockaddr *   pst_sa    --
//      UINT                u_sa_size --
//      UINT                u_TO_msec --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL _a7ssocket_connect_nb(PSSOCKDEV            pst_dev,
                           struct sockaddr *    pst_sa,
                           UINT                 u_sa_size,
                           UINT                 u_TO_msec)
{
    BOOL                b_result        = FALSE;
    ldiv_t              st_div;
    fd_set              fds;
    struct timeval      st_tv;

    ENTER(TRUE);

    if (connect(pst_dev->v_sock, pst_sa, u_sa_size) != SOCKET_ERROR)
    {
        st_div  = ldiv(u_TO_msec ?
                    u_TO_msec : DEF_DEFAULT_TIMEOUT, 1000);

        st_tv.tv_sec    = st_div.quot;
        st_tv.tv_usec   = st_div.rem * 1000;

        FD_ZERO(&fds);
        FD_SET(pst_dev->v_sock, &fds);
        select(pst_dev->v_sock + 1, NULL, &fds, NULL, &st_tv);

        b_result = FD_ISSET(pst_dev->v_sock, &fds);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _is_connected
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSSOCKDEV   pst_dev --
// RESULT
//      BOOL --
// ***************************************************************************
BOOL _a7socket_is_connected(SOCKET v_sock)
{
    BOOL                b_result    = FALSE;
    UINT                u_tmp;

    ENTER(v_sock != INVALID_SOCKET);

    b_result = TRUE;
    u_tmp    = TRUE;

    ioctlsocket(v_sock, FIONBIO, &u_tmp);

    if (recv(v_sock, (char*)&u_tmp, 1, MSG_PEEK) <= 0)
    {
        switch (WSAGetLastError())
        {
            case 0:
            case WSAECONNRESET:
            case WSAECONNABORTED:
                b_result = FALSE;
                break;
        }
    }

    u_tmp = FALSE;
    ioctlsocket(v_sock, FIONBIO , &u_tmp);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      a7ssocket_error
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      UINT  --
// ***************************************************************************
UINT a7socket_error(void)
{
    UINT            error  = 0;

    ENTER(TRUE);

    error = WSAGetLastError();

    RETURN(error);
}
