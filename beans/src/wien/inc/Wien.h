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

#ifndef __Wien_H__
#define __Wien_H__

#include <Display.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SWDEM_WRAP_CONTENT  0
#define SWDEM_FILL_PARENT   -1


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------



typedef enum
{
    WEALIGN_default         = 0,
    WEALIGN_left            = 0,
    WEALIGN_top             = 0,
    WEALIGN_right           = 1,
    WEALIGN_bottom          = 2,
    WEALIGN_vcenter         = 4,
    WEALIGN_hcenter         = 8,
    WEALIGN_center          = WEALIGN_vcenter | WEALIGN_hcenter,
} WEALIGN;


typedef struct __tag_WETEXTATTR
{
    PFONTFV1                font;
    int                     x;
    int                     y;
    int                     width;
    int                     height;
    WEALIGN                 align;
    int                     border;
    int                     fgColor;
    int                     bgColor;
    int                     fcBgColor;

    BOOL                    focused     : 1;
    BOOL                    disabled    : 1;
} WETEXTATTR, * PWETEXTATTR;

typedef struct __tag_WEDEMRECT
{
    int                     x;
    int                     y;
    UINT                    width;
    UINT                    height;
} WEDEMRECT, * PWEDEMRECT;



typedef struct __tag_SWDEMCALC
{
    WEDEMRECT               ori;
    WEDEMRECT               calc;
} SWDEMCALC, * PSWDEMCALC;



// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Wien
{
protected:
                        Display *               disp;

        UINT            calcWidth               (PWETEXTATTR    a,
                                                 PCSTR          text);

        void            calcAlign               (PWETEXTATTR    a,
                                                 SWDEMCALC &    dem);

public:
                        Wien           (Display &      disp)
                        {
                            this->disp = &disp;
                        }


        void            textBox                 (PWETEXTATTR    attr,
                                                 PCSTR          text);
};

#endif // #define __Wien_H__

