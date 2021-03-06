/* ------------------------------------------------------------------
 * GEM - Graphics Environment for Multimedia
 *
 *  Copyright (c) 2002-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
 *     zmoelnig@iem.at
 *  For information on usage and redistribution, and for a DISCLAIMER
 *  OF ALL WARRANTIES, see the file, "GEM.LICENSE.TERMS"
 *
 *  this file has been generated...
 * ------------------------------------------------------------------
 */

#ifndef _INCLUDE__GEM_OPENGL_GEMGLVERTEX4D_H_
#define _INCLUDE__GEM_OPENGL_GEMGLVERTEX4D_H_

#include "Base/GemGLBase.h"

/*
 CLASS
        GEMglVertex4d
 KEYWORDS
        openGL  0
 DESCRIPTION
        wrapper for the openGL-function
        "glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w)"
 */

class GEM_EXTERN GEMglVertex4d : public GemGLBase
{
  CPPEXTERN_HEADER(GEMglVertex4d, GemGLBase);

public:
  // Constructor
  GEMglVertex4d (t_float, t_float, t_float, t_float);   // CON

protected:
  // Destructor
  virtual ~GEMglVertex4d ();
  // Do the rendering
  virtual void  render (GemState *state);

  // variables
  GLdouble      x;              // VAR
  virtual void  xMess(t_float); // FUN

  GLdouble      y;              // VAR
  virtual void  yMess(t_float); // FUN

  GLdouble      z;              // VAR
  virtual void  zMess(t_float); // FUN

  GLdouble      w;              // VAR
  virtual void  wMess(t_float); // FUN


private:

  // we need some inlets
  t_inlet *m_inlet[4];

  // static member functions
  static void    xMessCallback (void*, t_float);
  static void    yMessCallback (void*, t_float);
  static void    zMessCallback (void*, t_float);
  static void    wMessCallback (void*, t_float);
};
#endif // for header file
