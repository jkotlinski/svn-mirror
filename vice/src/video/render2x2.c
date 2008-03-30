/*
 * render2x2.c - 2x2 renderers
 *
 * Written by
 *  John Selck <graham@cruise.de>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include "render2x2.h"
#include "types.h"


/* 16 color 2x2 renderers */

void render_08_2x2_04(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht,int doublescan)
{
	BYTE *tmpsrc;
	WORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast;
	WORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + xt;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	for (y=ys;y<(ys+height);y++)
	{
		tmpsrc=src;
		tmptrg=(WORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*((BYTE *)tmptrg)++=(BYTE)colortab[*tmpsrc++];
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=(WORD)colortab[*tmpsrc++];
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=(WORD)colortab[tmpsrc[0]];
				tmptrg[1]=(WORD)colortab[tmpsrc[1]];
				tmptrg[2]=(WORD)colortab[tmpsrc[2]];
				tmptrg[3]=(WORD)colortab[tmpsrc[3]];
				tmptrg[4]=(WORD)colortab[tmpsrc[4]];
				tmptrg[5]=(WORD)colortab[tmpsrc[5]];
				tmptrg[6]=(WORD)colortab[tmpsrc[6]];
				tmptrg[7]=(WORD)colortab[tmpsrc[7]];
				tmpsrc += 8;
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=(WORD)colortab[*tmpsrc++];
			}
			if (wlast)
			{
				*((BYTE *)tmptrg)=(BYTE)colortab[*tmpsrc];
			}
			if (y & 1) src += pitchs;
		}
		else
		{
			color=(WORD)colortab[0];
			if (wfirst)
			{
				*((BYTE *)tmptrg)++=(BYTE)color;
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=color;
				tmptrg[1]=color;
				tmptrg[2]=color;
				tmptrg[3]=color;
				tmptrg[4]=color;
				tmptrg[5]=color;
				tmptrg[6]=color;
				tmptrg[7]=color;
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=color;
			}
			if (wlast)
			{
				*((BYTE *)tmptrg)=(BYTE)color;
			}
		}
		trg += pitcht;
	}
}

void render_16_2x2_04(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht,int doublescan)
{
	BYTE *tmpsrc;
	DWORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast;
	DWORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt << 1);
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	for (y=ys;y<(ys+height);y++)
	{
		tmpsrc=src;
		tmptrg=(DWORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*((WORD *)tmptrg)++=(WORD)colortab[*tmpsrc++];
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=colortab[*tmpsrc++];
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=colortab[tmpsrc[0]];
				tmptrg[1]=colortab[tmpsrc[1]];
				tmptrg[2]=colortab[tmpsrc[2]];
				tmptrg[3]=colortab[tmpsrc[3]];
				tmptrg[4]=colortab[tmpsrc[4]];
				tmptrg[5]=colortab[tmpsrc[5]];
				tmptrg[6]=colortab[tmpsrc[6]];
				tmptrg[7]=colortab[tmpsrc[7]];
				tmpsrc += 8;
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=colortab[*tmpsrc++];
			}
			if (wlast)
			{
				*((WORD *)tmptrg)=(WORD)colortab[*tmpsrc];
			}
			if (y & 1) src += pitchs;
		}
		else
		{
			color=colortab[0];
			if (wfirst)
			{
				*((WORD *)tmptrg)++=(WORD)color;
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=color;
				tmptrg[1]=color;
				tmptrg[2]=color;
				tmptrg[3]=color;
				tmptrg[4]=color;
				tmptrg[5]=color;
				tmptrg[6]=color;
				tmptrg[7]=color;
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=color;
			}
			if (wlast)
			{
				*((WORD *)tmptrg)=(WORD)color;
			}
		}
		trg += pitcht;
	}
}

