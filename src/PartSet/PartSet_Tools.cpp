// File:        PartSet_Tools.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Tools.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>

#include <XGUI_ViewerPrs.h>

#include <V3d_View.hxx>
#include <gp_Pln.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

const double PRECISION_TOLERANCE = 0.000001;

gp_Pnt PartSet_Tools::ConvertClickToPoint(QPoint thePoint, Handle(V3d_View) theView)
{
  if (theView.IsNull())
    return gp_Pnt();

  V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
  theView->Eye(XEye, YEye, ZEye);

  theView->At(XAt, YAt, ZAt);
  gp_Pnt EyePoint(XEye, YEye, ZEye);
  gp_Pnt AtPoint(XAt, YAt, ZAt);

  gp_Vec EyeVector(EyePoint, AtPoint);
  gp_Dir EyeDir(EyeVector);

  gp_Pln PlaneOfTheView = gp_Pln(AtPoint, EyeDir);
  Standard_Real X, Y, Z;
  theView->Convert(thePoint.x(), thePoint.y(), X, Y, Z);
  gp_Pnt ConvertedPoint(X, Y, Z);

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView, ConvertedPoint);
  gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(), PlaneOfTheView);
  return ResultPoint;
}

void PartSet_Tools::ConvertTo2D(const gp_Pnt& thePoint, boost::shared_ptr<ModelAPI_Feature> theSketch,
                                Handle(V3d_View) theView, double& theX, double& theY)
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

  gp_Pnt anOriginPnt(anOrigin->x(), anOrigin->y(), anOrigin->z());
  gp_Vec aVec(anOriginPnt, thePoint);

  if (!theView.IsNull())
  {
    V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
    theView->Eye(XEye, YEye, ZEye);

    theView->At(XAt, YAt, ZAt);
    gp_Pnt EyePoint(XEye, YEye, ZEye);
    gp_Pnt AtPoint(XAt, YAt, ZAt);

    gp_Vec anEyeVec(EyePoint, AtPoint);
    anEyeVec.Normalize();

    boost::shared_ptr<GeomDataAPI_Dir> aNormal = 
                  boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_NORM));
    gp_Vec aNormalVec(aNormal->x(), aNormal->y(), aNormal->z());

    double aDen = anEyeVec * aNormalVec;
    double aLVec = aDen != 0 ? aVec * aNormalVec / aDen : DBL_MAX;

    gp_Vec aDeltaVec = anEyeVec*aLVec;
    aVec = aVec - aDeltaVec;
  }
  theX = aVec.X() * aX->x() + aVec.Y() * aX->y() + aVec.Z() * aX->z();
  theY = aVec.X() * anY->x() + aVec.Y() * anY->y() + aVec.Z() * anY->z();
}

void PartSet_Tools::ConvertTo3D(const double theX, const double theY,
                                boost::shared_ptr<ModelAPI_Feature> theSketch,
                                gp_Pnt& thePoint)
{
  if (!theSketch)
    return;

  boost::shared_ptr<ModelAPI_Data> aData = theSketch->data();

  boost::shared_ptr<GeomDataAPI_Point> aC = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SKETCH_ATTR_ORIGIN));
  boost::shared_ptr<GeomDataAPI_Dir> aX = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRX));
  boost::shared_ptr<GeomDataAPI_Dir> aY = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRY));

  boost::shared_ptr<GeomAPI_XYZ> aSum = aC->pnt()->xyz()->added(
    aX->dir()->xyz()->multiplied(theX))->added(aY->dir()->xyz()->multiplied(theY));

  boost::shared_ptr<GeomAPI_Pnt> aPoint = boost::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSum));
  thePoint = gp_Pnt(aPoint->x(), aPoint->y(), aPoint->z());
}

void PartSet_Tools::IntersectLines(double theX0, double theY0, double theX1, double theY1,
                                   double theX2, double theY2, double theX3, double theY3,
                                   double& theX, double& theY)
{
  double aV1 = theX1 - theX0, aV2 = theY1 - theY0;
  double aW1 = theX3 - theX2, aW2 = theY3 - theY2;

  double aT2 = 0;
  if (aV1  != 0 && aV2 != 0)
    aT2 = (( theY2 - theY0 )/aV2 - ( theX2 - theX0 )/aV1) / ( aW1/aV1 - aW2/aV2 );
  else
    aT2 = DBL_MAX;

  theX  = theX2 + aT2*aW1;
  theY = theY2 + aT2*aW2;

  // the coordinates of two lines are on the common line
  //It is not possible to use Precision::Confusion(), because it is e-0.8, but V is sometimes e-6
  Standard_Real aPrec = PRECISION_TOLERANCE;
  if (fabs(theX - theX0) < aPrec && fabs(theY - theY0) < aPrec) {
    ProjectPointOnLine(theX2, theY2, theX3, theY3, theX1, theY1, theX, theY);    
  }
}

void PartSet_Tools::ProjectPointOnLine(double theX1, double theY1, double theX2, double theY2,
                                       double thePointX, double thePointY, double& theX, double& theY)
{
  theX = theY = 0;

  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(theX1, theY1, 0),
                                     gp_Dir(gp_Vec(gp_Pnt(theX1, theY1, 0), gp_Pnt(theX2, theY2, 0))));
  GeomAPI_ProjectPointOnCurve aProj(gp_Pnt(thePointX, thePointY, 0), aLine);

  Standard_Integer aNbPoint = aProj.NbPoints();
  if (aNbPoint > 0) {
    gp_Pnt aPoint = aProj.Point(1);
    theX = aPoint.X();
    theY = aPoint.Y();
  }
}

boost::shared_ptr<ModelAPI_Feature> PartSet_Tools::NearestFeature(QPoint thePoint,
                                                   Handle_V3d_View theView,
                                                   boost::shared_ptr<ModelAPI_Feature> theSketch,
                                                   const std::list<XGUI_ViewerPrs>& theFeatures)
{
  double aX, anY;
  gp_Pnt aPoint = PartSet_Tools::ConvertClickToPoint(thePoint, theView);
  PartSet_Tools::ConvertTo2D(aPoint, theSketch, theView, aX, anY);

  boost::shared_ptr<ModelAPI_Feature> aFeature;
  std::list<XGUI_ViewerPrs>::const_iterator anIt = theFeatures.begin(), aLast = theFeatures.end();

  boost::shared_ptr<ModelAPI_Feature> aDeltaFeature;   
  double aMinDelta = -1;
  XGUI_ViewerPrs aPrs;
  for (; anIt != aLast; anIt++) {
    aPrs = *anIt;
    if (!aPrs.feature())
      continue;
    double aDelta = DistanceToPoint(aPrs.feature(), aX, anY);
    if (aMinDelta < 0 || aMinDelta > aDelta) {
      aMinDelta = aDelta;
      aDeltaFeature = aPrs.feature();
    }
  }
  return aDeltaFeature;
}

double PartSet_Tools::DistanceToPoint(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                      double theX, double theY)
{
  double aDelta = 0;
  if (theFeature->getKind() != "SketchLine")
    return aDelta;

  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));

  double aX, anY;
  PartSet_Tools::ProjectPointOnLine(aPoint1->x(), aPoint1->y(), aPoint2->x(), aPoint2->y(), theX, theY, aX, anY);

  aDelta = gp_Pnt(theX, theY, 0).Distance(gp_Pnt(aX, anY, 0));

  return aDelta;
}
