/*
 *      fastimg -
 *              Faster reading and writing of image files.
 *
 *      This code should work on machines with any byte order.
 *
 *      Could someone make this run real fast using multiple processors
 *      or how about using memory mapped files to speed it up?
 *
 *                              Paul Haeberli - 1991
 */
/*
 *  porting to Windows NT
 *              Masayuki Mastumoto - 1995
 */
/*
 * Modified so that it doesn't exit on error.
 * Added support for reading grayscale, rgb, and rgba images
 *                              Mark Danks - 1998
 */


// FIXXME: use C++ memory allocators

#include <stdlib.h>
#include "sgiimage.h"


#ifndef _WIN32
# include <unistd.h>
#endif  // for !win32

#include <string.h>
#include <stdio.h>

#include "Gem/RTE.h"

/*
 *      from image.h
 *
 */
typedef struct {
  unsigned short        imagic;         /* stuff saved on disk . . */
  unsigned short        type;
  unsigned short        dim;
  unsigned short        xsize;
  unsigned short        ysize;
  unsigned short        zsize;
  unsigned int32        min;
  unsigned int32        max;
  unsigned int32        wastebytes;
  char          name[80];
  unsigned int32        colormap;

  int32                 file;           /* stuff used in core only */
  unsigned short        flags;
  short         dorev;
  short         x;
  short         y;
  short         z;
  short         cnt;
  unsigned short        *ptr;
  unsigned short        *base;
  unsigned short        *tmpbuf;
  unsigned int32        offset;
  unsigned int32        rleend;         /* for rle images */
  unsigned int32        *rowstart;      /* for rle images */
  int32         *rowsize;       /* for rle images */
} IMAGE;

#define IMAGIC  0732

#define TYPEMASK                0xff00
#define BPPMASK                 0x00ff
#define ITYPE_VERBATIM          0x0000
#define ITYPE_RLE               0x0100
#define ISRLE(type)             (((type) & 0xff00) == ITYPE_RLE)
#define ISVERBATIM(type)        (((type) & 0xff00) == ITYPE_VERBATIM)
#define BPP(type)               ((type) & BPPMASK)
#define RLE(bpp)                (ITYPE_RLE | (bpp))
#define VERBATIM(bpp)           (ITYPE_VERBATIM | (bpp))
/*
 *      end of image.h stuff
 *
 */

#define RINTLUM (79)
#define GINTLUM (156)
#define BINTLUM (21)

#define ILUM(r,g,b)     (static_cast<int>(RINTLUM*(r)+GINTLUM*(g)+BINTLUM*(b))>>8)

#define OFFSET_R        3       /* this is byte order dependent */
#define OFFSET_G        2
#define OFFSET_B        1
#define OFFSET_A        0

#define CHANOFFSET(z)   (3-(z)) /* this is byte order dependent */

static void expandrow(unsigned char *optr, unsigned char *iptr, int32 z);
static void setalpha(unsigned char *lptr, size_t n);
static void copybw(int32 *lptr, size_t n);
static void lumrow(unsigned char *rgbptr, unsigned char *lumptr, size_t n);
static int compressrow(unsigned char *lbuf, unsigned char *rlebuf, int32 z,
                       int32 cnt);
static void interleaverow(unsigned char *lptr, unsigned char *cptr,
                          int32 z, int32 n);

#define TAGLEN  (5UL)

/*
 *      addlongimgtag -
 *              this is used to extract image data from core dumps.
 *
 */
unsigned int32 *
getLongImage(const char *textureFile, int32 *xsize, int32 *ysize,
             int32 *csize)
{
  int size=sizeofimage(textureFile, xsize, ysize, csize);
  if(size<1) {
    return 0;
  }
  return longimagedata(textureFile);
}

void addlongimgtag(unsigned int32 *dptr, int32 xsize, int32 ysize)
{
  dptr = dptr+(xsize*ysize);
  dptr[0] = 0x12345678;
  dptr[1] = 0x59493333;
  dptr[2] = 0x69434222;
  dptr[3] = xsize;
  dptr[4] = ysize;
}

