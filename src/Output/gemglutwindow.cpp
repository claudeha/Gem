///////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyright (c) G�nther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "gemglutwindow.h"

#include "Base/GemGL.h"
#include "GL/freeglut.h"

#include <stdio.h>

gemglutwindow::t_list *gemglutwindow::ggw_list = NULL;

gemglutwindow* gemglutwindow::list_find(int win)
{
  t_list*mylist=0;

  for(mylist=ggw_list; mylist; mylist=mylist->next) {
    if(mylist->window == win)
      return mylist->object;
  }
  /* not found */
  return 0;
}
void gemglutwindow::list_add(gemglutwindow*obj, int win)
{
  t_list*last=ggw_list;
  t_list*element=new t_list;
  element->object=obj;
  element->window=win;
  element->next  =NULL;
 
  if(last) {
    while(last->next) {
      last=last->next;
    }
    last->next = element;
  } else {
    ggw_list = element;
  }
}

void gemglutwindow::list_del(int win)
{
  t_list*mylist=NULL, *last=NULL;
  
  for(mylist=ggw_list; mylist; mylist=mylist->next) {
    if(mylist->window == win) {
      if(last) {
	last->next=mylist->next;
      } else {
	ggw_list = NULL;
      }
      mylist->window=0;
      mylist->object=NULL;
      mylist->next=NULL;
      delete mylist;
      return;
    }
    last=mylist;
  }
}
 
CPPEXTERN_NEW(gemglutwindow)

