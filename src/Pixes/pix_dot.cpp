////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// This object is an based on the DotTV effect from EffecTV
// Originally written by Fukuchi Kentaro
// Copyright (C) 2001 FUKUCHI Kentaro                              
//
// ported by tigital@mac.com
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyleft  (l) 2001 IOhannes m zm�lnig
//    Copyleft (l) 2003 James Tittle
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "pix_dot.h"
#include <stdlib.h>

CPPEXTERN_NEW(pix_dot)

/////////////////////////////////////////////////////////
//
// pix_dot
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_dot :: pix_dot()
{
  myImage.xsize=myImage.ysize=myImage.csize=1;
  myImage.allocate(1);
  DOTDEPTH = 5;
  DOTMAX = (1<<DOTDEPTH);
  sharedbuffer = NULL;
  sharedbuffer_length =0;
  tail = 0;
  m_scale = 1;
  m_useScale=true;
  dots_width=dots_height=64;
  yuv_init();
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("scale"));
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_dot :: ~pix_dot()
{
    myImage.clear();
    sharedbuffer_length = 0;
    free(sharedbuffer);
    free(pattern);
}

/////////////////////////////////////////////////////////
// draw a basic Dot
//
/////////////////////////////////////////////////////////
void pix_dot :: makePattern()
{
  int i, x, y, c;
  int u, v;
  double p, q, r;
  unsigned int *pat;
  
  for (i=0; i<DOTMAX; i++)
  {
/* Generated pattern is a quadrant of a disk. */
    pat = pattern + (i+1) * dot_hsize * dot_hsize - 1;
    r = (0.2 * i / DOTMAX + 0.8) * dot_hsize;
    r = r*r;
    for(y=0; y<dot_hsize; y++) {
        for(x=0; x<dot_hsize; x++) {
            c = 0;
            for(u=0; u<4; u++) {
                p = (double)u/4.0 + y;
                p = p*p;
                for(v=0; v<4; v++) {
                    q = (double)v/4.0 + x;
                    if(p+q*q<r) {
                        c++;
                    }
                }
            }
            c = (c>15)?15:c;
            *pat-- = c<<20 | c<<12 | c<<4;
/* The upper left part of a disk is needed, but generated pattern is a bottom
 * right part. So I spin the pattern. */
        }
    }
  }
}

void pix_dot :: makePatternYUV()
{
  int i, x, y, c;
  int u, v;
  double p, q, r;
  unsigned int *pat;
  
  for (i=0; i<DOTMAX; i++)
  {
/* Generated pattern is a quadrant of a disk. */
    pat = pattern + (i+1) * dot_hsize * dot_hsize - 1;
    r = (0.2 * i / DOTMAX + 0.8) * dot_hsize;
    r = r*r;
    for(y=0; y<dot_hsize; y++) {
        for(x=0; x<dot_hsize; x++) {
            c = 0;
            for(u=0; u<4; u++) {
                p = (double)u/4.0 + y;
                p = p*p;
                for(v=0; v<4; v++) {
                    q = (double)v/4.0 + x;
                    if(p+q*q<r) {
                        c++;
                    }
                }
            }
            c = (c>15)?15:c;
            *pat-- = c<<20 | c<<12 | c<<4;
/* The upper left part of a disk is needed, but generated pattern is a bottom
 * right part. So I spin the pattern. */
        }
    }
  }
}

void pix_dot :: drawDot(int xx, int yy, unsigned char c, unsigned int *dest)
{
  int x, y;
  unsigned int *pat;

  c = (c>>(8-DOTDEPTH));
  pat = pattern + c * dot_hsize * dot_hsize;
  dest = dest + yy * dot_size * m_xsize + xx * dot_size;
  for(y=0; y<dot_hsize; y++) {
    for(x=0; x<dot_hsize; x++) {
      *dest++ = *pat++;
    }
    pat -= 2;
    for(x=0; x<dot_hsize-1; x++) {
      *dest++ = *pat--;
    }
    dest += m_xsize - dot_size + 1;
    pat += dot_hsize + 1;
  }
  pat -= dot_hsize*2;
  for(y=0; y<dot_hsize-1; y++) {
    for(x=0; x<dot_hsize; x++) {
      *dest++ = *pat++;
    }
    pat -= 2;
    for(x=0; x<dot_hsize-1; x++) {
      *dest++ = *pat--;
    }
    dest += m_xsize - dot_size + 1;
    pat += -dot_hsize + 1;
  }
}
/////////////////////////////////////////////////////////
// sizeMess
//
/////////////////////////////////////////////////////////
void pix_dot :: sizeMess(int width, int height)
{
#if 0
  dot_size = (width>0)?width:8;
  dot_hsize = (height>0)?height:8;
#else
  if(width>0)dots_width=width;
  else  error("pix_dot: width must be > 0!");
  if(height>0)dots_height=height;
  else  error("pix_dot: height must be > 0!");
  m_useScale=false;
  alreadyInit=0;
  myImage.setBlack();

#endif
}