/*
 *      byte order independent read/write of shorts and longs.
 *
 */
static unsigned short getshort(FILE *inf)
{
  unsigned char buf[2];

  size_t count=fread(buf,2,1,inf);

  if(count<1) {
    fprintf(stderr, "[GEM:imageSGI] error reading file\n");
    return 0;
  }
  return (buf[0]<<8)+(buf[1]<<0);
}

static unsigned int32 getlong(FILE *inf)
{
  unsigned char buf[4];
  size_t count = fread(buf,4,1,inf);
  if(count<1) {
    fprintf(stderr, "[GEM:imageSGI] error reading file\n");
    return 0;
  }
  return (buf[0]<<24)+(buf[1]<<16)+(buf[2]<<8)+(buf[3]<<0);
}

static void putshort(FILE *outf, unsigned short val)
{
  unsigned char buf[2];

  buf[0] = (val>>8);
  buf[1] = (val>>0);
  fwrite(buf,2,1,outf);
}

static int putlong(FILE *outf, unsigned int32 val)
{
  unsigned char buf[4];

  buf[0] = static_cast<unsigned char>(val>>24);
  buf[1] = static_cast<unsigned char>(val>>16);
  buf[2] = static_cast<unsigned char>(val>>8);
  buf[3] = static_cast<unsigned char>(val>>0);
  return fwrite(buf,4,1,outf);
}

static void readheader(FILE *inf, IMAGE *image)
{
  memset(image, 0, sizeof(IMAGE));
  image->imagic = getshort(inf);
  image->type = getshort(inf);
  image->dim = getshort(inf);
  image->xsize = getshort(inf);
  image->ysize = getshort(inf);
  image->zsize = getshort(inf);
}

static int writeheader(FILE *outf, IMAGE *image, const char*name)
{
  IMAGE t;
  size_t namelen=0;
  if(0==name) {
    name="no name";
  }
  namelen=strlen(name);

  memset(&t, 0, sizeof(IMAGE));
  fwrite(&t,sizeof(IMAGE),1,outf);
  if(fseek(outf,0,SEEK_SET)<0) {
    return -1;
  }
  putshort(outf,image->imagic);
  putshort(outf,image->type);
  putshort(outf,image->dim);
  putshort(outf,image->xsize);
  putshort(outf,image->ysize);
  putshort(outf,image->zsize);
  putlong(outf,image->min);
  putlong(outf,image->max);
  putlong(outf,0);

  // name can only be 80 characters (including terminating 0-byte) long
  if(namelen>79) {
    unsigned char buf[1];
    buf[0]=0;

    fwrite(name, 79, 1, outf);
    return fwrite(buf, 1, 1, outf);
  }
  return fwrite(name,namelen,1,outf);
}

static int writetab(FILE *outf, unsigned int32 *tab, int32 len)
{
  int r=0;

  while(len) {
    r = putlong(outf,*tab++);
    len -= 4;
  }
  return r;
}

static void readtab(FILE *inf, unsigned int32 *tab, int32 len)
{
  while(len) {
    *tab++ = getlong(inf);
    len -= 4;
  }
}

/*
 *      sizeofimage -
 *              return the xsize and ysize of an iris image file.
 *
 */
int sizeofimage(const char *name, int32 *xsize, int32 *ysize, int32 *csize)
{
  IMAGE image;
  FILE *inf;

  inf = fopen(name,"rb");
  if(!inf) {
    return(0);
  }
  readheader(inf,&image);
  if(image.imagic != IMAGIC) {
    fclose(inf);
    return(0);
  }
  *xsize = image.xsize;
  *ysize = image.ysize;
  *csize = image.zsize;
  fclose(inf);
  return(1);
}

/*
 *      longimagedata -
 *              read in a B/W RGB or RGBA iris image file and return a
 *      pointer to an array of longs.
 *
 */
