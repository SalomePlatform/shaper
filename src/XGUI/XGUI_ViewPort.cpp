#include "XGUI_ViewPort.h"
#include "XGUI_ViewWindow.h"
#include "XGUI_Viewer.h"
#include "XGUI_Constants.h"

#include <QPaintEvent>
#include <QPainter>
#include <QFileInfo>
#include <QApplication>

#include <V3d_OrthographicView.hxx>
#include <V3d_PerspectiveView.hxx>
#include <Visual3d_View.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#ifdef WIN32
#include <WNT_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif

#include <GL/gl.h>

static double rx = 0.;
static double ry = 0.;
static int sx = 0;
static int sy = 0;
static Standard_Boolean zRotation = Standard_False;

/*!
 Create native view window for CasCade view [ static ]
 */
Handle(Aspect_Window) CreateCasWindow(const Handle(V3d_View)& view, WId winId)
{
  Aspect_Handle aWindowHandle = (Aspect_Handle) winId;
#ifdef WIN32
  Handle(WNT_Window) viewWindow = new WNT_Window(aWindowHandle);
#else
  Handle(Aspect_DisplayConnection) aDispConnection = view->Viewer()->Driver()->GetDisplayConnection();
  Handle(Xw_Window) viewWindow = new Xw_Window( aDispConnection, aWindowHandle );
#endif
  return viewWindow;
}

//************************************************************************
//************************************************************************
//************************************************************************
XGUI_ViewPort::XGUI_ViewPort(XGUI_ViewWindow* theParent, const Handle(V3d_Viewer)& theViewer,
                             V3d_TypeOfView theType)
    : QWidget(theParent),
      myPaintersRedrawing(false),
      myScale(1.0),
      myIsAdvancedZoomingEnabled(false),
      myBgImgHeight(0),
      myBgImgWidth(0)
{
  setMouseTracking(true);
  setBackgroundRole(QPalette::NoRole);

  // set focus policy to threat QContextMenuEvent from keyboard  
  setFocusPolicy(Qt::StrongFocus);
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);
  setAutoFillBackground(false);

  if (theType == V3d_ORTHOGRAPHIC) {
    myOrthoView = new V3d_OrthographicView(theViewer);
    myActiveView = myOrthoView;
    myPerspView = 0;
  } else {
    myPerspView = new V3d_PerspectiveView(theViewer);
    myActiveView = myPerspView;
  }
  myActiveView->SetSurfaceDetail(V3d_TEX_ALL);
}

//***********************************************
XGUI_ViewPort::~XGUI_ViewPort()
{
}

//***********************************************
bool XGUI_ViewPort::mapView(const Handle(V3d_View)& theView)
{
  if (!setWindow(theView))
    return false;

  if (!mapped(theView)) {
    theView->SetWindow(myWindow);
    //if (theView != activeView())
    //theView->View()->Deactivate();
  }

  /* create static trihedron (16551: EDF PAL 501) */
  //OCCViewer_ViewWindow* aVW = dynamic_cast<OCCViewer_ViewWindow*>( parentWidget()->parentWidget()->parentWidget() );
  //if ( aVW ) {
  //    OCCViewer_Viewer* aViewModel = dynamic_cast<OCCViewer_Viewer*>( aVW->getViewManager()->getViewModel() );
  //    if ( aViewModel && aViewModel->isStaticTrihedronDisplayed() ){
  //theView->ZBufferTriedronSetup();
  theView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.05, V3d_ZBUFFER);
  //    }
  //}

  emit(vpMapped());

  return true;
}

//***********************************************
bool XGUI_ViewPort::setWindow(const Handle(V3d_View)& theView)
{
  if (!myWindow.IsNull())
    return true;

  if (theView.IsNull())
    return false;

  attachWindow(theView, CreateCasWindow(theView, winId()));

  myWindow = theView->Window();
  return !myWindow.IsNull();
}

//***********************************************
bool XGUI_ViewPort::mapped(const Handle(V3d_View)& theView) const
{
  return (!theView.IsNull() && theView->View()->IsDefined());
}

