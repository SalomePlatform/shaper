#include "XGUI_Viewer.h"
#include "XGUI_MainWindow.h"
#include "XGUI_ViewWindow.h"

#include <QMdiArea>
#include <QMdiSubWindow>

#include <V3d_View.hxx>

#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#ifdef WIN32
#include <WNT_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif


/*!
    Creates viewer 3d [ static ]
*/
Handle(V3d_Viewer) CreateViewer( const Standard_ExtString name,
						     const Standard_CString displayName,
						     const Standard_CString domain,
						     const Standard_Real viewSize ,
						     const V3d_TypeOfOrientation viewProjection,
						     const Standard_Boolean computedMode,
						     const Standard_Boolean defaultComputedMode )
{
  static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
  if (aGraphicDriver.IsNull())
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#ifndef WIN32
    aDisplayConnection = new Aspect_DisplayConnection( displayName );
#else
    aDisplayConnection = new Aspect_DisplayConnection();
#endif
    aGraphicDriver = Graphic3d::InitGraphicDriver( aDisplayConnection );
  }

  return new V3d_Viewer( aGraphicDriver, name, domain, viewSize, viewProjection,
			 Quantity_NOC_GRAY30, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
			 computedMode, defaultComputedMode, V3d_TEX_NONE );
}



XGUI_Viewer::XGUI_Viewer(XGUI_MainWindow* theParent) :
QObject(theParent), myMainWindow(theParent)
{
    // init CasCade viewers
    myV3dViewer = CreateViewer(TCollection_ExtendedString("Viewer3d").ToExtString(),
        					   "", "", 1000.0, V3d_XposYnegZpos, Standard_True, Standard_True );
    //myV3dViewer->Init(); // to avoid creation of the useless perspective view (see OCCT issue 0024267)
    myV3dViewer->SetDefaultLights();

    // init selector
    myAISContext = new AIS_InteractiveContext( myV3dViewer );
    myAISContext->SelectionColor( Quantity_NOC_WHITE );
  
    // display isoline on planar faces (box for ex.)
    myAISContext->IsoOnPlane( true );
}


XGUI_Viewer::~XGUI_Viewer(void)
{
}


QMdiSubWindow* XGUI_Viewer::createView(V3d_TypeOfView theType)
{
    // create view frame
    XGUI_ViewWindow* view = new XGUI_ViewWindow(this, theType);
    // get main view window (created by view frame)
    //OCCViewer_ViewWindow* vw = view->getView(OCCViewer_ViewFrame::MAIN_VIEW);
    // initialize main view window
    //initView( vw );
    // set default background for view window
    //vw->setBackground( background(0) ); // 0 means MAIN_VIEW (other views are not yet created here)
    //// connect signal from viewport
    //connect(view->getViewPort(), SIGNAL(vpClosed()), this, SLOT(onViewClosed()));
    //connect(view->getViewPort(), SIGNAL(vpMapped()), this, SLOT(onViewMapped()));

    QMdiArea* aMDI = myMainWindow->mdiArea();
    QMdiSubWindow* aWnd = aMDI->addSubWindow(view, Qt::FramelessWindowHint);
    aWnd->setGeometry(0,0, aMDI->width() / 2, aMDI->height() / 2);
    aWnd->show();
    return aWnd;
}