unsigned char pix_dot :: inline_RGB2Y(int rgb)
{
	int i;

	i = R2Y[(rgb>>16)&0xff];
	i += G2Y[(rgb>>8)&0xff];
	i += B2Y[rgb&0xff];
	return i;
}

/////////////////////////////////////////////////////////
// processRGBAImage
//
/////////////////////////////////////////////////////////
void pix_dot :: processRGBAImage(imageStruct &image)
{
  unsigned int *src = (unsigned int*)image.data;
  unsigned int *dest;

  int x, y, sx, sy;
  
    if (m_xsize != image.xsize)
        alreadyInit = 0;
    if (!alreadyInit)    {
        m_xsize = image.xsize;
        m_ysize = image.ysize;
	if(m_useScale){
	  dot_size = (int)(8 * m_scale);
	  dot_size = dot_size & 0xfe;
	  if(dot_size==0)dot_size=2;
	  dots_width = m_xsize / dot_size;
	  dots_height = m_ysize / dot_size;
	} else {
	  dot_size=m_xsize / dots_width;
	  if(dot_size==0){
	    dot_size=2;
	    dots_width  = m_xsize / dot_size;
	    dots_height = m_ysize / dot_size;	    
	  }
	}
	dot_hsize = dot_size / 2;
  
        pattern = (unsigned int *)malloc(DOTMAX * dot_hsize * dot_hsize * sizeof(unsigned int));
        if (pattern == NULL) {
            post("pix_dot couldn't make pattern");
            return;
        }
       
        sharedbuffer_init();
        sharedbuffer_reset();
        sampx = (int *)sharedbuffer_alloc(m_xsize*sizeof(int));
        sampy = (int *)sharedbuffer_alloc(m_ysize*sizeof(int));
        if (sampx == NULL || sampy == NULL ){
            return;
        }
        makePattern();
        sampxy_table_init();
        alreadyInit = 1;
    }
  myImage.xsize = image.xsize;
  myImage.ysize = image.ysize;
  myImage.csize = image.csize;
  myImage.format= image.format;
  myImage.type  = image.type;
  myImage.reallocate();

  dest = (unsigned int*)myImage.data;
  for ( y=0; y<dots_height; y++) {
    sy = sampy[y];
    for ( x=0; x<dots_width; x++){
        sx = sampx[x];
        drawDot(x, y, inline_RGB2Y( src[sy*image.xsize+sx]), dest);
    }
  }

  image.data=myImage.data;
}