//***********************************************
void XGUI_ViewPort::updateBackground()
{
  if (activeView().IsNull())
    return;
  if (!myBackground.isValid())
    return;

  // VSR: Important note on below code.
  // In OCCT (in version 6.5.2), things about the background drawing
  // are not straightforward and not clearly understandable:
  // - Horizontal gradient is drawn vertically (!), well ok, from top side to bottom one.
  // - Vertical gradient is drawn horizontally (!), from right side to left one (!!!).
  // - First and second diagonal gradients are confused.
  // - Image texture, once set, can not be removed (!).
  // - Texture image fill mode Aspect_FM_NONE is not taken into account (and means the same
  //   as Aspect_FM_CENTERED).
  // - The only way to cancel gradient background (and get back to single colored) is to
  //   set gradient background style to Aspect_GFM_NONE while passing two colors is also needed
  //   (see V3d_View::SetBgGradientColors() function).
  // - Also, it is impossible to draw texture image above the gradiented background (only above
  //   single-colored).
  // In OCCT 6.5.3 all above mentioned problems are fixed; so, above comment should be removed as soon
  // as SALOME is migrated to OCCT 6.5.3. The same concerns #ifdef statements in the below code
  switch (myBackground.mode()) {
    case Qtx::ColorBackground: {
      QColor c = myBackground.color();
      if (c.isValid()) {
        // Unset texture should be done here
        // ...
        Quantity_Color qCol(c.red() / 255., c.green() / 255., c.blue() / 255., Quantity_TOC_RGB);
        activeView()->SetBgGradientStyle(Aspect_GFM_NONE);  // cancel gradient background
        activeView()->SetBgImageStyle(Aspect_FM_NONE);  // cancel texture background
        // then change background color
        activeView()->SetBackgroundColor(qCol);
        // update viewer
        activeView()->Update();
      }
      break;
    }
    case Qtx::SimpleGradientBackground: {
      QColor c1, c2;
      int type = myBackground.gradient(c1, c2);
      if (c1.isValid() && type >= XGUI::HorizontalGradient && type <= XGUI::LastGradient) {
        // Unset texture should be done here
        // ...
        // Get colors and set-up gradiented background
        if (!c2.isValid())
          c2 = c1;
        Quantity_Color qCol1(c1.red() / 255., c1.green() / 255., c1.blue() / 255.,
                             Quantity_TOC_RGB);
        Quantity_Color qCol2(c2.red() / 255., c2.green() / 255., c2.blue() / 255.,
                             Quantity_TOC_RGB);
        activeView()->SetBgImageStyle(Aspect_FM_NONE);  // cancel texture background
        switch (type) {
          case XGUI::HorizontalGradient:
            activeView()->SetBgGradientColors(qCol1, qCol2, Aspect_GFM_HOR,
            Standard_True);
            break;
          case XGUI::VerticalGradient:
            activeView()->SetBgGradientColors(qCol1, qCol2, Aspect_GFM_VER,
            Standard_True);
            break;
          case XGUI::Diagonal1Gradient:
            activeView()->SetBgGradientColors(qCol1, qCol2, Aspect_GFM_DIAG1,
            Standard_True);
            break;
          case XGUI::Diagonal2Gradient:
            activeView()->SetBgGradientColors(qCol1, qCol2, Aspect_GFM_DIAG2,
            Standard_True);
            break;
          case XGUI::Corner1Gradient:
            activeView()->SetBgGradientColors(qCol1, qCol2, Aspect_GFM_CORNER1,
            Standard_True);
            break;
          case XGUI::Corner2Gradient:
            activeView()->SetBgGradientColors(qCol1, qCol2, Aspect_GFM_CORNER2,
            Standard_True);
            break;
          case XGUI::Corner3Gradient:
            activeView()->SetBgGradientColors(qCol1, qCol2, Aspect_GFM_CORNER3,
            Standard_True);
            break;
          case XGUI::Corner4Gradient:
            activeView()->SetBgGradientColors(qCol1, qCol2, Aspect_GFM_CORNER4,
            Standard_True);
            break;
          default:
            break;
        }
      }
      break;
    }
    case Qtx::CustomGradientBackground:
      // NOT IMPLEMENTED YET
      break;
    default:
      break;
  }
  // VSR: In OCCT before v6.5.3 below code can't be used because of very ugly bug - it has been impossible to
  // clear the background texture image as soon as it was once set to the viewer.
  if (myBackground.isTextureShown()) {
    QString fileName;
    int textureMode = myBackground.texture(fileName);
    QFileInfo fi(fileName);
    if (!fileName.isEmpty() && fi.exists()) {
      // set texture image: file name and fill mode
      switch (textureMode) {
        case XGUI::CenterTexture:
          activeView()->SetBackgroundImage(fi.absoluteFilePath().toLatin1().constData(),
                                           Aspect_FM_CENTERED);
          break;
        case XGUI::TileTexture:
          activeView()->SetBackgroundImage(fi.absoluteFilePath().toLatin1().constData(),
                                           Aspect_FM_TILED);
          break;
        case XGUI::StretchTexture:
          activeView()->SetBackgroundImage(fi.absoluteFilePath().toLatin1().constData(),
                                           Aspect_FM_STRETCH);
          break;
        default:
          break;
      }
      activeView()->Update();
    }
  }
}

