// ***************************************************************************
// TITLE
//
// PROJECT
//     conductor
// ***************************************************************************

#include <Display.h>
#include <math.h>
#include <string.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define PITCH(a)			(UINT)ceil((float)mDepth / 8 * (a));

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------


//unsigned kChannels = 1;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// THE EXTREMELY FAST LINE ALGORITHM Variation E (Addition Fixed Point PreCalc)
void Display::lineForAllDisplays(int x, int y, int x2, int y2, UINT c)
{
   	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=true;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (int j=0x8000+(x<<16);y<=longLen;++y) {
				setPixel(j >> 16, y, c);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (int j=0x8000+(x<<16);y>=longLen;--y) {
			setPixel(j >> 16, y, c);	
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (int j=0x8000+(y<<16);x<=longLen;++x) {
			setPixel(x,j >> 16, c);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (int j=0x8000+(y<<16);x>=longLen;--x) {
		setPixel(x,j >> 16, c);
		j-=decInc;
	}
}

// THE EXTREMELY FAST LINE ALGORITHM Variation E (Addition Fixed Point PreCalc Small Display)
// Small Display (256x256) resolution.
void Display::lineForSmallDisplays(int            x,
                                   int            y,
                                   int            x2,
                                   int            y2,
                                   UINT           color)
{
   	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=true;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 8) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (int j=0x80+(x<<8);y<=longLen;++y) {
				setPixel(j >> 8,y, color);	
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (int j=0x80+(x<<8);y>=longLen;--y) {
			setPixel(j >> 8,y, color);	
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (int j=0x80+(y<<8);x<=longLen;++x) {
			setPixel(x,j >> 8, color);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (int j=0x80+(y<<8);x>=longLen;--x) {
		setPixel(x,j >> 8, color);
		j-=decInc;
	}
}

void Display::setPitch(UINT width)
{
    mPitch = PITCH(width);
}
void Display::tiltedRect(int            x,
                         int            y,
                         UINT           width,
                         UINT           height,
                         int            angle,
                         UINT           color)
{
	float cx 	= x;
	float cy	= y;
	float w		= (float)width  / 2;
	float h		= (float)height / 2;
	float rot	= (float)angle  / 57.2957795131;

	float sxx =  cos(rot);
	float sxy = -sin(rot);
	float syx = -sin(rot);
	float syy = -cos(rot);

	float x1 = cx - sxx * w - syx * h;
	float x2 = cx + sxx * w - syx * h;
	float x3 = cx + sxx * w + syx * h;
	float x4 = cx - sxx * w + syx * h;
 
	float y1 = cy -sxy * w - syy * h;
	float y2 = cy +sxy * w - syy * h;
	float y3 = cy +sxy * w + syy * h;
	float y4 = cy -sxy * w + syy * h;
 

	line(x1, y1, x4, y4, color);
	line(x2, y2, x1, y1, color);
	line(x3, y3, x2, y2, color);
	line(x4, y4, x3, y3, color);
}
BOOL Display::drawT(PU8 			buff,
					int            x,
					int            y,
					UINT           width,
					UINT           height,
					int            tc)
{
    int 	color;
	UINT 	h;
	UINT 	w;
	UINT 	pitch		= PITCH(width);
	PU8 	onBuff		= mBuffer + PITCH(x);
	PU8 	onT;
	PU8 	onS;

	if ((y + height) <= mHeight)
	{
		h = height;
	}
	else
		h = mHeight - y;

	if ((x + width) <= mWidth)
	{
		w = width;
	}
	else
		w = mWidth - x;

    for (int j = 0; j < h; j++)
    {
		onT = onBuff + ((y + j) * mPitch);
		onS = buff + (j * pitch);

		for (int i = 0; i < w; i++)
		{
			color = *(onS + i);

			if (color != tc)
			{
				*(onT + i) = color;
			}
		}
    }

	return true;
}
BOOL Display::drawNonT(PU8 			  buff,
                   	   int            x,
                   	   int            y,
                   	   UINT           width,
                   	   UINT           height)
{
    int 	color;
	UINT 	h;
	UINT 	pitch		= PITCH(width);
	PU8 	onBuff		= mBuffer + PITCH(x);
	UINT 	pitchCopy;

	if ((y + height) <= mHeight)
	{
		h = height;
	}
	else
		h = mHeight - y;

	if ((x + width) <= mWidth)
	{
		pitchCopy = pitch;
	}
	else
		pitchCopy = PITCH(mWidth - x);

    for (int j = 0; j < h; j++)
    {
		memcpy(onBuff + ((y + j) * mPitch), buff + (j * pitch), pitchCopy);
    }

    return true;
}