unsigned int32 *longimagedata(const char *name)
{
  unsigned int32 *base=NULL, *lptr=NULL;
  unsigned char *rledat=NULL, *verdat=NULL;
  unsigned int32 *starttab=NULL, *lengthtab=NULL;
  FILE *inf=NULL;
  IMAGE *image=NULL;
  int y, z;
  unsigned long tablen, xysize;
  unsigned short xsize, ysize, zsize;
  int bpp, rle, badorder;
  unsigned long rlebuflen;
  unsigned int cur;

  inf = fopen(name,"rb");
  if(!inf) {
    goto error;
  }
  image = (IMAGE *)malloc(sizeof(IMAGE));
  readheader(inf,image);
  if(image->imagic != IMAGIC) {
    goto error;
  }
  switch(image->zsize) {
  case 4:
  case 3:
  case 1:
    break;
  default:
    goto error;
  }
  rle = ISRLE(image->type);
  bpp = BPP(image->type);
  if(bpp != 1 ) {
    fprintf(stderr,
            "[GEM:imageSGI] longimagedata: image must have 1 byte per pix chan\n");
    goto error;
  }
  xsize = image->xsize;
  ysize = image->ysize;
  zsize = image->zsize;
  xysize=(unsigned long)xsize*(unsigned long)ysize;

  if(rle) {
    tablen = (unsigned long)ysize*zsize*sizeof(int32);
    starttab = (unsigned int32 *)malloc(tablen);
    lengthtab = (unsigned int32 *)malloc(tablen);
    rlebuflen = static_cast<unsigned long>(1.05*xsize)+10;
    rledat = (unsigned char *)malloc(rlebuflen);
    if(fseek(inf,512,SEEK_SET)<0) {
      fprintf(stderr, "[GEM:imageSGI] longimagedata: fseek returned 0\n");
      goto error;
    }
    readtab(inf,starttab,tablen);
    readtab(inf,lengthtab,tablen);

    /* check data order */
    cur = 0;
    badorder = 0;
    for(y=0; y<ysize; y++) {
      for(z=0; z<zsize; z++) {
        if(starttab[y+z*ysize]<cur) {
          badorder = 1;
          break;
        }
        cur = starttab[y+z*ysize];
      }
      if(badorder) {
        break;
      }
    }

    if(fseek(inf,512+2*tablen,SEEK_SET)) {
      goto error;
    }
    cur = 512+2*tablen;
    base = (unsigned int32 *)
           malloc((xysize+TAGLEN)*sizeof(int32));
    addlongimgtag(base,xsize,ysize);

    if(badorder) {
      for(z=0; z<zsize; z++) {
        lptr = base;
        for(y=0; y<ysize; y++) {
          if(cur != starttab[y+z*ysize]) {
            if(fseek(inf,starttab[y+z*ysize],SEEK_SET)<0) {
              goto error;
            }
            cur = starttab[y+z*ysize];
          }
          if(lengthtab[y+z*ysize]>rlebuflen) {
            fprintf(stderr,
                    "[GEM:imageSGI] longimagedata: rlebuf(%lu) is too small - bad poop : %d\n",
                    rlebuflen, lengthtab[y+z*ysize]);
            goto error;
          }
          size_t count=fread(rledat,lengthtab[y+z*ysize],1,inf);
          if(count<1) {
            fprintf(stderr, "[GEM:imageSGI] error reading file\n");
            goto error;
          }

          cur += lengthtab[y+z*ysize];
#ifdef IRISGL
          expandrow((unsigned char *)lptr,(unsigned char *)rledat,3-z);
#else
          expandrow((unsigned char *)lptr,(unsigned char *)rledat,z);
#endif
          lptr += xsize;
        }
      }
    } else {
      lptr = base;
      for(y=0; y<ysize; y++) {
        for(z=0; z<zsize; z++) {
          if(cur != starttab[y+z*ysize]) {
            if (fseek(inf,starttab[y+z*ysize],SEEK_SET) < 0) {
              goto error;
            }
            cur = starttab[y+z*ysize];
          }
          size_t count=fread(rledat,lengthtab[y+z*ysize],1,inf);
          if(count<1) {
            fprintf(stderr, "[GEM:imageSGI] error reading file\n");
            goto error;
          }

          cur += lengthtab[y+z*ysize];
#ifdef IRISGL
          expandrow((unsigned char *)lptr,(unsigned char *)rledat,3-z);
#else
          expandrow((unsigned char *)lptr,(unsigned char *)rledat,z);
#endif
        }
        lptr += xsize;
      }
    }
    if(zsize == 3) {
      setalpha((unsigned char *)base,xysize);
    } else if(zsize<3) {
      copybw((int32 *)base,xysize);
    }
    goto success;
  } else {
    base = (unsigned int32 *)
           malloc((xysize+TAGLEN)*sizeof(int32));
    addlongimgtag(base,xsize,ysize);
    verdat = (unsigned char *)malloc(xsize);
    if(fseek(inf,512,SEEK_SET) < 0) {
      goto error;
    }
    for(z=0; z<zsize; z++) {
      lptr = base;
      /* coverity[tainted_data] */
      for(y=0; y<ysize; y++) {
        size_t count = fread(verdat,xsize,1,inf);
        if(count<1) {
          fprintf(stderr, "[GEM:imageSGI] error reading file\n");
          goto error;
        }

#ifdef IRISGL
        interleaverow((unsigned char *)lptr,verdat,3-z,xsize);
#else
        interleaverow((unsigned char *)lptr,verdat,z,xsize);
#endif
        lptr += xsize;
      }
    }
    if(zsize == 3) {
      setalpha((unsigned char *)base,xysize);
    } else if(zsize<3) {
      copybw((int32 *)base,xysize);
    }
    goto success;
  }
error:
  if(base) {
    free(base);
  }
  base=NULL;
success:
  if(rledat) {
    free(rledat);
  }
  if(verdat) {
    free(verdat);
  }
  if(starttab) {
    free(starttab);
  }
  if(lengthtab) {
    free(lengthtab);
  }
  if(inf) {
    fclose(inf);
  }

  if(image) {
    free(image);
  }

  return(base);
}

