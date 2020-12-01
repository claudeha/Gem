/*
 * GemGL: openGL includes for GEM
 *
 * include this file if you want to include the
 * openGL-headers installed on your system
 *
 * tasks:
 *
 * + this file hides the peculiarities of the various platforms
 *   (like "OpenGL/gl.h" vs "GL/gl.h")
 *
 * + define some pre-processor defines that are missing in the GL-headers
 *
 * + try to exclude parts of the GL-headers based on GemConfig.h
 *
 */


#ifndef _INCLUDE__GEM_GEM_GEMGL_H_
#define _INCLUDE__GEM_GEM_GEMGL_H_

#include "Gem/ExportDef.h"

// I hate Microsoft...I shouldn't have to do this!
#ifdef _WIN32
# include <windows.h>
#endif

#include "Gem/GemGLconfig.h"

#ifdef GEM_MULTICONTEXT
# define GLEW_MX
#endif

#ifndef GEM_HAVE_GLU
# define GLEW_NO_GLU
#else
#endif


#ifdef __EMSCRIPTEN__

// GL 1.0
#define glAccum                             rglAccum
#define glAlphaFunc                         rglAlphaFunc
#define glBegin                             rglBegin
#define glBitmap                            rglBitmap
#define glBlendFunc                         rglBlendFunc
#define glCallList                          rglCallList
#define glCallLists                         rglCallLists
#define glClear                             rglClear
#define glClearAccum                        rglClearAccum
#define glClearColor                        rglClearColor
#define glClearDepth                        rglClearDepth
#define glClearIndex                        rglClearIndex
#define glClearStencil                      rglClearStencil
#define glClipPlane                         rglClipPlane
#define glColor3b                           rglColor3b
#define glColor3bv                          rglColor3bv
#define glColor3d                           rglColor3d
#define glColor3dv                          rglColor3dv
#define glColor3f                           rglColor3f
#define glColor3fv                          rglColor3fv
#define glColor3i                           rglColor3i
#define glColor3iv                          rglColor3iv
#define glColor3s                           rglColor3s
#define glColor3sv                          rglColor3sv
#define glColor3ub                          rglColor3ub
#define glColor3ubv                         rglColor3ubv
#define glColor3ui                          rglColor3ui
#define glColor3uiv                         rglColor3uiv
#define glColor3us                          rglColor3us
#define glColor3usv                         rglColor3usv
#define glColor4b                           rglColor4b
#define glColor4bv                          rglColor4bv
#define glColor4d                           rglColor4d
#define glColor4dv                          rglColor4dv
#define glColor4f                           rglColor4f
#define glColor4fv                          rglColor4fv
#define glColor4i                           rglColor4i
#define glColor4iv                          rglColor4iv
#define glColor4s                           rglColor4s
#define glColor4sv                          rglColor4sv
#define glColor4ub                          rglColor4ub
#define glColor4ubv                         rglColor4ubv
#define glColor4ui                          rglColor4ui
#define glColor4uiv                         rglColor4uiv
#define glColor4us                          rglColor4us
#define glColor4usv                         rglColor4usv
#define glColorMask                         rglColorMask
#define glColorMaterial                     rglColorMaterial
#define glCopyPixels                        rglCopyPixels
#define glCullFace                          rglCullFace
#define glDeleteLists                       rglDeleteLists
#define glDepthFunc                         rglDepthFunc
#define glDepthMask                         rglDepthMask
#define glDepthRange                        rglDepthRange
#define glDisable                           rglDisable
#define glDrawBuffer                        rglDrawBuffer
#define glDrawPixels                        rglDrawPixels
#define glEdgeFlag                          rglEdgeFlag
#define glEdgeFlagv                         rglEdgeFlagv
#define glEnable                            rglEnable
#define glEnd                               rglEnd
#define glEndList                           rglEndList
#define glEvalCoord1d                       rglEvalCoord1d
#define glEvalCoord1dv                      rglEvalCoord1dv
#define glEvalCoord1f                       rglEvalCoord1f
#define glEvalCoord1fv                      rglEvalCoord1fv
#define glEvalCoord2d                       rglEvalCoord2d
#define glEvalCoord2dv                      rglEvalCoord2dv
#define glEvalCoord2f                       rglEvalCoord2f
#define glEvalCoord2fv                      rglEvalCoord2fv
#define glEvalMesh1                         rglEvalMesh1
#define glEvalMesh2                         rglEvalMesh2
#define glEvalPoint1                        rglEvalPoint1
#define glEvalPoint2                        rglEvalPoint2
#define glFeedbackBuffer                    rglFeedbackBuffer
#define glFinish                            rglFinish
#define glFlush                             rglFlush
#define glFogf                              rglFogf
#define glFogfv                             rglFogfv
#define glFogi                              rglFogi
#define glFogiv                             rglFogiv
#define glFrontFace                         rglFrontFace
#define glFrustum                           rglFrustum
#define glGenLists                          rglGenLists
#define glGetBooleanv                       rglGetBooleanv
#define glGetClipPlane                      rglGetClipPlane
#define glGetDoublev                        rglGetDoublev
#define glGetError                          rglGetError
#define glGetFloatv                         rglGetFloatv
#define glGetIntegerv                       rglGetIntegerv
#define glGetLightfv                        rglGetLightfv
#define glGetLightiv                        rglGetLightiv
#define glGetMapdv                          rglGetMapdv
#define glGetMapfv                          rglGetMapfv
#define glGetMapiv                          rglGetMapiv
#define glGetMaterialfv                     rglGetMaterialfv
#define glGetMaterialiv                     rglGetMaterialiv
#define glGetPixelMapfv                     rglGetPixelMapfv
#define glGetPixelMapuiv                    rglGetPixelMapuiv
#define glGetPixelMapusv                    rglGetPixelMapusv
#define glGetPolygonStipple                 rglGetPolygonStipple
#define glGetString                         rglGetString
#define glGetTexEnvfv                       rglGetTexEnvfv
#define glGetTexEnviv                       rglGetTexEnviv
#define glGetTexGendv                       rglGetTexGendv
#define glGetTexGenfv                       rglGetTexGenfv
#define glGetTexGeniv                       rglGetTexGeniv
#define glGetTexImage                       rglGetTexImage
#define glGetTexLevelParameterfv            rglGetTexLevelParameterfv
#define glGetTexLevelParameteriv            rglGetTexLevelParameteriv
#define glGetTexParameterfv                 rglGetTexParameterfv
#define glGetTexParameteriv                 rglGetTexParameteriv
#define glHint                              rglHint
#define glIndexMask                         rglIndexMask
#define glIndexd                            rglIndexd
#define glIndexdv                           rglIndexdv
#define glIndexf                            rglIndexf
#define glIndexfv                           rglIndexfv
#define glIndexi                            rglIndexi
#define glIndexiv                           rglIndexiv
#define glIndexs                            rglIndexs
#define glIndexsv                           rglIndexsv
#define glInitNames                         rglInitNames
#define glIsEnabled                         rglIsEnabled
#define glIsList                            rglIsList
#define glLightModelf                       rglLightModelf
#define glLightModelfv                      rglLightModelfv
#define glLightModeli                       rglLightModeli
#define glLightModeliv                      rglLightModeliv
#define glLightf                            rglLightf
#define glLightfv                           rglLightfv
#define glLighti                            rglLighti
#define glLightiv                           rglLightiv
#define glLineStipple                       rglLineStipple
#define glLineWidth                         rglLineWidth
#define glListBase                          rglListBase
#define glLoadIdentity                      rglLoadIdentity
#define glLoadMatrixd                       rglLoadMatrixd
#define glLoadMatrixf                       rglLoadMatrixf
#define glLoadName                          rglLoadName
#define glLogicOp                           rglLogicOp
#define glMap1d                             rglMap1d
#define glMap1f                             rglMap1f
#define glMap2d                             rglMap2d
#define glMap2f                             rglMap2f
#define glMapGrid1d                         rglMapGrid1d
#define glMapGrid1f                         rglMapGrid1f
#define glMapGrid2d                         rglMapGrid2d
#define glMapGrid2f                         rglMapGrid2f
#define glMaterialf                         rglMaterialf
#define glMaterialfv                        rglMaterialfv
#define glMateriali                         rglMateriali
#define glMaterialiv                        rglMaterialiv
#define glMatrixMode                        rglMatrixMode
#define glMultMatrixd                       rglMultMatrixd
#define glMultMatrixf                       rglMultMatrixf
#define glNewList                           rglNewList
#define glNormal3b                          rglNormal3b
#define glNormal3bv                         rglNormal3bv
#define glNormal3d                          rglNormal3d
#define glNormal3dv                         rglNormal3dv
#define glNormal3f                          rglNormal3f
#define glNormal3fv                         rglNormal3fv
#define glNormal3i                          rglNormal3i
#define glNormal3iv                         rglNormal3iv
#define glNormal3s                          rglNormal3s
#define glNormal3sv                         rglNormal3sv
#define glOrtho                             rglOrtho
#define glPassThrough                       rglPassThrough
#define glPixelMapfv                        rglPixelMapfv
#define glPixelMapuiv                       rglPixelMapuiv
#define glPixelMapusv                       rglPixelMapusv
#define glPixelStoref                       rglPixelStoref
#define glPixelStorei                       rglPixelStorei
#define glPixelTransferf                    rglPixelTransferf
#define glPixelTransferi                    rglPixelTransferi
#define glPixelZoom                         rglPixelZoom
#define glPointSize                         rglPointSize
#define glPolygonMode                       rglPolygonMode
#define glPolygonStipple                    rglPolygonStipple
#define glPopAttrib                         rglPopAttrib
#define glPopMatrix                         rglPopMatrix
#define glPopName                           rglPopName
#define glPushAttrib                        rglPushAttrib
#define glPushMatrix                        rglPushMatrix
#define glPushName                          rglPushName
#define glRasterPos2d                       rglRasterPos2d
#define glRasterPos2dv                      rglRasterPos2dv
#define glRasterPos2f                       rglRasterPos2f
#define glRasterPos2fv                      rglRasterPos2fv
#define glRasterPos2i                       rglRasterPos2i
#define glRasterPos2iv                      rglRasterPos2iv
#define glRasterPos2s                       rglRasterPos2s
#define glRasterPos2sv                      rglRasterPos2sv
#define glRasterPos3d                       rglRasterPos3d
#define glRasterPos3dv                      rglRasterPos3dv
#define glRasterPos3f                       rglRasterPos3f
#define glRasterPos3fv                      rglRasterPos3fv
#define glRasterPos3i                       rglRasterPos3i
#define glRasterPos3iv                      rglRasterPos3iv
#define glRasterPos3s                       rglRasterPos3s
#define glRasterPos3sv                      rglRasterPos3sv
#define glRasterPos4d                       rglRasterPos4d
#define glRasterPos4dv                      rglRasterPos4dv
#define glRasterPos4f                       rglRasterPos4f
#define glRasterPos4fv                      rglRasterPos4fv
#define glRasterPos4i                       rglRasterPos4i
#define glRasterPos4iv                      rglRasterPos4iv
#define glRasterPos4s                       rglRasterPos4s
#define glRasterPos4sv                      rglRasterPos4sv
#define glReadBuffer                        rglReadBuffer
#define glReadPixels                        rglReadPixels
#define glRectd                             rglRectd
#define glRectdv                            rglRectdv
#define glRectf                             rglRectf
#define glRectfv                            rglRectfv
#define glRecti                             rglRecti
#define glRectiv                            rglRectiv
#define glRects                             rglRects
#define glRectsv                            rglRectsv
#define glRenderMode                        rglRenderMode
#define glRotated                           rglRotated
#define glRotatef                           rglRotatef
#define glScaled                            rglScaled
#define glScalef                            rglScalef
#define glScissor                           rglScissor
#define glSelectBuffer                      rglSelectBuffer
#define glShadeModel                        rglShadeModel
#define glStencilFunc                       rglStencilFunc
#define glStencilMask                       rglStencilMask
#define glStencilOp                         rglStencilOp
#define glTexCoord1d                        rglTexCoord1d
#define glTexCoord1dv                       rglTexCoord1dv
#define glTexCoord1f                        rglTexCoord1f
#define glTexCoord1fv                       rglTexCoord1fv
#define glTexCoord1i                        rglTexCoord1i
#define glTexCoord1iv                       rglTexCoord1iv
#define glTexCoord1s                        rglTexCoord1s
#define glTexCoord1sv                       rglTexCoord1sv
#define glTexCoord2d                        rglTexCoord2d
#define glTexCoord2dv                       rglTexCoord2dv
#define glTexCoord2f                        rglTexCoord2f
#define glTexCoord2fv                       rglTexCoord2fv
#define glTexCoord2i                        rglTexCoord2i
#define glTexCoord2iv                       rglTexCoord2iv
#define glTexCoord2s                        rglTexCoord2s
#define glTexCoord2sv                       rglTexCoord2sv
#define glTexCoord3d                        rglTexCoord3d
#define glTexCoord3dv                       rglTexCoord3dv
#define glTexCoord3f                        rglTexCoord3f
#define glTexCoord3fv                       rglTexCoord3fv
#define glTexCoord3i                        rglTexCoord3i
#define glTexCoord3iv                       rglTexCoord3iv
#define glTexCoord3s                        rglTexCoord3s
#define glTexCoord3sv                       rglTexCoord3sv
#define glTexCoord4d                        rglTexCoord4d
#define glTexCoord4dv                       rglTexCoord4dv
#define glTexCoord4f                        rglTexCoord4f
#define glTexCoord4fv                       rglTexCoord4fv
#define glTexCoord4i                        rglTexCoord4i
#define glTexCoord4iv                       rglTexCoord4iv
#define glTexCoord4s                        rglTexCoord4s
#define glTexCoord4sv                       rglTexCoord4sv
#define glTexEnvf                           rglTexEnvf
#define glTexEnvfv                          rglTexEnvfv
#define glTexEnvi                           rglTexEnvi
#define glTexEnviv                          rglTexEnviv
#define glTexGend                           rglTexGend
#define glTexGendv                          rglTexGendv
#define glTexGenf                           rglTexGenf
#define glTexGenfv                          rglTexGenfv
#define glTexGeni                           rglTexGeni
#define glTexGeniv                          rglTexGeniv
#define glTexImage1D                        rglTexImage1D
#define glTexImage2D                        rglTexImage2D
#define glTexParameterf                     rglTexParameterf
#define glTexParameterfv                    rglTexParameterfv
#define glTexParameteri                     rglTexParameteri
#define glTexParameteriv                    rglTexParameteriv
#define glTranslated                        rglTranslated
#define glTranslatef                        rglTranslatef
#define glVertex2d                          rglVertex2d
#define glVertex2dv                         rglVertex2dv
#define glVertex2f                          rglVertex2f
#define glVertex2fv                         rglVertex2fv
#define glVertex2i                          rglVertex2i
#define glVertex2iv                         rglVertex2iv
#define glVertex2s                          rglVertex2s
#define glVertex2sv                         rglVertex2sv
#define glVertex3d                          rglVertex3d
#define glVertex3dv                         rglVertex3dv
#define glVertex3f                          rglVertex3f
#define glVertex3fv                         rglVertex3fv
#define glVertex3i                          rglVertex3i
#define glVertex3iv                         rglVertex3iv
#define glVertex3s                          rglVertex3s
#define glVertex3sv                         rglVertex3sv
#define glVertex4d                          rglVertex4d
#define glVertex4dv                         rglVertex4dv
#define glVertex4f                          rglVertex4f
#define glVertex4fv                         rglVertex4fv
#define glVertex4i                          rglVertex4i
#define glVertex4iv                         rglVertex4iv
#define glVertex4s                          rglVertex4s
#define glVertex4sv                         rglVertex4sv
#define glViewport                          rglViewport

