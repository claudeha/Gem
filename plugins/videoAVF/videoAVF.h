/*-----------------------------------------------------------------

  GEM - Graphics Environment for Multimedia

  capture live video into a pix block on macOS 10.7+

  Copyright (c) 2016-2018 Dan Wilcox.
  Copyright (c) 2018 IOhannes m zmölnig.

  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

  -----------------------------------------------------------------*/
#ifndef _INCLUDE_GEMPLUGIN__VIDEOAVF_VIDEOAVF_H_
#define _INCLUDE_GEMPLUGIN__VIDEOAVF_VIDEOAVF_H_

#include "plugins/video.h"
#include "Gem/Image.h"

// forward declare PIMPL types, depending on the compiler
#ifdef __OBJC__
@class AVFVideoGrabber;
#else
typedef struct objc_object AVFVideoGrabber;
#endif

/*-----------------------------------------------------------------
  -------------------------------------------------------------------
  CLASS
  videoAVF

  Loads in a video on macOS using the AVFoundation framework

  KEYWORDS
  pix

  DESCRIPTION

  -----------------------------------------------------------------*/
namespace gem
{
namespace plugins
{
class GEM_EXPORT videoAVF : public video
{

public:

  //////////
  // Constructor
  videoAVF(void);

  //////////
  // Destructor
  virtual ~videoAVF(void);

  //////////
  // enumerate devices
  virtual std::vector<std::string>enumerate(void);
  virtual bool              setDevice(int ID);
  virtual bool              setDevice(const std::string&);

  //! open the device (calls openDevice())
  virtual bool open(gem::Properties&props);
  //! start the transmission (calls startTransfer())
  virtual bool start(void);

  //! get the next frame
  virtual pixBlock *getFrame(void);
  //! release the frame retrieved by the last getFrame() call
  virtual void releaseFrame(void);

  //! stop the transmission (calls stopTransfer())
  virtual bool stop(void);
  //! close the device (calls closeDevice())
  virtual void close(void);

  //! reset the backend
  virtual bool          reset(void);

  //! property handling
  virtual bool enumProperties(gem::Properties&readable,
                              gem::Properties&writeable);
  virtual void setProperties(gem::Properties&props);
  virtual void getProperties(gem::Properties&props);

  //! dialog handling
  virtual bool dialog(std::vector<std::string>names=
                        std::vector<std::string>());
  virtual std::vector<std::string>dialogs(void);

  virtual bool isThreadable(void)
  {
    return true;
  }
  virtual bool grabAsynchronous(bool);

  virtual bool setColor(int);
  virtual bool provides(const std::string&);
  // get a list of all provided devices
  virtual std::vector<std::string>provides(void);
  virtual const std::string getName(void);

  void updatePixels();
protected:
  std::string m_name;
  int m_device;
  std::vector<std::string>m_devices;

  gem::Properties m_props;

  // PIMPL pointer to the Obj-C object behind this class
  AVFVideoGrabber* m_videoGrabber;

  pixBlock m_image;             //< frame storage
  GLenum m_wantedFormat;        //< requested color space

  bool newFrame;
  bool bHavePixelsChanged;
  int width, height;

  int device;
  bool bIsInit;
  int fps;
  bool bLock;
  //std::mutex = capMutex;
  static void updatePixelsCB(videoAVF*);
};
};
};

#endif // header guard