/* static utility functions for longimagedata */

static void interleaverow(unsigned char *lptr, unsigned char *cptr,
                          int32 z, int32 n)
{
  lptr += z;
  while(n--) {
    *lptr = *cptr++;
    lptr += 4;
  }
}

static void copybw(int32 *lptr, size_t n)
{
  while(n>=8) {
    lptr[0] = 0xff000000+(0x010101*(lptr[0]&0xff));
    lptr[1] = 0xff000000+(0x010101*(lptr[1]&0xff));
    lptr[2] = 0xff000000+(0x010101*(lptr[2]&0xff));
    lptr[3] = 0xff000000+(0x010101*(lptr[3]&0xff));
    lptr[4] = 0xff000000+(0x010101*(lptr[4]&0xff));
    lptr[5] = 0xff000000+(0x010101*(lptr[5]&0xff));
    lptr[6] = 0xff000000+(0x010101*(lptr[6]&0xff));
    lptr[7] = 0xff000000+(0x010101*(lptr[7]&0xff));
    lptr += 8;
    n-=8;
  }
  while(n--) {
    *lptr = 0xff000000+(0x010101*(*lptr&0xff));
    lptr++;
  }
}

static void setalpha(unsigned char *lptr, size_t n)
{
#ifndef IRISGL
  lptr +=3;
#endif
  while(n>=8) {
    lptr[0*4] = 0xff;
    lptr[1*4] = 0xff;
    lptr[2*4] = 0xff;
    lptr[3*4] = 0xff;
    lptr[4*4] = 0xff;
    lptr[5*4] = 0xff;
    lptr[6*4] = 0xff;
    lptr[7*4] = 0xff;
    lptr += 4*8;
    n -= 8;
  }
  while(n--) {
    *lptr = 0xff;
    lptr += 4;
  }
}