void render_24_2x2_04(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht,int doublescan)
{
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast;
	register DWORD color;
	register DWORD tcolor;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt * 3);
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=4-((int)trg & 3);			/* alignment: 4 pixels*/
		wfast =(width - wstart) >> 2;		/* fast loop for 4 pixel segments*/
		wend  =(width - wstart) & 0x03;		/* do not forget the rest*/
	}
	for (y=ys;y<(ys+height);y++)
	{
		tmpsrc=src;
		tmptrg=trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				color=colortab[*tmpsrc++];
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
			}
			for (x=0;x<wstart;x++)
			{
				color=colortab[*tmpsrc++];
				tcolor=color;
				tmptrg[0]=(BYTE)color;
				color >>= 8;
				tmptrg[1]=(BYTE)color;
				color >>= 8;
				tmptrg[2]=(BYTE)color;
				tmptrg[3]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[4]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[5]=(BYTE)tcolor;
				tmptrg += 6;
			}
			for (x=0;x<wfast;x++)
			{
				color=colortab[tmpsrc[0]];
				tcolor=color;
				tmptrg[0]=(BYTE)color;
				color >>= 8;
				tmptrg[1]=(BYTE)color;
				color >>= 8;
				tmptrg[2]=(BYTE)color;
				tmptrg[3]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[4]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[5]=(BYTE)tcolor;
				color=colortab[tmpsrc[1]];
				tcolor=color;
				tmptrg[6]=(BYTE)color;
				color >>= 8;
				tmptrg[7]=(BYTE)color;
				color >>= 8;
				tmptrg[8]=(BYTE)color;
				tmptrg[9]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[10]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[11]=(BYTE)tcolor;
				color=colortab[tmpsrc[2]];
				tcolor=color;
				tmptrg[12]=(BYTE)color;
				color >>= 8;
				tmptrg[13]=(BYTE)color;
				color >>= 8;
				tmptrg[14]=(BYTE)color;
				tmptrg[15]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[16]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[17]=(BYTE)tcolor;
				color=colortab[tmpsrc[3]];
				tcolor=color;
				tmptrg[18]=(BYTE)color;
				color >>= 8;
				tmptrg[19]=(BYTE)color;
				color >>= 8;
				tmptrg[20]=(BYTE)color;
				tmptrg[21]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[22]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[23]=(BYTE)tcolor;
				tmpsrc += 4;
				tmptrg += 24;
			}
			for (x=0;x<wend;x++)
			{
				color=colortab[*tmpsrc++];
				tcolor=color;
				tmptrg[0]=(BYTE)color;
				color >>= 8;
				tmptrg[1]=(BYTE)color;
				color >>= 8;
				tmptrg[2]=(BYTE)color;
				tmptrg[3]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[4]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[5]=(BYTE)tcolor;
				tmptrg += 6;
			}
			if (wlast)
			{
				color=colortab[*tmpsrc];
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg=(BYTE)color;
			}
			if (y & 1) src += pitchs;
		}
		else
		{
			if (wfirst)
			{
				*tmptrg++=0;
				*tmptrg++=0;
				*tmptrg++=0;
			}
			for (x=0;x<wstart;x++)
			{
				tmptrg[0]=0;
				tmptrg[1]=0;
				tmptrg[2]=0;
				tmptrg[3]=0;
				tmptrg[4]=0;
				tmptrg[5]=0;
				tmptrg += 6;
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=0;
				tmptrg[1]=0;
				tmptrg[2]=0;
				tmptrg[3]=0;
				tmptrg[4]=0;
				tmptrg[5]=0;
				tmptrg[6]=0;
				tmptrg[7]=0;
				tmptrg[8]=0;
				tmptrg[9]=0;
				tmptrg[10]=0;
				tmptrg[11]=0;
				tmptrg[12]=0;
				tmptrg[13]=0;
				tmptrg[14]=0;
				tmptrg[15]=0;
				tmptrg[16]=0;
				tmptrg[17]=0;
				tmptrg[18]=0;
				tmptrg[19]=0;
				tmptrg[20]=0;
				tmptrg[21]=0;
				tmptrg[22]=0;
				tmptrg[23]=0;
				tmptrg += 24;
			}
			for (x=0;x<wend;x++)
			{
				tmptrg[0]=0;
				tmptrg[1]=0;
				tmptrg[2]=0;
				tmptrg[3]=0;
				tmptrg[4]=0;
				tmptrg[5]=0;
				tmptrg += 6;
			}
			if (wlast)
			{
				*tmptrg++=0;
				*tmptrg++=0;
				*tmptrg=0;
			}
		}
		trg += pitcht;
	}
}

