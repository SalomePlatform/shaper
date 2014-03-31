#ifndef XGUI_Constants_H
#define XGUI_Constants_H

#include <QList>

typedef QList<int> QIntList;       //!< list of int values
typedef QList<short> QShortList;     //!< list of short int values
typedef QList<double> QDoubleList;    //!< list of double values

namespace XGUI
{

enum GradientType
{
  NoGradient = -1,
  HorizontalGradient,
  VerticalGradient,
  Diagonal1Gradient,
  Diagonal2Gradient,
  Corner1Gradient,
  Corner2Gradient,
  Corner3Gradient,
  Corner4Gradient,
  LastGradient = Corner4Gradient
};

enum RotationPointType
{
  GRAVITY, SELECTED
};

enum SketchingType
{
  NoSketching, Rect, Polygon
};

enum HotOperation
{
  PAN, ZOOM, ROTATE, FIT_AREA
};

enum InteractionStyle
{
  STANDARD, KEY_FREE
};

enum Mode2dType
{
  No2dMode, XYPlane, XZPlane, YZPlane
};

enum BackgroundMode
{
  NoBackground,              // no (invalid) background data
  ColorBackground,           // single color
  SimpleGradientBackground,  // simple two-color gradient
  CustomGradientBackground   // custom (complex) gradient
};

//! Texture mode
enum TextureMode
{
  CenterTexture,             // center texture
  TileTexture,               // tile texture
  StretchTexture,            // stretch texture
};

}
;

#endif