/////////////////////////////////////////////////////////
// processYUVImage
//
/////////////////////////////////////////////////////////
void pix_dot :: processYUVImage(imageStruct &image)
{
    post("pix_dot:  YUV under construction");
    unsigned int *dest;
#if 0
    unsigned int *src = (unsigned int*)image.data;
    int x, y, sx, sx2, sy;
    int luma = 0;
    int luma2 = 0;
    int avgluma = 0;
#endif
    if (m_xsize != image.xsize)
        alreadyInit = 0;
    
    if (!alreadyInit)
    {
        m_xsize = image.xsize;
        m_ysize = image.ysize;
        dot_size = (int)(8 * m_scale);
        dot_size = dot_size & 0xfe;
        dot_hsize = dot_size / 2;
        dots_width = m_xsize / dot_size;
        dots_height = m_ysize / dot_size;
  
        pattern = (unsigned int *)malloc(DOTMAX * dot_hsize * dot_hsize * sizeof(unsigned int));
        if (pattern == NULL) {
            post("pix_dot couldn't make pattern");
            return;
        }
        
        sharedbuffer_init();
        sharedbuffer_reset();
        sampx = (int *)sharedbuffer_alloc(m_xsize*sizeof(int));
        sampy = (int *)sharedbuffer_alloc(m_ysize*sizeof(int));
        if (sampx == NULL || sampy == NULL ){
            return;
        }
        makePatternYUV();
        sampxy_table_init();

        alreadyInit = 1;
    }

    if (myImage.xsize*myImage.ysize*myImage.csize != image.xsize*image.ysize*image.csize)
    {
        dataSize = image.xsize * image.ysize * image.csize;
        myImage.clear();

        myImage.allocate(dataSize);
    }

    myImage.xsize = image.xsize;
    myImage.ysize = image.ysize;
    myImage.csize = image.csize;
    myImage.type  = image.type;

    dest = (unsigned int*)myImage.data;
/*  
    for ( y=0; y<dots_height; y++) {
        sy = sampy[y];
        for ( x=0; x<dots_width/2; x++){
            sx = sampx[x];
            //sx2 = sampx[x+1];
            luma = src[sy*image.xsize+sx+1]&0xff;
            luma2 = (src[sy*image.xsize+sx+1]>>8)&0xff;
            avgluma = (luma + luma2)/2;
            //drawDot(x, y, avgluma, dest);
            //drawDot(x+1, y, luma2, dest);
        }
    }
*/
    image.data = myImage.data;
}

void pix_dot :: scaleMess(float state)
{
  if(state<=0.f){
    error("pix_dot: scale-Factor must not be < 0!");
    return;
  }
  m_scale=state; /* used to be as (int)cast, but i have removed this...*/
  alreadyInit = 0;
  //myImage.reallocate(dataSize);we dont need to reallocate the image, since nothing changed
  myImage.setBlack();
  free(sharedbuffer); sharedbuffer=NULL;
  free(pattern); pattern=NULL;
  m_useScale=true;
}

void pix_dot :: sampxy_table_init()
{
    int i, j;

    j = dot_hsize;
    for(i=0; i<dots_width; i++) {
        sampx[i] = j; //* m_xsize;
        j += dot_size;
    }
    j = dot_hsize;
    for(i=0; i<dots_height; i++) {
        sampy[i] = j;// * m_ysize / screen_height;
        j += dot_size;
    }
}

void pix_dot :: yuv_init()
{
    static int initialized = 0;
    int i;

    if(!initialized) {
        for(i=0; i<256; i++) {
            R2Y[i] =  (int)(0.257*i);
            G2Y[i] =  (int)(0.504*i);
            B2Y[i] =  (int)(0.098*i);
        }
        initialized = 1;
    }
}

/////////////////////////////////////////////////////////
// shared buffer functions
//
/////////////////////////////////////////////////////////
int pix_dot :: sharedbuffer_init()
{
	/* maximum size of the frame buffer is for screen size x 2 */
	sharedbuffer_length = m_xsize * m_ysize * sizeof(unsigned int) * 2;

	sharedbuffer = (unsigned char *)malloc(sharedbuffer_length);
	if(sharedbuffer == NULL)
		return -1;
	else
		return 0;
}

/* The effects uses shared buffer must call this function at first in
 * each effect registrar.
 */
void pix_dot :: sharedbuffer_reset()
{
	tail = 0;
}

/* Allocates size bytes memory in shared buffer and returns a pointer to the
 * memory. NULL is returned when the rest memory is not enough for the request.
 */
unsigned char* pix_dot :: sharedbuffer_alloc(int size)
{
	unsigned char *head;

	if(sharedbuffer_length - tail < size) {
		return NULL;
	}

	head = sharedbuffer + tail;
	tail += size;

	return head;
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_dot :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_dot::sizeMessCallback,
  		  gensym("size"), A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_dot::scaleMessCallback,
                    gensym("scale"), A_FLOAT, A_NULL);
}


void pix_dot :: sizeMessCallback(void *data, t_floatarg width, t_floatarg height)
{
  GetMyClass(data)->sizeMess((int)width, (int)height);  
}

void pix_dot :: scaleMessCallback(void *data, t_floatarg state)
{
  GetMyClass(data)->scaleMess( state );
}