//***********************************************
void XGUI_ViewPort::attachWindow(const Handle(V3d_View)& theView,
                                 const Handle(Aspect_Window)& theWnd)
{
  if (!theView.IsNull()) {
    theView->SetWindow(theWnd);
    updateBackground();
  }
}

//***********************************************
void XGUI_ViewPort::paintEvent(QPaintEvent* theEvent)
{
#ifndef WIN32
  /* X11 : map before show doesn't work */
  if ( !mapped( activeView() ) )
  mapView( activeView() );
#endif
  if (!myWindow.IsNull()) {
    QApplication::syncX();
    QRect rc = theEvent->rect();
    //if ( !myPaintersRedrawing ) {
    //activeView()->Redraw();
    activeView()->Redraw(rc.x(), rc.y(), rc.width(), rc.height());
    emit vpUpdated();
    //}
  }
  //if ( myPaintersRedrawing ) {
  //    QPainter p( this );
  //    //emit vpDrawExternal( &p );
  //    myPaintersRedrawing = false;
  //}
}

//***********************************************
void XGUI_ViewPort::resizeEvent(QResizeEvent* theEvent)
{
#ifdef WIN32
  /* Win32 : map before first show to avoid flicker */
  if (!mapped(activeView()))
    mapView(activeView());
#endif
  QApplication::syncX();
  if (!activeView().IsNull()) {
    activeView()->MustBeResized();
  }
  emit resized();
}

//***********************************************
QImage XGUI_ViewPort::dumpView(unsigned char*& theData, QRect theRect, bool toUpdate)
{
  Handle(V3d_View) view = getView();
  if (view.IsNull())
    return QImage();

  int aWidth;
  int aHeight;
  if (theRect.isNull()) {
    aWidth = width();
    aHeight = height();
  } else {
    aWidth = theRect.width();
    aHeight = theRect.height();
  }
  QApplication::syncX();

  theData = new unsigned char[aWidth * aHeight * 4];

  QPoint p;
  if (theRect.isNull()) {
    if (toUpdate)
      view->Redraw();
    p = mapFromParent(geometry().topLeft());
  } else {
    if (toUpdate)
      view->Redraw(theRect.x(), theRect.y(), theRect.width(), theRect.height());
    p = theRect.topLeft();
  }
  glReadPixels(p.x(), p.y(), aWidth, aHeight, GL_RGBA, GL_UNSIGNED_BYTE, theData);

  QImage anImage(theData, aWidth, aHeight, QImage::Format_ARGB32);
  anImage = anImage.mirrored();
  anImage = anImage.rgbSwapped();
  return anImage;
}

/*!
 Inits 'rotation' transformation.
 */
void XGUI_ViewPort::startRotation(int x, int y, int theRotationPointType,
                                  const gp_Pnt& theSelectedPoint)
{
  if (!activeView().IsNull()) {
    switch (theRotationPointType) {
      case XGUI::GRAVITY:
        activeView()->StartRotation(x, y, 0.45);
        break;
      case XGUI::SELECTED:
        sx = x;
        sy = y;

        double X, Y;
        activeView()->Size(X, Y);
        rx = Standard_Real(activeView()->Convert(X));
        ry = Standard_Real(activeView()->Convert(Y));

        activeView()->Rotate(0., 0., 0., theSelectedPoint.X(), theSelectedPoint.Y(),
                             theSelectedPoint.Z(),
                             Standard_True);

        Quantity_Ratio zRotationThreshold;
        zRotation = Standard_False;
        zRotationThreshold = 0.45;
        if (zRotationThreshold > 0.) {
          Standard_Real dx = Abs(sx - rx / 2.);
          Standard_Real dy = Abs(sy - ry / 2.);
          Standard_Real dd = zRotationThreshold * (rx + ry) / 2.;
          if (dx > dd || dy > dd)
            zRotation = Standard_True;
        }
        break;
      default:
        break;
    }
    activeView()->DepthFitAll();
  }
}

/*!
 Rotates the viewport. 
 */
