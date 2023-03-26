// ***************************************************************************
// TITLE
//
// PROJECT
//     conductor
// ***************************************************************************

#include <Display.h>
#include <math.h>

unsigned kChannels = 1;

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
// void myRect(SURFACE* surface, int x, int y, int x2, int y2) {
// 	myLine(surface,x,y,x2,y);
// 	myLine(surface,x2,y,x2,y2);
// 	myLine(surface,x2,y2,x,y2);
// 	myLine(surface,x,y2,x,y);
// }

void Display::setPitch(UINT width)
{
    mPitch = (UINT)ceil((float)mDepth / 8 * width);
}