static void expandrow(unsigned char *optr, unsigned char *iptr, int32 z)
{
  unsigned char count;
  optr += z;
  while(1) {
    unsigned char pixel = *iptr++;
    if ( !(count = (pixel & 0x7f)) ) {
      return;
    }
    if(pixel & 0x80) {
      while(count>=8) {
        optr[0*4] = iptr[0];
        optr[1*4] = iptr[1];
        optr[2*4] = iptr[2];
        optr[3*4] = iptr[3];
        optr[4*4] = iptr[4];
        optr[5*4] = iptr[5];
        optr[6*4] = iptr[6];
        optr[7*4] = iptr[7];
        optr += 8*4;
        iptr += 8;
        count -= 8;
      }
      while(count--) {
        *optr = *iptr++;
        optr+=4;
      }
    } else {
      pixel = *iptr++;
      while(count>=8) {
        optr[0*4] = pixel;
        optr[1*4] = pixel;
        optr[2*4] = pixel;
        optr[3*4] = pixel;
        optr[4*4] = pixel;
        optr[5*4] = pixel;
        optr[6*4] = pixel;
        optr[7*4] = pixel;
        optr += 8*4;
        count -= 8;
      }
      while(count--) {
        *optr = pixel;
        optr+=4;
      }
    }
  }
}


/*
 *      longstoimage -
 *              copy an array of longs to an iris image file.  Each long
 *      represents one pixel.  xsize and ysize specify the dimensions of
 *      the pixel array.  zsize specifies what kind of image file to
 *      write out.  if zsize is 1, the luminance of the pixels are
 *      calculated, and a single channel black and white image is saved.
 *      If zsize is 3, an RGB image file is saved.  If zsize is 4, an
 *      RGBA image file is saved.
 *
 */
int longstoimage(unsigned int32 *lptr, int32 xsize, int32 ysize,
                 int32 zsize,
                 const char *name, const char*imgname)
{
  FILE *outf;
  IMAGE *image;
  int tablen, y, z, pos, len;
  int32 *starttab, *lengthtab;
  unsigned char *rlebuf;
  unsigned int32 *lumbuf;
  unsigned long rlebuflen;
  int goodwrite=0;

  switch(zsize) {
  case 4:
  case 3:
  case 1:
    break;
  default:
    fprintf(stderr,
            "[GEM:imageSGI] longstoimage: invalid zsize %d (must be 1,3 or 4)\n",
            zsize);
    return 0;
  }

  goodwrite = 1;
  outf = fopen(name,"wb");
  if(!outf) {
    fprintf(stderr, "[GEM:imageSGI] longstoimage: can't open output file\n");
    return 0;
  }
  tablen = ysize*zsize*sizeof(int32);

  image = (IMAGE *)malloc(sizeof(IMAGE));
  starttab = (int32 *)malloc(tablen);
  lengthtab = (int32 *)malloc(tablen);
  rlebuflen = static_cast<unsigned long>(1.05*xsize)+10;
  rlebuf = (unsigned char *)malloc(rlebuflen);
  lumbuf = (unsigned int32 *)malloc(xsize*sizeof(int32));

  memset(image,0,sizeof(IMAGE));
  image->imagic = IMAGIC;
  image->type = RLE(1);
  if(zsize>1) {
    image->dim = 3;
  } else {
    image->dim = 2;
  }
  image->xsize = static_cast<unsigned short>(xsize);
  image->ysize = static_cast<unsigned short>(ysize);
  image->zsize = static_cast<unsigned short>(zsize);
  image->min = 0;
  image->max = 255;
  goodwrite *= writeheader(outf, image, imgname);
  if(fseek(outf,512+2*tablen,SEEK_SET) < 0) {
    fprintf(stderr, "[GEM:imageSGI] longstoimage: fseek failed\n");
    goto longstoimage_close;
  }
  pos = 512+2*tablen;
  for(y=0; y<ysize; y++) {
    for(z=0; z<zsize; z++) {
      if(zsize == 1) {
        lumrow((unsigned char *)lptr,(unsigned char *)lumbuf,xsize);
#ifdef IRISGL
        len = compressrow((unsigned char *)lumbuf,rlebuf,CHANOFFSET(z),xsize);
#else
        len = compressrow((unsigned char *)lumbuf,rlebuf,CHANOFFSET(3-z),xsize);
#endif
      } else {
#ifdef IRISGL
        len = compressrow((unsigned char *)lptr,rlebuf,CHANOFFSET(z),xsize);
#else
        len = compressrow((unsigned char *)lptr,rlebuf,CHANOFFSET(3-z),xsize);
#endif
      }
      if(len>rlebuflen) {
        fprintf(stderr,
                "[GEM:imageSGI] longstoimage: rlebuf is too small - bad poop\n");
        goodwrite=0;
        goto longstoimage_close;
      }
      goodwrite *= fwrite(rlebuf,len,1,outf);
      starttab[y+z*ysize] = pos;
      lengthtab[y+z*ysize] = len;
      pos += len;
    }
    lptr += xsize;
  }

  if(fseek(outf,512,SEEK_SET) < 0) {
    fprintf(stderr, "[GEM:imageSGI] longstoimage: fseek failed...\n");
    goodwrite=0;
    goto longstoimage_close;
  }
  goodwrite *= writetab(outf,(unsigned int32 *)starttab,tablen);
  goodwrite *= writetab(outf,(unsigned int32 *)lengthtab,tablen);
longstoimage_close:
  free(image);
  free(starttab);
  free(lengthtab);
  free(rlebuf);
  free(lumbuf);
  fclose(outf);
  if(goodwrite) {
    return 1;
  } else {
    fprintf(stderr,
            "[GEM:imageSGI] longstoimage: not enough space for image!!\n");
    return 0;
  }
}

