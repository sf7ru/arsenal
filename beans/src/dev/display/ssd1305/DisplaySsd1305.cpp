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


#include <DisplaySsd1305.h>
#include <customboard.h>
#include <string.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      DisplaySsd1305::DisplaySsd1305
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
DisplaySsd1305::DisplaySsd1305()
{
    hal     = &Hal::getInstance();
    
    mHeight = 64;
    mWidth  = 128;
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd1305::init
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL DisplaySsd1305::init()
{
    BOOL            result          = false;

    ENTER(true);
    hal->gpio.setMode(CBRD_DISP_CS);
    hal->gpio.setMode(CBRD_DISP_RESET);
    hal->gpio.setMode(CBRD_DISP_DC);

    hal->gpio.setPin(CBRD_DISP_RESET, 1);
    axdelay(1);

    reset();

    // Init sequence for 128x64 OLED module
    command(SSD1305_DISPLAYOFF);                    // 0xAE


    if (64 == mHeight)
    {
        page_offset = 0;
        column_offset = 0;

        command(SSD1305_SETLOWCOLUMN  | 0x4);  // low col = 0
        command(SSD1305_SETHIGHCOLUMN | 0x4);  // hi col = 0

    }
    else // 32
    {
        page_offset = 4;
        column_offset = 4;

        command(SSD1305_SETLOWCOLUMN  | 0x0);  // low col = 0
        command(SSD1305_SETHIGHCOLUMN | 0x0);  // hi col = 0
    }

//    command(SSD1305_MEMORYMODE);
//    command(0x02);

    command(SSD1305_SETSTARTLINE | 0x0); // line #0
    command(0x2E); //??
    command(SSD1305_SETCONTRAST);                   // 0x81
    command(0x32);
    command(SSD1305_SETBRIGHTNESS);                 // 0x82
    command(0x80);
    command(SSD1305_SEGREMAP | 0x01);
    command(SSD1305_NORMALDISPLAY);                 // 0xA6
    command(SSD1305_SETMULTIPLEX);                  // 0xA8
    command(0x3F); // 1/64
    command(SSD1305_MASTERCONFIG);
    command(0x8e); /* external vcc supply */
    command(SSD1305_COMSCANDEC);
    command(SSD1305_SETDISPLAYOFFSET);              // 0xD3
    command(0x40);
    command(SSD1305_SETDISPLAYCLOCKDIV);            // 0xD5
    command(0xf0);
    command(SSD1305_SETAREACOLOR);
    command(0x05);
    command(SSD1305_SETPRECHARGE);                  // 0xd9
    command(0xF1);
    command(SSD1305_SETCOMPINS);                    // 0xDA
    command(0x12);


    command(SSD1305_SETLUT);
    command(0x3F);
    command(0x3F);
    command(0x3F);
    command(0x3F);

//    command(SSD1305_SETRECT);               // Rectangle
//    command(0);                             // Col Start
//    command(0);                             // Row Start
//    command(mWidth - 1);                    // Col End
//    command(mHeight - 1);                   // Row End
//    command(0x00);                      // 2 Pixel color (black)

    axdelay(100);

    command(SSD1305_DISPLAYON);//--turn on oled panel

    result = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd1305::reset
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd1305::reset()
{
    ENTER(true);

    hal->gpio.setPin(CBRD_DISP_RESET, 0);
    axdelay(10);
    hal->gpio.setPin(CBRD_DISP_RESET, 1);
    axdelay(10);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd1305::setCommand
// PURPOSE
//
// PARAMETERS
//      BOOL value --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd1305::setModeCommand(BOOL value)
{
    hal->gpio.setPin(CBRD_DISP_DC, !value);
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd1305::command
// PURPOSE
//
// PARAMETERS
//      U8 cmd --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd1305::command(U8  cmd)
{
    ENTER(true);

    lockDevice();

    setModeCommand(true);
    write((PU8)&cmd, sizeof(cmd));

    unlockDevice();

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd1305::sendData
// PURPOSE
//
// PARAMETERS
//      PU8  data --
//      UINT size --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd1305::sendData(PU8            data,
                                 UINT           size)
{
    ENTER(true);

    lockDevice();

    setModeCommand(false);

    write(data, size);

    unlockDevice();

    QUIT;
}
/*
void DisplaySsd1305::applyBuffer(UINT           x,
                                 UINT           y,
                                 UINT           w,
                                 UINT           h)
{
    PU8     on      = mBuffer;
    U8      cmd     [ 3 ];
    U8      page    = (64 == mHeight) ? 0 : 4;

    cmd[1] = 0x00;
    cmd[2] = 0x10;

    for(; page < 8; page++)
    {
        cmd[0] = SSD1305_SETPAGESTART + page;

        lockDevice();
        longSelect(true);

        setModeCommand(true);
        write(cmd, sizeof(cmd));

        setModeCommand(false);
        write(on, mWidth);

        longSelect(false);
        unlockDevice();

        on += mWidth;
    }
}
*/

void DisplaySsd1305::applyBuffer(UINT           x,
                                 UINT           y,
                                 UINT           w,
                                 UINT           h)
{
    UINT window_x1 = x;
    UINT window_y1 = y;
    UINT window_x2 = x + w - 1;
    UINT window_y2 = y + h - 1;

    // uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
    U8 *ptr = mBuffer;
    U8 dc_byte = 0x40;
    // U8 pages = ((HEIGHT + 7) / 8);

    U8 bytes_per_page = mWidth;
    U8 maxbuff = 128;

    U8 first_page = window_y1 / 8;
    U8 last_page = (window_y2 + 7) / 8;
    U8 page_start = MAC_MIN((I16)bytes_per_page, window_x1);
    U8 page_end = MAC_MAX((I16)0, window_x2);

    lockDevice();
    longSelect(true);

    for (U8 p = first_page; p < last_page; p++)
    {
        U8 bytes_remaining = bytes_per_page;
        ptr = mBuffer + (U16)p * (U16)bytes_per_page;
        // fast forward to dirty rectangle beginning
        ptr += page_start;
        bytes_remaining -= page_start;
        // cut off end of dirty rectangle
        bytes_remaining -= (mWidth - 1) - page_end;

        U8 cmd[] = {U8(SSD1305_SETPAGESTART + p + page_offset),
                         U8(0x10 + ((page_start + column_offset) >> 4)),
                         U8((page_start + column_offset) & 0xF)};

        setModeCommand(true);
        write(cmd, sizeof(cmd));

        while (bytes_remaining) {
            U8 to_write = MAC_MIN(bytes_remaining, maxbuff);

            setModeCommand(false);
            write(ptr, to_write);

            ptr += to_write;
            bytes_remaining -= to_write;
        }
    }

    longSelect(false);
    unlockDevice();
}

void DisplaySsd1305::clearDisplay()
{
    command(0x24);                // Rectangle
    command(0);              // Col Start
    command(0);              // Row Start
    command(mWidth - 1);    // Col End
    command(mHeight - 1);    // Row End
    command(0x00);                // 2 Pixel color (black)
}
void DisplaySsd1305::print(PFONTFV1       font,
                              int            x,
                              int            y,
                              int            w,
                              int            h,
                              int            color,
                              PCSTR          message)
{
//    int             i;
//    int             iFontOff;
//    int             tx;
//    int             ty;
//    unsigned char   ucMask;
//    unsigned char   c;
//    unsigned char*  src;
//    unsigned char*  dest;
//
//    U8      cx      = font->width;
//    U8*     pFont   = (PU8)(font + 1);
//
//    int     dy      = font->height / 8;
//
//    if (dy % 8)
//        dy++;
//
//    i = 0;
//    if (y > mHeight-(font->height-1)) return; // will write past the bottom
//
//    while (x < mWidth-(font->width-1) && message[i] != 0)
//    {
//        c               = (unsigned char)message[i];
//
//        if (  (c >= font->bnk1offset)                   &&
//              (c < (font->bnk1offset + font->bnk1size)) )
//        {
//            iFontOff = (int)(c - font->bnk1offset) * cx * dy;
//        }
//        else if (  font->bnk2offset                       &&
//                   (c >= font->bnk2offset)                &&
//                   (c < (font->bnk2offset + font->bnk2size)) )
//        {
//            iFontOff = (int)((c - font->bnk2offset) + font->bnk1size) * cx * dy;
//        }
//        else
//            iFontOff = -1;
//
//        if (iFontOff >= 0)
//        {
//            src    = &pFont[iFontOff];
//            dest   = &mBuffer[(y * mPitch) + (x / 2)];
//            ucMask = 1;
//
//            for (ty = 0; ty < font->height; ty++)
//            {
//                if (ty && !(ty % 8))
//                {
//                    src += font->width;
//                    ucMask = 1;
//                }
//                for (tx=0; tx<cx; tx+=2)
//                {
//                    if (src[tx] & ucMask) // foreground
//                    {
//                        dest[0] &= 0xf;
//                        dest[0] |= (color << 4);
//                    }
//                    if (src[tx+1] & ucMask)
//                    {
//                        dest[0] &= 0xf0;
//                        dest[0] |= color;
//                    }
//                    dest++;
//                }
//                ucMask <<= 1;
//
//                dest -= cx / 2;
//                if (cx & 1)
//                    dest--;
//
//                dest += mPitch; // move to next line
//            } // for ty
//            x += cx;
//        }
//
//        i++;
//    }
}
void DisplaySsd1305::rect(int            x1,
                             int            y1,
                             int            width,
                             int            height,
                             int            ucColor)
{
    int     tx;
    int     ty;
    int     x2  = x1 + width - 1;
    int     y2  = y1 + height - 1;

    for (tx=x1; tx<x2; tx++)
    {
        setPixel(tx, y1, ucColor);
    }

    for (tx=x1; tx<x2; tx++)
    {
        setPixel(tx, y2, ucColor);
    }

    for (ty=y1; ty<y2; ty++)
    {
        setPixel(x1, ty, ucColor);
    }

    for (ty=y1; ty<y2; ty++)
    {
        setPixel(x2, ty, ucColor);
    }

}
void DisplaySsd1305::box(int            x1,
                            int            y1,
                            int            width,
                            int            height,
                            int            ucColor)
{
    U8*     pBuffer;
    int     x2  = x1 + width;
    int     y2  = y1 + height;

    for (int y = y1; y < y2; y++)
    {
        for (int x = x1; x < x2; x++)
        {
            //pBuffer = setPixel(x, y, pBuffer, ucColor);
        }
    }
}
void DisplaySsd1305::clear(int            color)
{
    memset(mBuffer, color ? 0xFF : 0x00, mBufferSize);
}

///////////////////////////////////////////////////////////////
/*
//----------------------------------------------------------------------------------------
// Очистка прямоугольной области дисплея
//----------------------------------------------------------------------------------------
void DisplaySsd1305::ClearDisplay(int xStart, int yStart, int xEnd, int yEnd)
{
        command(0x24);                // Rectangle
        command(xStart);              // Col Start
        command(yStart);              // Row Start
        command(xEnd);                // Col End
        command(yEnd);                // Row End
        command(0x00);                // 2 Pixel color (black)
        axdelay(5);                                    //
}


//-----------------------------------------------------------------------------
// Установка границ вывода StartPoint(xStart,yStart) left up
//                     EndPoint(xEnd,yEnd) right down
//-----------------------------------------------------------------------------
void DisplaySsd1305::Set_Window(int xStart, int yStart, int xEnd, int yEnd)
{
        command(0x15);           // Set Column Address
        command(xStart);         // Begin
        command(xEnd);           // End

        command(0x75);           // Set Row Address
        command(yStart);                 // Begin
        command(yEnd);           // End
}
//  Вывод картинок
// ------------------------------------------------------------------
void DisplaySsd1305::PaintPic(char *Ptr, unsigned char column, unsigned char row)
{
  unsigned char CharCol_1=0, CharCol_2=0;
  unsigned char char_r_y_p12, char_r_y_p34, char_r_y_p56;
  unsigned char pz, y,x,z,tmp;
  unsigned int pos,bitmode,hoehe,breite;
  tmp = column;
  pos = 0;

  bitmode = pgm_read_byte(&Ptr[pos++]);
  hoehe = pgm_read_byte(&Ptr[pos++]);
  breite = pgm_read_byte(&Ptr[pos++]);
  if (bitmode==1)
  {
    for (z=0; z<6; z++)
    {
      for (x=0; x<42; x++)
      {
        CharCol_1 = pgm_read_byte(&Ptr[pos++]);
        CharCol_2 = pgm_read_byte(&Ptr[pos++]);
        Set_Window(column+7, 8*row, column+7, 8*row+7);

        pz = 1;                                                  //pixelzeile
        for (y=0; y<8; y++)
        {
          char_r_y_p12 = char_r_y_p34 = char_r_y_p56 = 0;
          if (CharCol_1 & pz) { char_r_y_p12 |= 0xF0; }
          if (CharCol_2 & pz) { char_r_y_p12 |= 0x0F; }
          OLED_WriteData(char_r_y_p12);
          pz = pz << 1;
        }

        column++;
      } // for < 42
      column=tmp;
      row++;
    } // z 10 Zeilen
  }
  else                       // bitmode: 4
  {
    pos = (hoehe * breite)/2-1+3;
    for (z=0; z<hoehe; z++)  // Zeilen
    {
        Set_Window(0+7, z, (breite/2-1)+7, z);

      for (y=0; y<(breite/2); y++)
      {
        tmp = pgm_read_byte(&Ptr[pos--]);
        x = tmp>>4;
        tmp = tmp<<4;
        tmp = tmp+x;
        OLED_WriteData(tmp);
      }

    }
  }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Divided by 2)
//    c: Row Address of Start
//    d: Row Address of End
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void DisplaySsd1305::Show_Pattern(char *Data_Pointer, int xStart, int yStart, int xEnd, int yEnd)
{
        char *Src_Pointer;
        unsigned char i,j;

        Src_Pointer=Data_Pointer;
        Set_Window(xStart, yStart, xEnd, yEnd);

        for(i=0;i<(yEnd-yStart+1);i++)
        {
                for(j=0;j<(xEnd-xStart+1);j++)
                {
                        OLED_WriteData(*Src_Pointer);
                        Src_Pointer++;
                }
        }
}


// ------------------------------------------------------------------
// Вывод символов 5x7
// ------------------------------------------------------------------
void DisplaySsd1305::DrawChar(char Sign, char Col, char Row,  char Contrast)
{
        unsigned char CharCol_1, CharCol_2, CharCol_3, CharCol_4, CharCol_5;
        unsigned char char_r_y_p12, char_r_y_p34, char_r_y_p56;
        unsigned char pz, y;
        unsigned char inverse;
        unsigned int pos;

        if ((Sign<0x20) || (Sign>0xC7)) { Sign = 0x20; }
        pos = 5*(Sign-0x20);

        CharCol_1 = pgm_read_byte(&code5x7[pos++]);
        CharCol_2 = pgm_read_byte(&code5x7[pos++]);
        CharCol_3 = pgm_read_byte(&code5x7[pos++]);
        CharCol_4 = pgm_read_byte(&code5x7[pos++]);
        CharCol_5 = pgm_read_byte(&code5x7[pos++]);

        Set_Window(3*Col, 8*Row, 3*Col+2, 8*Row+7);
        inverse = Contrast & 0xF0;
        Contrast = Contrast & 0x0F;

        pz = 1;                                                  //pixelsize
        if (!inverse)
        {
                for (y=0; y<8; y++)
                {
                        char_r_y_p12 = char_r_y_p34 = char_r_y_p56 = 0;
                        if (CharCol_1 & pz) { char_r_y_p12 |= Contrast << 4; }
                        if (CharCol_2 & pz) { char_r_y_p12 |= Contrast; }
                        OLED_WriteData(char_r_y_p12);
                        if (CharCol_3 & pz) { char_r_y_p34 |= Contrast << 4; }
                        if (CharCol_4 & pz) { char_r_y_p34 |= Contrast; }
                        OLED_WriteData(char_r_y_p34);
                        if (CharCol_5 & pz) { char_r_y_p56 = Contrast << 4; }
                        OLED_WriteData(char_r_y_p56);
                        pz = pz << 1;
                }
        }
        else
        {
                for (y=0; y<8; y++)
                {
                        char_r_y_p12 = char_r_y_p34 = char_r_y_p56 = 0;
                        if ((~CharCol_1) & pz) { char_r_y_p12 |= Contrast << 4; }
                        if ((~CharCol_2) & pz) { char_r_y_p12 |= Contrast; }
                        OLED_WriteData(char_r_y_p12);
                        if ((~CharCol_3) & pz) { char_r_y_p34 |= Contrast << 4; }
                        if ((~CharCol_4) & pz) { char_r_y_p34 |= Contrast; }
                        OLED_WriteData(char_r_y_p34);
                        if ((~CharCol_5) & pz) { char_r_y_p56 = Contrast << 4; }
                        char_r_y_p56 |= Contrast;
                        OLED_WriteData(char_r_y_p56);
                        pz = pz << 1;
                }

        }
}
// ------------------------------------------------------------------
// Вывод символов 11x16
// ------------------------------------------------------------------
void DisplaySsd1305::DrawChar2(char Sign, char Col, char Row,  char Contrast)
{
        unsigned char CharCol_1, CharCol_2, CharCol_3, CharCol_4, CharCol_5, CharCol_6, CharCol_7, CharCol_8, CharCol_9, CharCol_10, CharCol_11;
        unsigned char char_r_y_p12, char_r_y_p34, char_r_y_p56, char_r_y_p78, char_r_y_p910, char_r_y_p1112;
        unsigned char pz, y, i;
        unsigned int pos;

        if ((Sign<0x20) || (Sign>0xC7)) { Sign = 0x20; }
        Sign = Sign - '.';
        pos = 22*(Sign);

//      Set_Window(5*Col, 16*Row, 5*Col+5, 16*Row+15);
        Set_Window(5*Col, 8*Row, 5*Col+5, 8*Row+15);
//      inverse = Contrast & 0xF0;
        Contrast = Contrast & 0x0F;
        for (i=0; i<2; i++) {
                CharCol_1 = pgm_read_byte(&numchar[pos++]);
                CharCol_2 = pgm_read_byte(&numchar[pos++]);
                CharCol_3 = pgm_read_byte(&numchar[pos++]);
                CharCol_4 = pgm_read_byte(&numchar[pos++]);
                CharCol_5 = pgm_read_byte(&numchar[pos++]);
                CharCol_6 = pgm_read_byte(&numchar[pos++]);
                CharCol_7 = pgm_read_byte(&numchar[pos++]);
                CharCol_8 = pgm_read_byte(&numchar[pos++]);
                CharCol_9 = pgm_read_byte(&numchar[pos++]);
                CharCol_10 = pgm_read_byte(&numchar[pos++]);
                CharCol_11 = pgm_read_byte(&numchar[pos++]);
                pz = 1;                                                  //pixelsize
                for (y=0; y<8; y++)
                {
                        char_r_y_p12 = char_r_y_p34 = char_r_y_p56 = char_r_y_p78 = char_r_y_p910 = char_r_y_p1112 = 0;
                        if (CharCol_1 & pz) { char_r_y_p12 |= Contrast << 4; }
                        if (CharCol_2 & pz) { char_r_y_p12 |= Contrast; }
                        OLED_WriteData(char_r_y_p12);
                        if (CharCol_3 & pz) { char_r_y_p34 |= Contrast << 4; }
                        if (CharCol_4 & pz) { char_r_y_p34 |= Contrast; }
                        OLED_WriteData(char_r_y_p34);
                        if (CharCol_5 & pz) { char_r_y_p56 |= Contrast << 4; }
                        if (CharCol_6 & pz) { char_r_y_p56 |= Contrast; }
                        OLED_WriteData(char_r_y_p56);
                        if (CharCol_7 & pz) { char_r_y_p78 |= Contrast << 4; }
                        if (CharCol_8 & pz) { char_r_y_p78 |= Contrast; }
                        OLED_WriteData(char_r_y_p78);
                        if (CharCol_9 & pz) { char_r_y_p910 |= Contrast << 4; }
                        if (CharCol_10 & pz) { char_r_y_p910 |= Contrast; }
                        OLED_WriteData(char_r_y_p910);
                        if (CharCol_11 & pz) { char_r_y_p1112 |= Contrast << 4; }
                        OLED_WriteData(char_r_y_p1112);
                        pz = pz << 1;
                }
        }
}


//-----------------------------------------------------------------------------
// send a string to oled
//
// xPos: 0..16 (17 Spalten a 6 Pixel breit)
// yPos: 0.. 9 (10 Zeilen a 8 Pixel breit)
// Str:  max 17 Zeiche a
//-----------------------------------------------------------------------------
void DisplaySsd1305::WriteString(char Col, char Row, char *Str, char Contrast)
{
        while ((*Str) && (Col<21))
        {
                if ((*Str > 0x7F) && (*Str < 0xC0)) Col--;      // R
                DrawChar(*Str++, Col++, Row, Contrast);
        }
}

void DisplaySsd1305::WriteString2(char Col, char Row, char *Str, char Contrast)
{
        while ((*Str) && (Col<21))
        {
                if ((*Str > 0x7F) && (*Str < 0xC0)) Col--;      // R
                DrawChar2(*Str++, Col++, Row, Contrast);
        }
}

void DisplaySsd1305::WriteMeasuring(char *IzmVal, char *Razmernost, char *Val)
{
        uint8_t len1, len2;
        ClearDisplay(0,24,63,39);
        len1 = strlen(IzmVal)/2+1;
        len2 = (strlen(Val) + len1)*2 - 1;
        WriteString(1,4, IzmVal, 0x0F);
        WriteString2(len1,3, Val, 0x0F);
        WriteString(len2,4, Razmernost, 0x0F);
}

void DisplaySsd1305::WriteNum(char Col, char Row, int Num, char Len, char Contrast)
{
        char tmp10000, tmp1000, tmp100, tmp10, tmp1;

//      Num = abs(Num);
        tmp10000 = Num / 10000; Num -= tmp10000*10000;
        tmp1000 = Num / 1000;   Num -= tmp1000*1000;
        tmp100 = Num / 100;     Num -= tmp100*100;
        tmp10 = Num / 10;       Num -= tmp10*10;
        tmp1 = Num;

        tmp1 += 0x30;
        tmp10 += 0x30;
        tmp100 += 0x30;
        tmp1000 += 0x30;
        tmp10000 += 0x30;

        DrawChar(tmp10000, Col++, Row,  0xFF);
        DrawChar(tmp1000, Col++, Row,  0xFF);
        DrawChar(tmp100, Col++, Row,  0xFF);
        DrawChar(tmp10, Col++, Row,  0xFF);
        DrawChar(tmp1, Col++, Row,  0xFF);
}

void DisplaySsd1305::DrawSpectr(uint16_t* Sprectr)
{
int AvgVal;
int i,j;
int xOffs = 2;
int yOffs = 30;
int avgBins = FFT_LENGTH/110;
        for (i=0; i<((FFT_LENGTH/2)/(avgBins)); i++)
        {
                AvgVal = 0;
                for (j=0; j<avgBins; j++)
                {
                        AvgVal += Sprectr[i*avgBins+j];
                }
                AvgVal = AvgVal / 50;
                if (AvgVal > 30) AvgVal = 30;
                command(0x24);                // Rectangle
                command(xOffs + i);
                command(0x3E - AvgVal-1);
                command(xOffs + i);
                command(0x3E);
                command(0xFF);                // 2 pixel color

                command(0x24);                // Rectangle
                command(xOffs + i);
                command(yOffs);
                command(xOffs + i);
                command(0x3E - AvgVal);
                command(0x00);                // 2 pixel color (black)

        }

}

void DisplaySsd1305::SetContrast(char clevel) {
        command(0x81);//contrast
        command(clevel);//contrast val
}

void DisplaySsd1305::SetPhaze(char pha) {
        pha = (pha<<4)+1;
        command(0xB1);
        command(pha);
        command(0x05);
}

void DisplaySsd1305::Demo(void)
{
        unsigned int i;

//                ClearDisplay();
//                PaintPic(QRCode,3,0);  // QRCode
//                axdelay(2000);

                  ClearDisplay();

                  for(i=0;i<5;i++)
                  {
                    PaintPic(Eye_01,3,0);
                    axdelay(10);
                    PaintPic(Eye_02,3,0);
                    axdelay(10);
                    PaintPic(Eye_03,3,0);
                    axdelay(10);
                    PaintPic(Eye_04,3,0);
                    axdelay(10);
                    PaintPic(Eye_05,3,0);
                    axdelay(10);
                    PaintPic(Eye_04,3,0);
                    axdelay(10);
                    PaintPic(Eye_03,3,0);
                    axdelay(10);
                    PaintPic(Eye_02,3,0);
                    axdelay(10);
                    PaintPic(Eye_01,3,0);
                    axdelay(100);
                  }

}

*/

int getRotation()
{
    return 0;
}

#define grayoled_swap(a, b)                                                    \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

void DisplaySsd1305::setPixel(int            x,
                              int            y,
                              U8             color)
{
    if ((x >= 0) && (x < mWidth) && (y >= 0) && (y < mHeight))
    {
        // Pixel is in-bounds. Rotate coordinates if needed.
//        switch (getRotation()) {
//            case 1:
//                grayoled_swap(x, y);
//                x = WIDTH - x - 1;
//                break;
//            case 2:
//                x = WIDTH - x - 1;
//                y = HEIGHT - y - 1;
//                break;
//            case 3:
//                grayoled_swap(x, y);
//                y = HEIGHT - y - 1;
//                break;
//        }

        // adjust dirty window
        window_x1 = MAC_MIN(window_x1, x);
        window_y1 = MAC_MIN(window_y1, y);
        window_x2 = MAC_MIN(window_x2, x);
        window_y2 = MAC_MIN(window_y2, y);

        if (color)
        {
            mBuffer[x + (y / 8) * mWidth] |= (1 << (y & 7));
        }
        else
        {
            mBuffer[x + (y / 8) * mWidth] &= ~(1 << (y & 7));
        }
    }
}
