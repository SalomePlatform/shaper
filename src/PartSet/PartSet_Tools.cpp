// File:        PartSet_Tools.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Tools.h>

#include <V3d_View.hxx>
#include <gp_Pln.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <SketchPlugin_Sketch.h>

#ifdef _DEBUG
#include <QDebug>
#endif

gp_Pnt PartSet_Tools::ConvertClickToPoint(QPoint thePoint, Handle(V3d_View) theView)
{
  if (theView.IsNull())
    return gp_Pnt();

  V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
  theView->Eye( XEye, YEye, ZEye );

  theView->At( XAt, YAt, ZAt );
  gp_Pnt EyePoint( XEye, YEye, ZEye );
  gp_Pnt AtPoint( XAt, YAt, ZAt );

  gp_Vec EyeVector( EyePoint, AtPoint );
  gp_Dir EyeDir( EyeVector );

  gp_Pln PlaneOfTheView = gp_Pln( AtPoint, EyeDir );
  Standard_Real X, Y, Z;
  theView->Convert( thePoint.x(), thePoint.y(), X, Y, Z );
  gp_Pnt ConvertedPoint( X, Y, Z );

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project( PlaneOfTheView, ConvertedPoint );
  gp_Pnt ResultPoint = ElSLib::Value( ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(), PlaneOfTheView );
  return ResultPoint;
}

void PartSet_Tools::ConvertTo2D(const gp_Pnt& thePoint, boost::shared_ptr<ModelAPI_Feature> theSketch,
                                double& theX, double& theY)
{
  if (!theSketch)
    return;

  boost::shared_ptr<ModelAPI_AttributeDouble> anAttr;
  boost::shared_ptr<ModelAPI_Data> aData = theSketch->data();

  boost::shared_ptr<GeomDataAPI_Point> anOrigin = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SKETCH_ATTR_ORIGIN));

  boost::shared_ptr<GeomDataAPI_Dir> aX = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRX));
  boost::shared_ptr<GeomDataAPI_Dir> anY = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRY));

  gp_Pnt aVec(thePoint.X() - anOrigin->x(), thePoint.Y() - anOrigin->y(), thePoint.Z() - anOrigin->z());
  theX = aVec.X() * aX->x() + aVec.Y() * aX->y() + aVec.Z() * aX->z();
  theY = aVec.X() * anY->x() + aVec.Y() * anY->y() + aVec.Z() * anY->z();
}
