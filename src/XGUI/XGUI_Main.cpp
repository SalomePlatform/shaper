#include "XGUI_Workshop.h"
#include <QApplication>
#include <QTranslator>

#ifndef WIN32

#include <X11/Xlib.h>
#include <GL/glx.h>

/*!
  \brief Open the default X display and returns pointer to it.
         This method is available on Linux only.
  \return Pointer to X display.
  \sa getVisual()
*/
void* getDisplay()
{
  static Display* pDisplay = NULL;
  if ( !pDisplay )
    pDisplay = XOpenDisplay( NULL );
  return pDisplay;
}

/*!
  \brief Returns pointer to X visual suitable for 3D rendering.
         This method is available on Linux only.
  \return Pointer to X visual.
  \sa getDisplay()
*/
Qt::HANDLE getVisual()
{
  Qt::HANDLE res = (Qt::HANDLE)NULL;

  Display* pDisplay = (Display*)getDisplay();
  if ( !pDisplay )
    return res;

  int errorBase;
  int eventBase;

  // Make sure OpenGL's GLX extension supported
  if( !glXQueryExtension( pDisplay, &errorBase, &eventBase ) ){
    qCritical( "Could not find glx extension" );
    return res;
  }

  // Find an appropriate visual

  int doubleBufferVisual[]  = {
    GLX_RGBA,           // Needs to support OpenGL
    GLX_DEPTH_SIZE, 16, // Needs to support a 16 bit depth buffer
    GLX_DOUBLEBUFFER,   // Needs to support double-buffering
    None                // end of list
  };

  // Try for the double-bufferd visual first
  XVisualInfo *visualInfo = NULL;
  visualInfo = glXChooseVisual( pDisplay, DefaultScreen(pDisplay), doubleBufferVisual );

  if( visualInfo == NULL ){
    qCritical( "Could not find matching glx visual" );
    return res;
  }

  qDebug() << "Picked visual 0x" << hex << XVisualIDFromVisual( visualInfo->visual );
  res = (Qt::HANDLE)( visualInfo->visual );
 
  return res;
}
#endif // WIN32



int main(int argc, char *argv[])
{
#ifndef WIN32
  QApplication app(getDisplay(), argc, argv, getVisual());
#else
  QApplication app(argc, argv);
#endif

  // Install translator
  QTranslator aTranslator;
  aTranslator.load(app.applicationDirPath() + "\\XGUI_msg_en.qm");
  app.installTranslator(&aTranslator);

  XGUI_Workshop aWorkshop;
  aWorkshop.startApplication();

  return app.exec();
}