// GL 1.1
#define glAreTexturesResident               rglAreTexturesResident
#define glArrayElement                      rglArrayElement
#define glBindTexture                       rglBindTexture
#define glColorPointer                      rglColorPointer
#define glCopyTexImage1D                    rglCopyTexImage1D
#define glCopyTexImage2D                    rglCopyTexImage2D
#define glCopyTexSubImage1D                 rglCopyTexSubImage1D
#define glCopyTexSubImage2D                 rglCopyTexSubImage2D
#define glDeleteTextures                    rglDeleteTextures
#define glDisableClientState                rglDisableClientState
#define glDrawArrays                        rglDrawArrays
#define glDrawElements                      rglDrawElements
#define glEdgeFlagPointer                   rglEdgeFlagPointer
#define glEnableClientState                 rglEnableClientState
#define glGenTextures                       rglGenTextures
#define glGetPointerv                       rglGetPointerv
#define glIndexPointer                      rglIndexPointer
#define glIndexub                           rglIndexub
#define glIndexubv                          rglIndexubv
#define glInterleavedArrays                 rglInterleavedArrays
#define glIsTexture                         rglIsTexture
#define glNormalPointer                     rglNormalPointer
#define glPolygonOffset                     rglPolygonOffset
#define glPopClientAttrib                   rglPopClientAttrib
#define glPrioritizeTextures                rglPrioritizeTextures
#define glPushClientAttrib                  rglPushClientAttrib
#define glTexCoordPointer                   rglTexCoordPointer
#define glTexSubImage1D                     rglTexSubImage1D
#define glTexSubImage2D                     rglTexSubImage2D
#define glVertexPointer                     rglVertexPointer

