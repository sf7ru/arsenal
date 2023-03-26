// ***************************************************************************
// TITLE
//      Gets with timeout for UNIX
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: charset.c,v 1.2 2003/09/03 08:16:31 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <unistd.h>

#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <arsenal.h>
#include <axtime.h>
#include <axutils.h>
#include <pvt_unix.h>


// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

#define DEF_GETS_TIMEOUT        100


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

struct termios orig_termios;

//void reset_terminal_mode()
//{
//    tcsetattr(0, TCSANOW, &orig_termios);
//}

//void set_conio_terminal_mode()
//{
//    struct termios new_termios;
//
//    /* take two copies - one for now, one for later */
//    tcgetattr(0, &orig_termios);
//    memcpy(&new_termios, &orig_termios, sizeof(new_termios));
//
//    /* register cleanup handler, and set the new terminal mode */
//    atexit(reset_terminal_mode);
//    cfmakeraw(&new_termios);
//    tcsetattr(0, TCSANOW, &new_termios);
//}

// ***************************************************************************
// FUNCTION
//      axcon_getc
// PURPOSE
//
// PARAMETERS
//      UINT to --
// RESULT
//      int --
// ***************************************************************************
int axcon_getc(UINT               to)
{
    int     result          = 0;
    CHAR    buff;

    switch (result = _ax_nonblock_dev_read(STDIN_FILENO, &buff, sizeof(buff), to))
    {
        case 0:
        case -1:
            break;

        default:
            result = buff;
            break;
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      axcon_gets
// PURPOSE
//
// PARAMETERS
//      PSTR psz_string --
//      UINT u_size     --
//      UINT flags      --
//      UINT TO         --
// RESULT
//      UINT --
// ***************************************************************************
INT axcon_gets(PSTR          psz_string,
                UINT          u_size,
                UINT          flags,
                UINT          TO)
{
    INT                 result              = -1;
    PSTR                psz_on_string       = psz_string;
    UINT                u_left              = u_size - 1; // For zero
    UINT                u_tries             = 0;
    UINT                u_timeout;
    int                 i_flags_fcntl;
    struct termios      st_ti;
    struct termios      st_nti;

    if (psz_string && u_size)
    {
// -------------------------- Prepare terminal -------------------------------

        if (tcgetattr(0, &st_ti) != -1)
        {
            memcpy(&st_nti, &st_ti, sizeof(struct termios));
            st_nti.c_lflag      &= ~ICANON;
            st_nti.c_cc[VMIN]    = 0;
            st_nti.c_cc[VTIME]   = 1;

            if (tcsetattr(0, TCSANOW, &st_nti) != -1)
            {
                if ((i_flags_fcntl = fcntl(0, F_GETFL)) != -1)
                {
                    if (fcntl(0, F_SETFL, i_flags_fcntl | O_NONBLOCK) != -1)
                    {
                        if (flags & AXCON_FL_FLUSH)
                        {
// ------------------------- Flush previon input -----------------------------

                            while (read(0, psz_string, 1) != -1)
                                ;
                        }

// ------------------------ Receviving characters ----------------------------

                        u_timeout   = TO / DEF_GETS_TIMEOUT;
                        result  = 0;

                        while (  u_left                 &&
                                 (u_tries < u_timeout)  &&
                                 (result != -1)     )
                        {
                            switch (read(0, psz_on_string, 1))
                            {
                                case -1:
                                case 0:
                                    axsleep(DEF_GETS_TIMEOUT);
                                    u_tries++;
                                    break;

                                default:
                                    //printf("RECEIVED '%c'\n", *psz_on_string);

                                    if (  (*psz_on_string != '\r')  &&
                                          (*psz_on_string != '\n')  )
                                    {
                                        psz_on_string++;
                                        result++;
                                        u_tries = 0;
                                    }
                                    else
                                        u_left = 0;
                                    break;
                            }
                        }

                        *psz_on_string = 0;

                        fcntl(0, F_SETFL, i_flags_fcntl & ~O_NONBLOCK);
                    }
                }

                tcsetattr(0, TCSANOW, &st_ti);
            }
        }
    }

    return result;
}
// {traceprofile}