void render_32_2x2_04(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht,int doublescan)
{
	BYTE *tmpsrc;
	DWORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast;
	register DWORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt << 2);
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	for (y=ys;y<(ys+height);y++)
	{
		tmpsrc=src;
		tmptrg=(DWORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*tmptrg++=colortab[*tmpsrc++];
			}
			for (x=0;x<wstart;x++)
			{
				color=colortab[*tmpsrc++];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				color=colortab[tmpsrc[0]];
				tmptrg[0]=color;
				tmptrg[1]=color;
				color=colortab[tmpsrc[1]];
				tmptrg[2]=color;
				tmptrg[3]=color;
				color=colortab[tmpsrc[2]];
				tmptrg[4]=color;
				tmptrg[5]=color;
				color=colortab[tmpsrc[3]];
				tmptrg[6]=color;
				tmptrg[7]=color;
				color=colortab[tmpsrc[4]];
				tmptrg[8]=color;
				tmptrg[9]=color;
				color=colortab[tmpsrc[5]];
				tmptrg[10]=color;
				tmptrg[11]=color;
				color=colortab[tmpsrc[6]];
				tmptrg[12]=color;
				tmptrg[13]=color;
				color=colortab[tmpsrc[7]];
				tmptrg[14]=color;
				tmptrg[15]=color;
				tmpsrc += 8;
				tmptrg += 16;
			}
			for (x=0;x<wend;x++)
			{
				color=colortab[*tmpsrc++];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg=colortab[*tmpsrc];
			}
			if (y & 1) src += pitchs;
		}
		else
		{
			color=colortab[0];
			if (wfirst)
			{
				*tmptrg++=color;
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=color;
				tmptrg[1]=color;
				tmptrg[2]=color;
				tmptrg[3]=color;
				tmptrg[4]=color;
				tmptrg[5]=color;
				tmptrg[6]=color;
				tmptrg[7]=color;
				tmptrg[8]=color;
				tmptrg[9]=color;
				tmptrg[10]=color;
				tmptrg[11]=color;
				tmptrg[12]=color;
				tmptrg[13]=color;
				tmptrg[14]=color;
				tmptrg[15]=color;
				tmptrg += 16;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg=color;
			}
		}
		trg += pitcht;
	}
}

/*****************************************************************************/
/*****************************************************************************/

/* 256 color 2x2 renderers */

