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

#include <Wien.h>
#include <string.h>
#include <customboard.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define CHECK_W(val)        (SWDEM_WRAP_CONTENT == (val) ? dem.ori.width : \
                            (SWDEM_FILL_PARENT  == (val) ? (disp->getWidth() - a->x) : \
                            (val)))

#define CHECK_H(val)        (SWDEM_WRAP_CONTENT == (val) ? dem.ori.height : \
                            (SWDEM_FILL_PARENT  == (val) ? (disp->getHeight() - a->y) : \
                            (val)))

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

UINT Wien::calcWidth(PWETEXTATTR    a,
                              PCSTR          text)
{
    return (UINT)(strlen(text) * a->font->width);
}
void Wien::calcAlign(PWETEXTATTR    a,
                              SWDEMCALC &    dem)
{
    dem.calc.width  = CHECK_W(a->width);
    dem.calc.height = CHECK_H(a->height);

    dem.calc.x = a->x;
    dem.calc.y = a->y;

    if (a->align & WEALIGN_hcenter)
    {
        if (dem.calc.width > dem.ori.width)
        {
            dem.calc.x = a->x + ((dem.calc.width - dem.ori.width) / 2);
        }
    }

    if (a->align & WEALIGN_vcenter)
    {
        if (dem.calc.height > dem.ori.height)
        {
            dem.calc.y += ((dem.calc.height - dem.ori.height) / 2);
        }
    }

    if (a->align & WEALIGN_right)
    {
        if (dem.calc.width > dem.ori.width)
        {
            dem.calc.x = a->x + (dem.calc.width - dem.ori.width);
        }
    }
}
void Wien::textBox(PWETEXTATTR    a,
                            PCSTR          text)
{
    SWDEMCALC       dem;
    dem.ori.width   = calcWidth(a, text);
    dem.ori.height  = a->font->height;

    calcAlign(a, dem);

    disp->box(a->x, a->y, dem.calc.width, dem.calc.height, a->focused ? a->fcBgColor : a->bgColor);

    disp->print(a->font, dem.calc.x, dem.calc.y, dem.calc.width, dem.calc.height, a->fgColor, text);

    if (a->border)
    {
        disp->rect(a->x, a->y, dem.calc.width, dem.calc.height, a->fgColor);
    }
}
