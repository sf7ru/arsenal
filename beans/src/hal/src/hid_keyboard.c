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

#include <hid_keyboard.h>
#include <string.h>

static char * _from_1e_shift  = "!@#$%^&*()";
static char * _from_2b        = "\t -=[]\\№;'`,./";
static char * _from_2d_shift  = "_+{}|#~:\"~<>?";


BOOL hidkbd_char_to_scancode(PHIDKBDREPORT     report,
                           U8             c)
{
#define SEARCH(str,begin,mod) else if ((found = strchr((str), c)) != nil) { m = (mod); k = (begin) + (found - (str)); }

    BOOL        result          = true;
    U8          m               = KEY_MOD_NONE;
    U8          k;
    char *      found;

    ENTER(true);

    if ((c >= '1') && (c <= '9'))
    {
        k = (U8)KEY_1 + (c - '1');
    }
    else if ( ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))
    {
        k = (U8)KEY_A + ((c | 0x20) - 'a');
        m = (c & 0x20) ? KEY_MOD_NONE : KEY_MOD_LSHIFT;
    }
    SEARCH(_from_1e_shift, 0x1e, KEY_MOD_LSHIFT)
    SEARCH(_from_2b,       0x2b, 0)
    SEARCH(_from_2d_shift, 0x2d, KEY_MOD_LSHIFT)
    else switch (c)
    {
        case '\n':
            k = KEY_ENTER;
            break;

        case '0':
            k = KEY_0;
            break;

        default:
            result = false;
            break;
    }

    if (result)
    {
        report->MODIFIER = m;
        report->KEYCODE1 = k;
    }


    RETURN(result);
}


