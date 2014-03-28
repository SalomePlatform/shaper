
#ifndef XGUI_Constants_H
#define XGUI_Constants_H

namespace XGUI
{

enum GradientType { 
    HorizontalGradient, VerticalGradient,
    Diagonal1Gradient,  Diagonal2Gradient,
    Corner1Gradient,    Corner2Gradient,
    Corner3Gradient,    Corner4Gradient,
    LastGradient = Corner4Gradient
};

enum RotationPointType{ GRAVITY, SELECTED };

enum SketchingType { NoSketching, Rect, Polygon };

enum HotOperation { PAN, ZOOM, ROTATE, FIT_AREA };

enum InteractionStyle { STANDARD, KEY_FREE };

enum Mode2dType { No2dMode, XYPlane, XZPlane, YZPlane};

};

#endif