#ifndef XGUI_Constants_H
#define XGUI_Constants_H

#include <QList>
#include <ModelAPI_Feature.h>

//! This file contains various constants used in the application


typedef QList<int> QIntList;       //!< list of int values
typedef QList<short> QShortList;     //!< list of short int values
typedef QList<double> QDoubleList;    //!< list of double values

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_Feature> FeaturePtr;
typedef QList<FeaturePtr> QFeatureList; //!< List of features

namespace XGUI
{

//! Types of gradient type used in background of Viewer 3d
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

//! Type of rotation point in viewer 3d
enum RotationPointType
{
  GRAVITY, SELECTED
};

//! Type of ribbon rect in Viewer 3d
enum SketchingType
{
  NoSketching, Rect, Polygon
};


//! View window operations accessible by hot keys
enum HotOperation
{
  PAN, ZOOM, ROTATE, FIT_AREA
};

//! Types of view window interactions
enum InteractionStyle
{
  STANDARD, KEY_FREE
};

//! Types of 2d mode in viewer 3d
enum Mode2dType
{
  No2dMode, XYPlane, XZPlane, YZPlane
};

//! Types of background in view window
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

const static char* PROP_PANEL = "property_panel_dock";
const static char* PROP_PANEL_OK = "property_panel_ok";
const static char* PROP_PANEL_CANCEL = "property_panel_cancel";
const static char* PROP_PANEL_WDG = "property_panel_widget";

};

#endif