# include <GL/RegalGLEW.h>
# include <GL/Regal.h>
# ifdef GEM_HAVE_GLU
#  include <GL/RegalGLU.h>
# endif
# include <SDL/SDL_opengl.h>
#else
# include "Gem/glew.h"
#endif

#ifdef __APPLE__
# include <OpenGL/OpenGL.h>
#elif defined(__EMSCRIPTEN__)
#elif defined _WIN32
# include "Gem/wglew.h"
#elif defined(__linux__) || defined(__FreeBSD_kernel__)
# include "Gem/glxew.h"
#endif /* OS */

#ifdef GEM_MULTICONTEXT
GEM_EXTERN GLEWContext*glewGetContext(void);
# ifdef __APPLE__
# elif defined _WIN32
GEM_EXTERN WGLEWContext*wglewGetContext(void);
# elif defined __linux__ || defined HAVE_GL_GLX_H
GEM_EXTERN GLXEWContext*glxewGetContext(void);
# endif

#endif /* GEM_MULTICONTEXT */

#ifndef GL_YUV422_GEM
# define GL_YCBCR_422_GEM GL_YCBCR_422_APPLE
# define GL_YUV422_GEM GL_YCBCR_422_GEM
#endif /* GL_YUV422_GEM */


#ifndef GL_RGBA_GEM
# ifdef __APPLE__
#  define GL_RGBA_GEM GL_BGRA_EXT
# else
#  define GL_RGBA_GEM GL_RGBA
# endif
#endif /* GL_RGBA_GEM */

/* default draw-style */
#ifndef GL_DEFAULT_GEM
# define GL_DEFAULT_GEM 0xFFFF
#endif



/* uäh: in OSX10.3 we only have CGL-1.1 and
 * all the functions are using "long*" rather than "GLint*")
 * only CGL-1.2 got it right
 */
#ifdef CGL_VERSION_1_0
# ifdef CGL_VERSION_1_2
#  define GemCGLint GLint
# else
#  define GemCGLint long
# endif
#endif


// missing from Emscripten's glew implementation
#ifndef GL_TEXTURE_RECTANGLE_EXT
# define GL_TEXTURE_RECTANGLE_EXT 0x84F5
#endif

#endif /* _INCLUDE__GEM_GEM_GEMGL_H_ */