void XGUI_ViewPort::rotate(int x, int y, int theRotationPointType, const gp_Pnt& theSelectedPoint)
{
  if (!activeView().IsNull()) {
    switch (theRotationPointType) {
      case XGUI::GRAVITY:
        activeView()->Rotation(x, y);
        break;
      case XGUI::SELECTED:
        double dx, dy, dz;
        if (zRotation) {
          dz = atan2(Standard_Real(x) - rx / 2., ry / 2. - Standard_Real(y))
              - atan2(sx - rx / 2., ry / 2. - sy);
          dx = dy = 0.;
        } else {
          dx = (Standard_Real(x) - sx) * M_PI / rx;
          dy = (sy - Standard_Real(y)) * M_PI / ry;
          dz = 0.;
        }

        activeView()->Rotate(dx, dy, dz, theSelectedPoint.X(), theSelectedPoint.Y(),
                             theSelectedPoint.Z(),
                             Standard_False);
        break;
      default:
        break;
    }
    emit vpTransformed();
  }
  //  setZSize( getZSize() );
}

/*!
 Resets the viewport after 'rotation'. 
 */
void XGUI_ViewPort::endRotation()
{
  if (!activeView().IsNull()) {
    activeView()->ZFitAll(1.);
    activeView()->SetZSize(0.);
    activeView()->Update();
    emit vpTransformed();
  }
}

/*!
 Inits 'zoom' transformation.
 */
void XGUI_ViewPort::startZoomAtPoint(int x, int y)
{
  if (!activeView().IsNull()/* && isAdvancedZoomingEnabled() */)
    activeView()->StartZoomAtPoint(x, y);
}

/*!
 Centers the viewport. 
 */
void XGUI_ViewPort::setCenter(int x, int y)
{
  if (!activeView().IsNull()) {
    activeView()->Place(x, y, myScale);
    emit vpTransformed();
  }
}

/*!
 Called at 'pan' transformation. 
 */
void XGUI_ViewPort::pan(int dx, int dy)
{
  if (!activeView().IsNull()) {
    activeView()->Pan(dx, dy, 1.0);
    emit vpTransformed();
  }
}

/*!
 Called at 'window fit' transformation.
 */
void XGUI_ViewPort::fitRect(const QRect& rect)
{
  if (!activeView().IsNull()) {
    activeView()->WindowFit(rect.left(), rect.top(), rect.right(), rect.bottom());
    emit vpTransformed();
  }
}

/*!
 Called at 'zoom' transformation.
 */
void XGUI_ViewPort::zoom(int x0, int y0, int x, int y)
{
  if (!activeView().IsNull()) {
    if (isAdvancedZoomingEnabled())
      activeView()->ZoomAtPoint(x0, y0, x, y);
    else
      activeView()->Zoom(x0 + y0, 0, x + y, 0);
    emit vpTransformed();
  }
}

/*!
 Sets the background data
 */
void XGUI_ViewPort::setBackground(const Qtx::BackgroundData& bgData)
{
  if (bgData.isValid()) {
    myBackground = bgData;
    updateBackground();
    emit vpChangeBackground(myBackground);
  }
}

void XGUI_ViewPort::fitAll(bool theKeepScale, bool theWithZ, bool theUpd)
{
  if (activeView().IsNull())
    return;

  if (theKeepScale)
    myScale = activeView()->Scale();

  Standard_Real aMargin = 0.01;
  activeView()->FitAll(aMargin, theWithZ, theUpd);
  activeView()->SetZSize(0.);
  emit vpTransformed();
}

void XGUI_ViewPort::syncronizeWith(const XGUI_ViewPort* ref)
{
  Handle(V3d_View) refView = ref->getView();
  Handle(V3d_View) tgtView = getView();

  /*  The following params are copied:
   - view type( ortho/persp )
   - position of view point
   - orientation of high point
   - position of the eye
   - projection vector
   - view center ( 2D )
   - view twist
   - view scale
   */

  /* we'll update after setting all params */
  tgtView->SetImmediateUpdate( Standard_False);

  /* perspective */
  if (refView->Type() == V3d_PERSPECTIVE)
    tgtView->SetFocale(refView->Focale());

  /* copy params */
  Standard_Real x, y, z;
  refView->At(x, y, z);
  tgtView->SetAt(x, y, z);
  refView->Up(x, y, z);
  tgtView->SetUp(x, y, z);
  refView->Eye(x, y, z);
  tgtView->SetEye(x, y, z);
  refView->Proj(x, y, z);
  tgtView->SetProj(x, y, z);
  refView->Center(x, y);
  tgtView->SetCenter(x, y);
  tgtView->SetScale(refView->Scale());
  tgtView->SetTwist(refView->Twist());

  /* update */
  tgtView->Update();
  tgtView->SetImmediateUpdate( Standard_True);
}