void render_08_2x2_08(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht,int doublescan)
{
	BYTE *pre;
	BYTE *tmppre;
	BYTE *tmpsrc;
	WORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast;
	register DWORD rcolor;
	register WORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + xt;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	pre = src-pitchs-1;
	for (y=ys;y<(ys+height);y++)
	{
		tmppre=pre;
		tmpsrc=src;
		tmptrg=(WORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*((BYTE *)tmptrg)++=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			for (x=0;x<wfast;x++)
			{
				rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
				tmptrg[0]=(WORD)colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[1]=(WORD)colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[2]=(WORD)colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[3]=(WORD)colortab[(BYTE)rcolor];
				rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
				tmptrg[4]=(WORD)colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[5]=(WORD)colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[6]=(WORD)colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[7]=(WORD)colortab[(BYTE)rcolor];
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			if (wlast)
			{
				*((BYTE *)tmptrg)=(BYTE)colortab[*tmpsrc | (*tmppre << 4)];
			}
			if (y & 1)
			{
				pre = src-1;
				src += pitchs;
			}
		}
		else
		{
			color=(WORD)colortab[0];
			if (wfirst)
			{
				*((BYTE *)tmptrg)++=(BYTE)color;
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=color;
				tmptrg[1]=color;
				tmptrg[2]=color;
				tmptrg[3]=color;
				tmptrg[4]=color;
				tmptrg[5]=color;
				tmptrg[6]=color;
				tmptrg[7]=color;
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=color;
			}
			if (wlast)
			{
				*((BYTE *)tmptrg)=(BYTE)color;
			}
		}
		trg += pitcht;
	}
}

void render_16_2x2_08(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht,int doublescan)
{
	BYTE *pre;
	BYTE *tmppre;
	BYTE *tmpsrc;
	DWORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast;
	register DWORD rcolor;
	DWORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt << 1);
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	pre = src-pitchs-1;
	for (y=ys;y<(ys+height);y++)
	{
		tmppre=pre;
		tmpsrc=src;
		tmptrg=(DWORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*((WORD *)tmptrg)++=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			for (x=0;x<wfast;x++)
			{
				rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
				tmptrg[0]=colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[1]=colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[2]=colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[3]=colortab[(BYTE)rcolor];
				rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
				tmptrg[4]=colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[5]=colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[6]=colortab[(BYTE)rcolor];
				rcolor >>= 8;
				tmptrg[7]=colortab[(BYTE)rcolor];
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			if (wlast)
			{
				*((WORD *)tmptrg)=(WORD)colortab[*tmpsrc | (*tmppre << 4)];
			}
			if (y & 1)
			{
				pre = src-1;
				src += pitchs;
			}
		}
		else
		{
			color=colortab[0];
			if (wfirst)
			{
				*((WORD *)tmptrg)++=(WORD)color;
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=color;
				tmptrg[1]=color;
				tmptrg[2]=color;
				tmptrg[3]=color;
				tmptrg[4]=color;
				tmptrg[5]=color;
				tmptrg[6]=color;
				tmptrg[7]=color;
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=color;
			}
			if (wlast)
			{
				*((WORD *)tmptrg)=(WORD)color;
			}
		}
		trg += pitcht;
	}
}

void render_24_2x2_08(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht,int doublescan)
{
	BYTE *pre;
	BYTE *tmppre;
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast;
	register DWORD rcolor;
	register DWORD color;
	register DWORD tcolor;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt * 3);
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=4-((int)trg & 3);			/* alignment: 4 pixels*/
		wfast =(width - wstart) >> 2;		/* fast loop for 4 pixel segments*/
		wend  =(width - wstart) & 0x03;		/* do not forget the rest*/
	}
	pre = src-pitchs-1;
	for (y=ys;y<(ys+height);y++)
	{
		tmppre=pre;
		tmpsrc=src;
		tmptrg=trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
			}
			for (x=0;x<wstart;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				tcolor=color;
				tmptrg[0]=(BYTE)color;
				color >>= 8;
				tmptrg[1]=(BYTE)color;
				color >>= 8;
				tmptrg[2]=(BYTE)color;
				tmptrg[3]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[4]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[5]=(BYTE)tcolor;
				tmptrg += 6;
			}
			for (x=0;x<wfast;x++)
			{
				rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
				color=colortab[(BYTE)rcolor];
				tcolor=color;
				tmptrg[0]=(BYTE)color;
				color >>= 8;
				tmptrg[1]=(BYTE)color;
				color >>= 8;
				tmptrg[2]=(BYTE)color;
				tmptrg[3]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[4]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[5]=(BYTE)tcolor;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				tcolor=color;
				tmptrg[6]=(BYTE)color;
				color >>= 8;
				tmptrg[7]=(BYTE)color;
				color >>= 8;
				tmptrg[8]=(BYTE)color;
				tmptrg[9]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[10]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[11]=(BYTE)tcolor;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				tcolor=color;
				tmptrg[12]=(BYTE)color;
				color >>= 8;
				tmptrg[13]=(BYTE)color;
				color >>= 8;
				tmptrg[14]=(BYTE)color;
				tmptrg[15]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[16]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[17]=(BYTE)tcolor;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				tcolor=color;
				tmptrg[18]=(BYTE)color;
				color >>= 8;
				tmptrg[19]=(BYTE)color;
				color >>= 8;
				tmptrg[20]=(BYTE)color;
				tmptrg[21]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[22]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[23]=(BYTE)tcolor;
				tmptrg += 24;
			}
			for (x=0;x<wend;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				tcolor=color;
				tmptrg[0]=(BYTE)color;
				color >>= 8;
				tmptrg[1]=(BYTE)color;
				color >>= 8;
				tmptrg[2]=(BYTE)color;
				tmptrg[3]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[4]=(BYTE)tcolor;
				tcolor >>= 8;
				tmptrg[5]=(BYTE)tcolor;
				tmptrg += 6;
			}
			if (wlast)
			{
				color=colortab[*tmpsrc | (*tmppre << 4)];
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg=(BYTE)color;
			}
			if (y & 1)
			{
				pre = src-1;
				src += pitchs;
			}
		}
		else
		{
			if (wfirst)
			{
				*tmptrg++=0;
				*tmptrg++=0;
				*tmptrg++=0;
			}
			for (x=0;x<wstart;x++)
			{
				tmptrg[0]=0;
				tmptrg[1]=0;
				tmptrg[2]=0;
				tmptrg[3]=0;
				tmptrg[4]=0;
				tmptrg[5]=0;
				tmptrg += 6;
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=0;
				tmptrg[1]=0;
				tmptrg[2]=0;
				tmptrg[3]=0;
				tmptrg[4]=0;
				tmptrg[5]=0;
				tmptrg[6]=0;
				tmptrg[7]=0;
				tmptrg[8]=0;
				tmptrg[9]=0;
				tmptrg[10]=0;
				tmptrg[11]=0;
				tmptrg[12]=0;
				tmptrg[13]=0;
				tmptrg[14]=0;
				tmptrg[15]=0;
				tmptrg[16]=0;
				tmptrg[17]=0;
				tmptrg[18]=0;
				tmptrg[19]=0;
				tmptrg[20]=0;
				tmptrg[21]=0;
				tmptrg[22]=0;
				tmptrg[23]=0;
				tmptrg += 24;
			}
			for (x=0;x<wend;x++)
			{
				tmptrg[0]=0;
				tmptrg[1]=0;
				tmptrg[2]=0;
				tmptrg[3]=0;
				tmptrg[4]=0;
				tmptrg[5]=0;
				tmptrg += 6;
			}
			if (wlast)
			{
				*tmptrg++=0;
				*tmptrg++=0;
				*tmptrg=0;
			}
		}
		trg += pitcht;
	}
}