/* static utility functions for longstoimage */

static void lumrow(unsigned char *rgbptr, unsigned char *lumptr, size_t n)
{
  lumptr += CHANOFFSET(0);
  while(n--) {
    *lumptr = ILUM(rgbptr[OFFSET_R],rgbptr[OFFSET_G],rgbptr[OFFSET_B]);
    lumptr += 4;
    rgbptr += 4;
  }
}

static int compressrow(unsigned char *lbuf, unsigned char *rlebuf, int32 z,
                       int32 cnt)
{
  unsigned char *iptr, *ibufend, *optr;
  lbuf += z;
  iptr = lbuf;
  ibufend = iptr+cnt*4;
  optr = rlebuf;

  while(iptr<ibufend) {
    unsigned char *sptr = iptr;
    iptr += 8;
    while((iptr<ibufend)&& ((iptr[-8]!=iptr[-4])||(iptr[-4]!=iptr[0]))) {
      iptr+=4;
    }
    iptr -= 8;
    int32 count = (iptr-sptr)/4;
    while(count) {
      short todo = static_cast<short>((count > 126) ? 126 : count);
      count -= todo;
      *optr++ = 0x80|todo;
      while(todo>8) {
        optr[0] = sptr[0*4];
        optr[1] = sptr[1*4];
        optr[2] = sptr[2*4];
        optr[3] = sptr[3*4];
        optr[4] = sptr[4*4];
        optr[5] = sptr[5*4];
        optr[6] = sptr[6*4];
        optr[7] = sptr[7*4];
        optr += 8;
        sptr += 8*4;
        todo -= 8;
      }
      while(todo--) {
        *optr++ = *sptr;
        sptr += 4;
      }
    }
    sptr = iptr;
    short cc = *iptr;
    iptr += 4;
    while( (iptr<ibufend) && (*iptr == cc) ) {
      iptr += 4;
    }
    count = (iptr-sptr)/4;
    while(count) {
      short todo = static_cast<short>(count>126 ? 126:count);
      count -= todo;
      *optr++ = static_cast<unsigned char>(todo);
      *optr++ = static_cast<unsigned char>(cc);
    }
  }
  *optr++ = 0;
  return optr - rlebuf;
}
