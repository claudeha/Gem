////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.at
//
// Implementation file
//
//    Copyright (c) 1997-1999 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
//
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "GemGluObj.h"

/////////////////////////////////////////////////////////
//
// GemGluObj
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
GemGluObj :: GemGluObj(t_floatarg size, t_floatarg slices,
                       t_floatarg stacks)
  : GemShape(size),
    m_numSlices((int)slices),m_numStacks((int)stacks),
    m_sliceInlet(NULL)
{
  m_drawTypes.clear();
  m_drawTypes["default"]=GL_DEFAULT_GEM;
  m_drawTypes["line"]=GL_LINE;
  m_drawTypes["fill"]=GL_FILL;
  m_drawTypes["point"]=GL_POINT;
  m_drawTypes["points"]=GL_POINT;

  m_drawType = (GLenum) GL_FILL;
  if(m_numSlices<=0) {
    m_numSlices=10;
  }
  if(m_numStacks<=0) {
    m_numStacks=m_numSlices;
  }

  // the number of slices
  m_sliceInlet = inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"),
                           gensym("numslices"));
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
GemGluObj :: ~GemGluObj()
{
  // in case we are deleted while still running
  inlet_free(m_sliceInlet);
}

/////////////////////////////////////////////////////////
// numSlicesMess
//
/////////////////////////////////////////////////////////
void GemGluObj :: numSlicesMess(int numSlices)
{
  m_numSlices = (numSlices < 2) ? 2 : numSlices;
  m_numStacks = m_numSlices;
  setModified();
}
void GemGluObj :: numSlicesMess(int numSlices, int numStacks)
{
  m_numSlices = (numSlices < 2) ? 2 : numSlices;
  m_numStacks = (numStacks < 2) ? 2 : numStacks;
  setModified();
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void GemGluObj :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr,
                  reinterpret_cast<t_method>(&GemGluObj::numSlicesMessCallback),
                  gensym("numslices"), A_GIMME, A_NULL);
}
void GemGluObj :: numSlicesMessCallback(void *data, t_symbol*, int argc,
                                        t_atom*argv)
{
  switch(argc) {
  case 1:
    GetMyClass(data)->numSlicesMess(atom_getint(argv));
    break;
  case 2:
    GetMyClass(data)->numSlicesMess(atom_getint(argv), atom_getint(argv+1));
    break;
  default:
    GetMyClass(
      data)->error("only 1 or 2 arguments for \"slices [stacks]\" allowed!");
  }
}