/////////////////////////////////////////////////////////
//
// gemglutwindow
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
gemglutwindow :: gemglutwindow(void) :
  m_buffer(2),
  m_fsaa(0),
  m_title((char*)"GEM"),
  m_border(false),
  m_width(500), m_height(500),
  m_fullscreen(false),
  m_xoffset(-1), m_yoffset(-1),
  m_cursor(false)
{
  m_infoOut = outlet_new(this->x_obj, 0);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
gemglutwindow :: ~gemglutwindow()
{
  destroyMess();
  outlet_free(m_infoOut); m_infoOut=NULL;
}


bool gemglutwindow :: checkWindow(void){
  if(m_window>0) {
    glutSetWindow(m_window);
  }

  return(m_window>0);
}


/////////////////////////////////////////////////////////
// bangMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: bangMess()
{
  if(!checkWindow()){ 
    error("no window made, cannot render!");
    return;
  }
  glutPostRedisplay();
  glutSwapBuffers();
}

/////////////////////////////////////////////////////////
// renderMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: renderMess()
{
  if(!checkWindow()){ 
    error("no window made, cannot render!");
    return;
  }
  glutPostRedisplay();
  glutMainLoopEvent();
  if(checkWindow())
    glutSwapBuffers();
}
void gemglutwindow :: doRender()
{
  outlet_bang(m_infoOut);
}

void gemglutwindow :: info(t_symbol*s, t_float value)
{
  t_atom atom;
  SETFLOAT(&atom, value);
  outlet_anything(m_infoOut, s, 1, &atom); 
}
void gemglutwindow :: info(t_symbol*s, int argc, t_atom*argv)
{
  outlet_anything(m_infoOut, s, argc, argv); 
}

void gemglutwindow :: motion(int x, int y)
{
  t_atom ap[3];
  SETSYMBOL(ap+0, gensym("motion"));
  SETFLOAT (ap+1, x);
  SETFLOAT (ap+2, y);

  info(gensym("mouse"), 3, ap);
}
void gemglutwindow :: button(int id, int state)
{
  t_atom ap[3];
  SETSYMBOL(ap+0, gensym("button"));
  SETFLOAT (ap+1, id);
  SETFLOAT (ap+2, state);

  info(gensym("mouse"), 3, ap);
}
void gemglutwindow :: key(t_symbol*id, int state)
{
  t_atom ap[3];
  SETSYMBOL(ap+0, gensym("key"));
  SETSYMBOL(ap+1, id);
  SETFLOAT (ap+2, state);

  info(gensym("keyboard"), 3, ap);
}

/////////////////////////////////////////////////////////
// bufferMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: bufferMess(int buf)
{
  switch(buf) {
  case 1: case 2:
    m_buffer=buf;
    if(m_window) {
      post("changing buffer type will only effect newly created windows");
    }
    break;
  default:
    error("buffer can only be '1' (single) or '2' (double) buffered");
    break;
  }
}

/////////////////////////////////////////////////////////
// fsaaMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: fsaaMess(int value)
{
}

/////////////////////////////////////////////////////////
// titleMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: titleMess(t_symbol* s)
{
  m_title = s->s_name;
  if(checkWindow()){
    glutSetWindowTitle(m_title);
    glutSetIconTitle(m_title);
  }
}
/////////////////////////////////////////////////////////
// border
//
/////////////////////////////////////////////////////////
void gemglutwindow :: borderMess(bool setting)
{
  m_border=setting;
}
/////////////////////////////////////////////////////////
// dimensionsMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: dimensionsMess(int width, int height)
{
  if (width <= 0) {
    error("width must be greater than 0");
    return;
  }
    
  if (height <= 0 ) {
    error ("height must be greater than 0");
    return;
  }
  m_width = width;
  m_height = height;
  if(checkWindow()){
    glutReshapeWindow(m_width, m_height);
  }
}
/////////////////////////////////////////////////////////
// fullscreenMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: fullscreenMess(bool on)
{
  m_fullscreen = on;
  if(checkWindow()){
    if(m_fullscreen)
      glutFullScreen();
    else {
      glutReshapeWindow(m_width, m_height);
      glutPositionWindow(m_xoffset, m_yoffset);
    }  
  }
}

/////////////////////////////////////////////////////////
// offsetMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: offsetMess(int x, int y)
{
  m_xoffset = x;
  m_yoffset = y;
  if(checkWindow()){
    glutPositionWindow(x, y);
  }
}

/////////////////////////////////////////////////////////
// createMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: createMess(void)
{
  m_window=glutCreateWindow(m_title);
  list_add(this, m_window);

  glutDisplayFunc   (&gemglutwindow::displayCb);
  glutVisibilityFunc(&gemglutwindow::visibleCb);

  glutKeyboardFunc(&gemglutwindow::keyboardCb);
  glutSpecialFunc(&gemglutwindow::specialCb);
  glutReshapeFunc(&gemglutwindow::reshapeCb);
  glutMouseFunc(&gemglutwindow::mouseCb);
  glutMotionFunc(&gemglutwindow::motionCb);
  glutPassiveMotionFunc(&gemglutwindow::passivemotionCb);
  glutEntryFunc(&gemglutwindow::entryCb);
  glutKeyboardUpFunc(&gemglutwindow::keyboardupCb);
  glutSpecialUpFunc(&gemglutwindow::specialupCb);
  glutJoystickFunc(&gemglutwindow::joystickCb, 20);

  glutMenuStateFunc(&gemglutwindow::menustateCb);
  glutMenuStatusFunc(&gemglutwindow::menustatusCb);

  glutWindowStatusFunc(&gemglutwindow::windowstatusCb);

  //  glutNameFunc(&gemglutwindow::nameCb);
}
/////////////////////////////////////////////////////////
// destroy window
//
/////////////////////////////////////////////////////////
void gemglutwindow :: destroyMess(void)
{
  if(checkWindow()) {
    list_del(m_window);
    glutDestroyWindow(m_window);
    glutMainLoopEvent();
  }
  m_window=0;
}


/////////////////////////////////////////////////////////
// cursorMess
//
/////////////////////////////////////////////////////////
void gemglutwindow :: cursorMess(bool setting)
{
  m_cursor=setting;
  if(checkWindow()){
    glutSetCursor(setting?GLUT_CURSOR_INHERIT:GLUT_CURSOR_NONE);
  }
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void gemglutwindow :: obj_setupCallback(t_class *classPtr)
{
  int argc=0;
  char*argv=NULL;

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500,500);
  glutInit(&argc,&argv);

  class_addbang(classPtr, (t_method)&gemglutwindow::bangMessCallback);
  class_addmethod(classPtr, (t_method)&gemglutwindow::renderMessCallback, gensym("render"), A_NULL);

  class_addmethod(classPtr, (t_method)&gemglutwindow::titleMessCallback,        gensym("title"), A_DEFSYM ,A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::createMessCallback,       gensym("create") ,A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::bufferMessCallback,       gensym("buffer"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::fullscreenMessCallback,   gensym("fullscreen"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::dimensionsMessCallback,   gensym("dimen"), A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::offsetMessCallback,       gensym("offset"), A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::cursorMessCallback,       gensym("cursor"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::destroyMessCallback,      gensym("destroy"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::printMessCallback,        gensym("print"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::borderMessCallback,       gensym("border"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemglutwindow::fsaaMessCallback,         gensym("FSAA"), A_FLOAT, A_NULL);
}
void gemglutwindow :: printMessCallback(void *)
{
  //  GemMan::printInfo();
}
void gemglutwindow :: borderMessCallback(void *data, t_floatarg state)
{
  GetMyClass(data)->borderMess((int)state);
}
void gemglutwindow :: destroyMessCallback(void *data)
{
  GetMyClass(data)->destroyMess();
}
void gemglutwindow :: bangMessCallback(void *data)
{
  GetMyClass(data)->bangMess();
}
void gemglutwindow :: renderMessCallback(void *data)
{
  GetMyClass(data)->renderMess();
}
void gemglutwindow :: titleMessCallback(void *data, t_symbol* disp)
{
  GetMyClass(data)->titleMess(disp);
}
void gemglutwindow :: createMessCallback(void *data)
{
  GetMyClass(data)->createMess();
}
void gemglutwindow :: bufferMessCallback(void *data, t_floatarg buf)
{
  GetMyClass(data)->bufferMess((int)buf);
}
void gemglutwindow :: fullscreenMessCallback(void *data, t_floatarg on)
{
  GetMyClass(data)->fullscreenMess((int)on);
}
void gemglutwindow :: dimensionsMessCallback(void *data, t_floatarg width, t_floatarg height)
{
  GetMyClass(data)->dimensionsMess((int)width, (int)height);
}
void gemglutwindow :: offsetMessCallback(void *data, t_floatarg x, t_floatarg y)
{
  GetMyClass(data)->offsetMess((int)x, (int)y);
}
void gemglutwindow :: cursorMessCallback(void *data, t_floatarg val)
{
  GetMyClass(data)->cursorMess((float)val);
}
void gemglutwindow :: fsaaMessCallback(void *data, t_floatarg val)
{
  GetMyClass(data)->fsaaMess((int) val);
}

#define CALLBACK4WIN gemglutwindow*ggw=list_find(glutGetWindow()); if(!ggw){::error("couldn't find [gemglutwindow] for window#%d", glutGetWindow()); return;} else ggw


void gemglutwindow::displayCb(void) {
  CALLBACK4WIN ->doRender();
}

void gemglutwindow::visibleCb(int state) {
  CALLBACK4WIN->info(gensym("visible"), state);
}


static t_symbol*key2symbol(unsigned char c) {
  t_symbol*sym=NULL;
  char s[2];
  switch(c) {
  default:
    sprintf(s, "%c\0", c);
    sym=gensym(s);
  }
  return sym;
}

static t_symbol*key2symbol(int c) {
  t_symbol*s=NULL;

  switch(c) {
  case GLUT_KEY_F1: s=gensym("F1"); break;
  case GLUT_KEY_F2: s=gensym("F2"); break;
  case GLUT_KEY_F3: s=gensym("F3"); break;
  case GLUT_KEY_F4: s=gensym("F4"); break;
  case GLUT_KEY_F5: s=gensym("F5"); break;
  case GLUT_KEY_F6: s=gensym("F6"); break;
  case GLUT_KEY_F7: s=gensym("F7"); break;
  case GLUT_KEY_F8: s=gensym("F8"); break;
  case GLUT_KEY_F9: s=gensym("F9"); break;
  case GLUT_KEY_F10: s=gensym("F10"); break;
  case GLUT_KEY_F11: s=gensym("F11"); break;
  case GLUT_KEY_F12: s=gensym("F12"); break;
  case GLUT_KEY_LEFT: s=gensym("Left"); break;
  case GLUT_KEY_UP: s=gensym("Up"); break;
  case GLUT_KEY_RIGHT: s=gensym("Right"); break;
  case GLUT_KEY_DOWN: s=gensym("Down"); break;
  case GLUT_KEY_PAGE_UP: s=gensym("PageUp"); break;
  case GLUT_KEY_PAGE_DOWN: s=gensym("PageDown"); break;
  case GLUT_KEY_HOME: s=gensym("Home"); break;
  case GLUT_KEY_END: s=gensym("End"); break;
  case GLUT_KEY_INSERT: s=gensym("Insert"); break;
  default:
    s=gensym("unknown");
  }

  return s;
}
void gemglutwindow::keyboardCb(unsigned char c, int x, int y) {
  CALLBACK4WIN->motion(x,y);
  ggw->key(key2symbol(c), 1);
}
void gemglutwindow::keyboardupCb(unsigned char c, int x, int y) {
  CALLBACK4WIN->motion(x,y);
  ggw->key(key2symbol(c), 0);
}

void gemglutwindow::specialCb(int c, int x, int y) {
  CALLBACK4WIN->motion(x,y);
  ggw->key(key2symbol(c), 1);
}

void gemglutwindow::specialupCb(int c, int x, int y) {
  CALLBACK4WIN->motion(x,y);
  ggw->key(key2symbol(c), 0);
}


void gemglutwindow::reshapeCb(int x, int y) {
  t_atom ap[2];
  SETFLOAT (ap+0, x);
  SETFLOAT (ap+1, y);

  CALLBACK4WIN->info(gensym("dimen"), 2, ap);
}
void gemglutwindow::mouseCb(int button, int state, int x, int y) {
  CALLBACK4WIN->motion(x,y);
  ggw->button(button, state);
}
void gemglutwindow::motionCb(int x, int y) {
  CALLBACK4WIN->motion(x,y);
}
void gemglutwindow::passivemotionCb(int x, int y) {
  CALLBACK4WIN->motion(x,y);
}

void gemglutwindow::entryCb(int state) {
  CALLBACK4WIN->info(gensym("entry"), state);
}
void gemglutwindow::joystickCb(unsigned int a, int x, int y, int z) {
}
void gemglutwindow::menustateCb(int value) {
}
void gemglutwindow::menustatusCb(int x, int y, int z) {
}
void gemglutwindow::windowstatusCb(int value) {
  CALLBACK4WIN->info(gensym("window"), value);
}