void render_32_2x2_08(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht,int doublescan)
{
	BYTE *pre;
	BYTE *tmppre;
	BYTE *tmpsrc;
	DWORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast;
	register DWORD rcolor;
	register DWORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt << 2);
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=4-((int)trg & 3);			/* alignment: 4 pixels*/
		wfast =(width - wstart) >> 2;		/* fast loop for 4 pixel segments*/
		wend  =(width - wstart) & 0x03;		/* do not forget the rest*/
	}
	pre = src-pitchs-1;
	for (y=ys;y<(ys+height);y++)
	{
		tmppre=pre;
		tmpsrc=src;
		tmptrg=(DWORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*tmptrg++=colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			for (x=0;x<wstart;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
				color=colortab[(BYTE)rcolor];
				tmptrg[0]=color;
				tmptrg[1]=color;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				tmptrg[2]=color;
				tmptrg[3]=color;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				tmptrg[4]=color;
				tmptrg[5]=color;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				tmptrg[6]=color;
				tmptrg[7]=color;
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg=colortab[*tmpsrc | (*tmppre << 4)];
			}
			if (y & 1)
			{
				pre = src-1;
				src += pitchs;
			}
		}
		else
		{
			color=colortab[0];
			if (wfirst)
			{
				*tmptrg++=color;
			}
			for (x=0;x<wstart;x++)
			{
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				tmptrg[0]=color;
				tmptrg[1]=color;
				tmptrg[2]=color;
				tmptrg[3]=color;
				tmptrg[4]=color;
				tmptrg[5]=color;
				tmptrg[6]=color;
				tmptrg[7]=color;
				tmptrg += 8;
			}
			for (x=0;x<wend;x++)
			{
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg=color;
			}
		}
		trg += pitcht;
	}
}

