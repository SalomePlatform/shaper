
#ifndef XGUI_Viewer_H
#define XGUI_Viewer_H

#include <QObject>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Trihedron.hxx>

class XGUI_MainWindow;
class QMdiSubWindow;

class XGUI_Viewer : public QObject
{
    Q_OBJECT
public:
    enum { 
        HorizontalGradient, VerticalGradient,
        Diagonal1Gradient,  Diagonal2Gradient,
        Corner1Gradient,    Corner2Gradient,
        Corner3Gradient,    Corner4Gradient,
        LastGradient = Corner4Gradient
    };

    XGUI_Viewer(XGUI_MainWindow* theParent);
    ~XGUI_Viewer();

    QMdiSubWindow* createView(V3d_TypeOfView theType = V3d_ORTHOGRAPHIC);

    XGUI_MainWindow* mainWindow() const { return myMainWindow; }

    Handle(V3d_Viewer) v3dViewer() const { return myV3dViewer; }

private:
    XGUI_MainWindow* myMainWindow;

    Handle(V3d_Viewer)              myV3dViewer;
    Handle(AIS_Trihedron)           myTrihedron;
    Handle(AIS_InteractiveContext)  myAISContext;
};

#endif