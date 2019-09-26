// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SketchAPI_Sketch.h"
//--------------------------------------------------------------------------------------
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintDistanceHorizontal.h>
#include <SketchPlugin_ConstraintDistanceVertical.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_Fillet.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_Trim.h>
#include <SketchPlugin_Split.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketcherPrs_Tools.h>
//--------------------------------------------------------------------------------------
#include <ModelAPI_Events.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_RefAttr.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
#include "SketchAPI_Arc.h"
#include "SketchAPI_Circle.h"
#include "SketchAPI_Ellipse.h"
#include "SketchAPI_EllipticArc.h"
#include "SketchAPI_IntersectionPoint.h"
#include "SketchAPI_Line.h"
#include "SketchAPI_MacroArc.h"
#include "SketchAPI_MacroCircle.h"
#include "SketchAPI_MacroEllipse.h"
#include "SketchAPI_MacroEllipticArc.h"
#include "SketchAPI_Mirror.h"
#include "SketchAPI_Point.h"
#include "SketchAPI_Projection.h"
#include "SketchAPI_Rectangle.h"
#include "SketchAPI_Rotation.h"
#include "SketchAPI_Translation.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Curve.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_XY.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <cmath>
//--------------------------------------------------------------------------------------
SketchAPI_Sketch::SketchAPI_Sketch(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_Sketch::SketchAPI_Sketch(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::shared_ptr<GeomAPI_Ax3> & thePlane)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setPlane(thePlane);
  }
}

SketchAPI_Sketch::SketchAPI_Sketch(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternal)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setExternal(theExternal);
  }
}

SketchAPI_Sketch::SketchAPI_Sketch(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    std::shared_ptr<ModelAPI_Object> thePlaneObject)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setExternal(thePlaneObject);
  }
}

SketchAPI_Sketch::~SketchAPI_Sketch()
{

}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_CompositeFeature> SketchAPI_Sketch::compositeFeature() const
{
  return std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(feature());
}

//--------------------------------------------------------------------------------------
void SketchAPI_Sketch::setPlane(const std::shared_ptr<GeomAPI_Ax3> & thePlane)
{
  fillAttribute(thePlane->origin(), myorigin);
  fillAttribute(thePlane->dirX(), mydirX);
  fillAttribute(thePlane->normal(), mynormal);

  execute();
}

void SketchAPI_Sketch::setPlane(const ModelHighAPI_Selection & thePlane,
                                bool theRemoveExternalDependency)
{
  FeaturePtr aSketch = feature();

  DocumentPtr aDoc = aSketch->document();
  bool useVisible = false;
  FeaturePtr aCurFeatureBefore = aDoc->currentFeature(useVisible);
  aDoc->setCurrentFeature(aSketch, useVisible);

  if (theRemoveExternalDependency)
    aSketch->customAction(SketchPlugin_Sketch::ACTION_REMOVE_EXTERNAL());

  setExternal(thePlane);

  aDoc->setCurrentFeature(aCurFeatureBefore, useVisible);
}

//--------------------------------------------------------------------------------------
void SketchAPI_Sketch::setExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, myexternal);

  execute();
}

void SketchAPI_Sketch::setExternal(std::shared_ptr<ModelAPI_Object> thePlaneObject)
{
  ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(thePlaneObject);
  ModelHighAPI_Selection aSel(aRes);
  setExternal(aSel);
}

//--------------------------------------------------------------------------------------
void SketchAPI_Sketch::setValue(
    const std::shared_ptr<ModelHighAPI_Interface> & theConstraint,
    const ModelHighAPI_Double & theValue)
{
  fillAttribute(theValue, theConstraint->feature()->real(SketchPlugin_Constraint::VALUE()));

//  theConstraint->execute();
}

//--------------------------------------------------------------------------------------
std::list<ModelHighAPI_Selection> SketchAPI_Sketch::selectFace() const
{
  const_cast<SketchAPI_Sketch*>(this)->execute();

  std::list<ModelHighAPI_Selection> aSelectionList;

  ResultConstructionPtr aResultConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(feature()->firstResult());
  if (aResultConstruction.get() == NULL)
    return aSelectionList;

  for (int anIndex = 0; anIndex < aResultConstruction->facesNum(); ++anIndex) {
    aSelectionList.push_back(
        ModelHighAPI_Selection(aResultConstruction,
                               aResultConstruction->face(anIndex)));
  }

  return aSelectionList;
}

//--------------------------------------------------------------------------------------
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const std::shared_ptr<GeomAPI_Ax3> & thePlane)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, thePlane));
}

SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, theExternal));
}

SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(
    new SketchAPI_Sketch(aFeature, ModelHighAPI_Selection("FACE", theExternalName)));
}

SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    std::shared_ptr<ModelAPI_Object> thePlaneObject)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, thePlaneObject));
}


//--------------------------------------------------------------------------------------
std::list< std::shared_ptr<SketchAPI_Point> > SketchAPI_Sketch::getFreePoints()
{
  std::list< std::shared_ptr<SketchAPI_Point> > aFreePoints;
  std::list<ResultPtr> aPoints = SketcherPrs_Tools::getFreePoints(compositeFeature());
  for (std::list<ResultPtr>::iterator anIt = aPoints.begin(); anIt != aPoints.end(); ++anIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    PointPtr aPoint(new SketchAPI_Point(aFeature));
    aFreePoints.push_back(aPoint);
  }
  return aFreePoints;
}

//--------------------------------------------------------------------------------------
static GeomCurvePtr untrimmedCurve(GeomShapePtr theShape)
{
  GeomCurvePtr aCurve(new GeomAPI_Curve(theShape));
  if (aCurve->isTrimmed())
    aCurve = aCurve->basisCurve();
  return aCurve;
}

void SketchAPI_Sketch::changeFacesOrder(
    const std::list<std::list<ModelHighAPI_Selection> >& theFaces)
{
  // collect faces of the sketch
  ResultConstructionPtr aSketchResult =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(feature()->lastResult());
  if (!aSketchResult) {
    // sketch is nested to a boolean operation, thus, it has no result yet.
    feature()->execute();
    aSketchResult =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(feature()->lastResult());
  }
  std::list<GeomFacePtr> aFaces;
  int aFacesNum = aSketchResult->facesNum();
  for (int i = 0; i < aFacesNum; ++i)
    aFaces.push_back(aSketchResult->face(i));
  // find new faces order according to the given lists of edges
  std::list<GeomFacePtr> aNewFacesOrder;
  std::list<std::list<ModelHighAPI_Selection> >::const_iterator anIt = theFaces.begin();
  for (; anIt != theFaces.end(); ++anIt) {
    // find the appropriate face
    std::list<GeomFacePtr>::iterator aFIt = aFaces.begin();
    for (; aFIt != aFaces.end(); ++aFIt) {
      std::list<ModelHighAPI_Selection>::const_iterator aEdgeIt = anIt->begin();
      GeomAPI_ShapeExplorer aFExp(*aFIt, GeomAPI_Shape::EDGE);
      for (; aEdgeIt != anIt->end() && aFExp.more(); ++aEdgeIt, aFExp.next()) {
        ResultPtr aCurRes = aEdgeIt->resultSubShapePair().first;
        if (!aCurRes)
          break;
        GeomCurvePtr aCurve1 = untrimmedCurve(aCurRes->shape());
        GeomCurvePtr aCurve2 = untrimmedCurve(aFExp.current());
        if (!aCurve1->isEqual(aCurve2))
          break;
      }

      if (aEdgeIt == anIt->end() && !aFExp.more()) {
        // face is found
        aNewFacesOrder.push_back(*aFIt);
        aFaces.erase(aFIt);
        break;
      }
    }
  }
  // place the rest faces at the end of new faces list
  if (!aFaces.empty())
    aNewFacesOrder.insert(aNewFacesOrder.end(), aFaces.begin(), aFaces.end());
  // update the result of the sketch with the new order of faces
  aSketchResult->setFacesOrder(aNewFacesOrder);
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Point> SketchAPI_Sketch::addPoint(
    double theX, double theY)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Point::ID());
  return PointPtr(new SketchAPI_Point(aFeature, theX, theY));
}
std::shared_ptr<SketchAPI_Point> SketchAPI_Sketch::addPoint(
    const std::shared_ptr<GeomAPI_Pnt2d> & thePoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Point::ID());
  return PointPtr(new SketchAPI_Point(aFeature, thePoint));
}
std::shared_ptr<SketchAPI_Point>
  SketchAPI_Sketch::addPoint(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Point::ID());
  return PointPtr(new SketchAPI_Point(aFeature, theExternal));
}
std::shared_ptr<SketchAPI_Point> SketchAPI_Sketch::addPoint(const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Point::ID());
  return PointPtr(new SketchAPI_Point(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_IntersectionPoint> SketchAPI_Sketch::addIntersectionPoint(
    const ModelHighAPI_Selection & theExternal,
    bool theKeepResult)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_IntersectionPoint::ID());
  IntersectionPointPtr anIntersection(new SketchAPI_IntersectionPoint(aFeature, theExternal));
  anIntersection->setIncludeToResult(theKeepResult);
  return anIntersection;
}
std::shared_ptr<SketchAPI_IntersectionPoint> SketchAPI_Sketch::addIntersectionPoint(
    const std::string & theExternalName,
    bool theKeepResult)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_IntersectionPoint::ID());
  IntersectionPointPtr anIntersection(new SketchAPI_IntersectionPoint(aFeature, theExternalName));
  anIntersection->setIncludeToResult(theKeepResult);
  return anIntersection;
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(double theX1, double theY1,
                                                          double theX2, double theY2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theX1, theY1, theX2, theY2));
}
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theStartPoint, theEndPoint));
}
std::shared_ptr<SketchAPI_Line>
  SketchAPI_Sketch::addLine(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theExternal));
}
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Rectangle> SketchAPI_Sketch::addRectangle(double theX1, double theY1,
                                                                    double theX2, double theY2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchAPI_Rectangle::ID());
  return RectanglePtr(new SketchAPI_Rectangle(aFeature, theX1, theY1, theX2, theY2));
}
std::shared_ptr<SketchAPI_Rectangle> SketchAPI_Sketch::addRectangle(
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchAPI_Rectangle::ID());
  return RectanglePtr(new SketchAPI_Rectangle(aFeature, theStartPoint, theEndPoint));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(double theCenterX,
                                                              double theCenterY,
                                                              double theRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theCenterX, theCenterY, theRadius));
}

std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(
                                    const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                    double theRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theCenter, theRadius));
}

std::shared_ptr<SketchAPI_MacroCircle> SketchAPI_Sketch::addCircle(double theCenterX,
                                                                   double theCenterY,
                                                                   double thePassedX,
                                                                   double thePassedY)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MacroCircle::ID());
  return MacroCirclePtr(new SketchAPI_MacroCircle(aFeature, theCenterX, theCenterY,
                                                            thePassedX, thePassedY));
}

std::shared_ptr<SketchAPI_MacroCircle> SketchAPI_Sketch::addCircle(
    const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MacroCircle::ID());
  return MacroCirclePtr(new SketchAPI_MacroCircle(aFeature, theCenterPoint, thePassedPoint));
}

std::shared_ptr<SketchAPI_MacroCircle> SketchAPI_Sketch::addCircle(double theX1, double theY1,
                                                                   double theX2, double theY2,
                                                                   double theX3, double theY3)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MacroCircle::ID());
  return MacroCirclePtr(new SketchAPI_MacroCircle(aFeature, theX1, theY1,
                                                            theX2, theY2,
                                                            theX3, theY3));
}

std::shared_ptr<SketchAPI_MacroCircle> SketchAPI_Sketch::addCircle(
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MacroCircle::ID());
  return MacroCirclePtr(new SketchAPI_MacroCircle(aFeature, thePoint1, thePoint2, thePoint3));
}

std::shared_ptr<SketchAPI_Circle>
  SketchAPI_Sketch::addCircle(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theExternal));
}

std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(double theCenterX, double theCenterY,
                                                        double theStartX, double theStartY,
                                                        double theEndX, double theEndY,
                                                        bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature,
                                  theCenterX, theCenterY,
                                  theStartX, theStartY,
                                  theEndX, theEndY,
                                  theInversed));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(
                                              const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                              const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                              const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                              bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theCenter, theStart, theEnd, theInversed));
}

std::shared_ptr<SketchAPI_MacroArc> SketchAPI_Sketch::addArc(double theStartX, double theStartY,
                                                        double theEndX, double theEndY,
                                                        double thePassedX, double thePassedY)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MacroArc::ID());
  return MacroArcPtr(new SketchAPI_MacroArc(aFeature,
                                       theStartX, theStartY,
                                       theEndX, theEndY,
                                       thePassedX, thePassedY));
}

std::shared_ptr<SketchAPI_MacroArc> SketchAPI_Sketch::addArc(
                                                const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                                const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MacroArc::ID());
  return MacroArcPtr(new SketchAPI_MacroArc(aFeature, theStart, theEnd, thePassed));
}

std::shared_ptr<SketchAPI_MacroArc> SketchAPI_Sketch::addArc(
                                                const ModelHighAPI_RefAttr& theTangentPoint,
                                                double theEndX, double theEndY,
                                                bool theInversed,
                                                bool theTransversal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MacroArc::ID());
  MacroArcPtr aMacroArc(new SketchAPI_MacroArc(aFeature));
  if (theTransversal)
    aMacroArc->setByTransversal(theTangentPoint, theEndX, theEndY, theInversed);
  else
    aMacroArc->setByTangent(theTangentPoint, theEndX, theEndY, theInversed);
  return aMacroArc;
}

std::shared_ptr<SketchAPI_MacroArc> SketchAPI_Sketch::addArc(
                                              const ModelHighAPI_RefAttr& theTangentPoint,
                                              const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                              bool theInversed,
                                              bool theTransversal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MacroArc::ID());
  MacroArcPtr aMacroArc(new SketchAPI_MacroArc(aFeature));
  if (theTransversal)
    aMacroArc->setByTransversal(theTangentPoint, theEnd, theInversed);
  else
    aMacroArc->setByTangent(theTangentPoint, theEnd, theInversed);
  return aMacroArc;
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theExternal));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Ellipse> SketchAPI_Sketch::addEllipse(
    double theCenterX, double theCenterY,
    double theFocusX, double theFocusY,
    double theMinorRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_Ellipse::ID());
  return EllipsePtr(new SketchAPI_Ellipse(aFeature,
      theCenterX, theCenterY, theFocusX, theFocusY, theMinorRadius));
}

std::shared_ptr<SketchAPI_Ellipse> SketchAPI_Sketch::addEllipse(
    const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
    const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
    double theMinorRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_Ellipse::ID());
  return EllipsePtr(new SketchAPI_Ellipse(aFeature, theCenter, theFocus, theMinorRadius));
}

std::shared_ptr<SketchAPI_MacroEllipse> SketchAPI_Sketch::addEllipse(
    double thePoint1X, double thePoint1Y,
    double thePoint2X, double thePoint2Y,
    double thePassedX, double thePassedY,
    bool isPoint1Center)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_MacroEllipse::ID());
  return MacroEllipsePtr(new SketchAPI_MacroEllipse(aFeature,
      thePoint1X, thePoint1Y, thePoint2X, thePoint2Y, thePassedX, thePassedY, isPoint1Center));
}

std::shared_ptr<SketchAPI_MacroEllipse> SketchAPI_Sketch::addEllipse(
    const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr>& thePoint1,
    const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr>& thePoint2,
    const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr>& thePassedPoint,
    bool isPoint1Center)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_MacroEllipse::ID());
  MacroEllipsePtr anEllipse;
  if (thePoint1.second.isEmpty() &&
      thePoint2.second.isEmpty() &&
      thePassedPoint.second.isEmpty()) {
    anEllipse.reset(new SketchAPI_MacroEllipse(aFeature,
        thePoint1.first, thePoint2.first, thePassedPoint.first, isPoint1Center));
  }
  else {
    anEllipse.reset(new SketchAPI_MacroEllipse(aFeature,
        thePoint1.first, thePoint1.second,
        thePoint2.first, thePoint2.second,
        thePassedPoint.first, thePassedPoint.second,
        isPoint1Center));
  }
  return anEllipse;
}

std::shared_ptr<SketchAPI_Ellipse> SketchAPI_Sketch::addEllipse(
    const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_Ellipse::ID());
  return EllipsePtr(new SketchAPI_Ellipse(aFeature, theExternal));
}

std::shared_ptr<SketchAPI_Ellipse> SketchAPI_Sketch::addEllipse(
    const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_Ellipse::ID());
  return EllipsePtr(new SketchAPI_Ellipse(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_EllipticArc> SketchAPI_Sketch::addEllipticArc(
    double theCenterX, double theCenterY,
    double theFocusX, double theFocusY,
    double theStartX, double theStartY,
    double theEndX, double theEndY,
    bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_EllipticArc::ID());
  return EllipticArcPtr(new SketchAPI_EllipticArc(aFeature,
      theCenterX, theCenterY,
      theFocusX, theFocusY,
      theStartX, theStartY,
      theEndX, theEndY,
      theInversed));
}

std::shared_ptr<SketchAPI_EllipticArc> SketchAPI_Sketch::addEllipticArc(
    const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
    const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
    const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
    const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
    bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_EllipticArc::ID());
  return EllipticArcPtr(new SketchAPI_EllipticArc(aFeature,
      theCenter, theFocus, theStart, theEnd, theInversed));
}

std::shared_ptr<SketchAPI_MacroEllipticArc> SketchAPI_Sketch::addEllipticArc(
    const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr>& theCenter,
    const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr>& theMajorAxisPoint,
    const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr>& theStartPoint,
    const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr>& theEndPoint,
    bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_MacroEllipticArc::ID());
  return MacroEllipticArcPtr(new SketchAPI_MacroEllipticArc(aFeature,
      theCenter.first, theCenter.second,
      theMajorAxisPoint.first, theMajorAxisPoint.second,
      theStartPoint.first, theStartPoint.second,
      theEndPoint.first, theEndPoint.second,
      theInversed));
}

std::shared_ptr<SketchAPI_EllipticArc> SketchAPI_Sketch::addEllipticArc(
    const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_EllipticArc::ID());
  return EllipticArcPtr(new SketchAPI_EllipticArc(aFeature, theExternal));
}

std::shared_ptr<SketchAPI_EllipticArc> SketchAPI_Sketch::addEllipticArc(
    const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_EllipticArc::ID());
  return EllipticArcPtr(new SketchAPI_EllipticArc(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Projection> SketchAPI_Sketch::addProjection(
    const ModelHighAPI_Selection & theExternalFeature,
    bool theKeepResult)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Projection::ID());
  ProjectionPtr aProjection(new SketchAPI_Projection(aFeature, theExternalFeature));
  aProjection->setIncludeToResult(theKeepResult);
  return aProjection;
}

std::shared_ptr<SketchAPI_Projection> SketchAPI_Sketch::addProjection(
    const std::string & theExternalName,
    bool theKeepResult)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Projection::ID());
  ProjectionPtr aProjection(new SketchAPI_Projection(aFeature, theExternalName));
  aProjection->setIncludeToResult(theKeepResult);
  return aProjection;
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Mirror> SketchAPI_Sketch::addMirror(
    const ModelHighAPI_RefAttr & theMirrorLine,
    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_ConstraintMirror::ID());
  return MirrorPtr(new SketchAPI_Mirror(aFeature, theMirrorLine, theObjects));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Translation> SketchAPI_Sketch::addTranslation(
    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
    const ModelHighAPI_RefAttr & thePoint1,
    const ModelHighAPI_RefAttr & thePoint2,
    const ModelHighAPI_Integer & theNumberOfObjects,
    bool theFullValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MultiTranslation::ID());
  return TranslationPtr(new SketchAPI_Translation(aFeature, theObjects, thePoint1,
                                                  thePoint2, theNumberOfObjects, theFullValue));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Rotation> SketchAPI_Sketch::addRotation(
    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
    const ModelHighAPI_RefAttr & theCenter,
    const ModelHighAPI_Double & theAngle,
    const ModelHighAPI_Integer & theNumberOfObjects,
    bool theFullValue,
    bool theReversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_MultiRotation::ID());
  return RotationPtr(
    new SketchAPI_Rotation(aFeature, theObjects, theCenter,
                           theAngle, theNumberOfObjects, theFullValue, theReversed));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::addSplit(
                                          const ModelHighAPI_Reference& theFeature,
                                          const std::shared_ptr<GeomAPI_Pnt2d>& thePositionPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Split::ID());
  fillAttribute(theFeature, aFeature->reference(SketchPlugin_Split::SELECTED_OBJECT()));

  AttributePtr anAttribute = aFeature->attribute(SketchPlugin_Split::SELECTED_POINT());
  if (anAttribute->attributeType() == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttribute);
    fillAttribute(thePositionPoint, aPointAttr);
  }

  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::addTrim(
                                        const ModelHighAPI_Reference& theFeature,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& thePositionPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    compositeFeature()->addFeature(SketchPlugin_Trim::ID());
  fillAttribute(theFeature, aFeature->reference(SketchPlugin_Trim::SELECTED_OBJECT()));

  AttributePtr anAttribute = aFeature->attribute(SketchPlugin_Trim::SELECTED_POINT());
  if (anAttribute->attributeType() == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttribute);
    fillAttribute(thePositionPoint, aPointAttr);
  }

  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setAngle(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintAngle::ID());
  fillAttribute(SketcherPrs_Tools::ANGLE_DIRECT,
      aFeature->integer(SketchPlugin_ConstraintAngle::TYPE_ID()));
  // fill the value before llines to avoid calculation of angle value by the Angle feature
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setAngleComplementary(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintAngle::ID());
  fillAttribute(SketcherPrs_Tools::ANGLE_COMPLEMENTARY,
      aFeature->integer(SketchPlugin_ConstraintAngle::TYPE_ID()));
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setAngleBackward(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintAngle::ID());
  fillAttribute(SketcherPrs_Tools::ANGLE_BACKWARD,
      aFeature->integer(SketchPlugin_ConstraintAngle::TYPE_ID()));
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setCoincident(
    const ModelHighAPI_RefAttr & thePoint1,
    const ModelHighAPI_RefAttr & thePoint2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  fillAttribute(thePoint1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(thePoint2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setCollinear(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintCollinear::ID());
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setDistance(
    const ModelHighAPI_RefAttr & thePoint,
    const ModelHighAPI_RefAttr & thePointOrLine,
    const ModelHighAPI_Double & theValue,
    bool isSigned)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintDistance::ID());
  fillAttribute(thePoint, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(thePointOrLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  fillAttribute(isSigned, aFeature->boolean(SketchPlugin_ConstraintDistance::SIGNED()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setSignedDistance(
    const ModelHighAPI_RefAttr & thePoint,
    const ModelHighAPI_RefAttr & thePointOrLine,
    const ModelHighAPI_Double & theValue)
{
  return setDistance(thePoint, thePointOrLine, theValue, true);
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setUnsignedDistance(
    const ModelHighAPI_RefAttr & thePoint,
    const ModelHighAPI_RefAttr & thePointOrLine,
    const ModelHighAPI_Double & theValue)
{
  return setDistance(thePoint, thePointOrLine, theValue, false);
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setHorizontalDistance(
    const ModelHighAPI_RefAttr & thePoint1,
    const ModelHighAPI_RefAttr & thePoint2,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintDistanceHorizontal::ID());
  fillAttribute(thePoint1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(thePoint2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  fillAttribute(theValue,
      aFeature->real(SketchPlugin_ConstraintDistanceAlongDir::DISTANCE_VALUE_ID()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setVerticalDistance(
    const ModelHighAPI_RefAttr & thePoint1,
    const ModelHighAPI_RefAttr & thePoint2,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintDistanceVertical::ID());
  fillAttribute(thePoint1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(thePoint2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  fillAttribute(theValue,
      aFeature->real(SketchPlugin_ConstraintDistanceAlongDir::DISTANCE_VALUE_ID()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setEqual(
    const ModelHighAPI_RefAttr & theObject1,
    const ModelHighAPI_RefAttr & theObject2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintEqual::ID());
  fillAttribute(theObject1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theObject2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setFillet(
    const ModelHighAPI_RefAttr & thePoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_Fillet::ID());
  fillAttribute(thePoint, aFeature->data()->refattr(SketchPlugin_Fillet::FILLET_POINT_ID()));
  apply(); // finish operation to remove Fillet feature correcly
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setFilletWithRadius(
    const ModelHighAPI_RefAttr & thePoint,
    const ModelHighAPI_Double & theRadius)
{
  CompositeFeaturePtr aSketch = compositeFeature();
  int aNbSubs = aSketch->numberOfSubs();

  // create fillet
  InterfacePtr aFilletFeature = setFillet(thePoint);

  // set radius for just created arc
  FeaturePtr anArc = aSketch->subFeature(aNbSubs - 1);
  if (anArc->getKind() == SketchPlugin_Arc::ID())
    setRadius(ModelHighAPI_RefAttr(ObjectPtr(anArc->lastResult())), ModelHighAPI_Double(theRadius));

  return aFilletFeature;
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setFixed(
    const ModelHighAPI_RefAttr & theObject)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintRigid::ID());
  fillAttribute(theObject, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setHorizontal(
    const ModelHighAPI_RefAttr & theLine)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintHorizontal::ID());
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setLength(
    const ModelHighAPI_RefAttr & theLine,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintLength::ID());
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setMiddlePoint(
    const ModelHighAPI_RefAttr & thePoint,
    const ModelHighAPI_RefAttr & theLine)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintMiddle::ID());
  fillAttribute(thePoint, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setParallel(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintParallel::ID());
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setPerpendicular(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintPerpendicular::ID());
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setRadius(
    const ModelHighAPI_RefAttr & theCircleOrArc,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintRadius::ID());
  fillAttribute(theCircleOrArc, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setTangent(
    const ModelHighAPI_RefAttr & theLine,
    const ModelHighAPI_RefAttr & theCircle)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintTangent::ID());
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theCircle, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Sketch::setVertical(
    const ModelHighAPI_RefAttr & theLine)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintVertical::ID());
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  aFeature->execute();
  return InterfacePtr(new ModelHighAPI_Interface(aFeature));
}

//--------------------------------------------------------------------------------------

static std::shared_ptr<GeomAPI_Pnt2d> pointCoordinates(const AttributePtr& thePoint)
{
  AttributePoint2DPtr aPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(thePoint);
  return aPnt ? aPnt->pnt() : std::shared_ptr<GeomAPI_Pnt2d>();
}

static std::shared_ptr<GeomAPI_Pnt2d> middlePointOnLine(const FeaturePtr& theFeature)
{
  AttributePoint2DPtr aStartAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::START_ID()));
  AttributePoint2DPtr aEndAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::END_ID()));

  if (!aStartAttr || !aEndAttr)
    return std::shared_ptr<GeomAPI_Pnt2d>();

  std::shared_ptr<GeomAPI_XY> aStartPoint = aStartAttr->pnt()->xy();
  std::shared_ptr<GeomAPI_XY> aEndPoint = aEndAttr->pnt()->xy();
  return std::shared_ptr<GeomAPI_Pnt2d>(
      new GeomAPI_Pnt2d(aStartPoint->added(aEndPoint)->multiplied(0.5)));
}

static std::shared_ptr<GeomAPI_Pnt2d> pointOnCircle(const FeaturePtr& theFeature)
{
  AttributePoint2DPtr aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Circle::CENTER_ID()));
  AttributeDoublePtr aRadius = theFeature->real(SketchPlugin_Circle::RADIUS_ID());

  if (!aCenter || !aRadius)
    return std::shared_ptr<GeomAPI_Pnt2d>();

  return std::shared_ptr<GeomAPI_Pnt2d>(
      new GeomAPI_Pnt2d(aCenter->x() + aRadius->value(), aCenter->y()));
}

static std::shared_ptr<GeomAPI_Pnt2d> middlePointOnArc(const FeaturePtr& theFeature)
{
  static const double PI = 3.141592653589793238463;

  AttributePoint2DPtr aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::CENTER_ID()));
  AttributePoint2DPtr aStartAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::START_ID()));
  AttributePoint2DPtr aEndAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::END_ID()));

  if (!aCenterAttr || !aStartAttr || !aEndAttr)
    return std::shared_ptr<GeomAPI_Pnt2d>();

  std::shared_ptr<GeomAPI_Dir2d> aStartDir(new GeomAPI_Dir2d(
      aStartAttr->x() - aCenterAttr->x(), aStartAttr->y() - aCenterAttr->y()));
  std::shared_ptr<GeomAPI_Dir2d> aEndDir(new GeomAPI_Dir2d(
      aEndAttr->x() - aCenterAttr->x(), aEndAttr->y() - aCenterAttr->y()));

  double anAngle = aStartDir->angle(aEndDir);
  bool isReversed = theFeature->boolean(SketchPlugin_Arc::REVERSED_ID())->value();
  if (isReversed && anAngle > 0.)
    anAngle -= 2.0 * PI;
  else if (!isReversed && anAngle <= 0.)
    anAngle += 2.0 * PI;

  double cosA = cos(anAngle);
  double sinA = sin(anAngle);

  // rotate start dir to find middle point on arc
  double aRadius = aStartAttr->pnt()->distance(aCenterAttr->pnt());
  double x = aCenterAttr->x() + aRadius * (aStartDir->x() * cosA - aStartDir->y() * sinA);
  double y = aCenterAttr->y() + aRadius * (aStartDir->x() * sinA + aStartDir->y() * cosA);

  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(x, y));
}

static std::shared_ptr<GeomAPI_Pnt2d> pointOnEllipse(const FeaturePtr& theFeature)
{
  AttributePoint2DPtr aMajorAxisEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_END_ID()));
  return aMajorAxisEnd ? aMajorAxisEnd->pnt() : std::shared_ptr<GeomAPI_Pnt2d>();
}

static std::shared_ptr<GeomAPI_Pnt2d> middlePoint(const ObjectPtr& theObject)
{
  std::shared_ptr<GeomAPI_Pnt2d> aMiddlePoint;
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature) {
    // move only features of the following types
    const std::string& aFeatureKind = aFeature->getKind();
    if (aFeatureKind == SketchPlugin_Point::ID())
      aMiddlePoint = pointCoordinates(aFeature->attribute(SketchPlugin_Point::COORD_ID()));
    else if (aFeatureKind == SketchPlugin_Line::ID())
      aMiddlePoint = middlePointOnLine(aFeature);
    else if (aFeatureKind == SketchPlugin_Circle::ID())
      aMiddlePoint = pointOnCircle(aFeature);
    else if (aFeatureKind == SketchPlugin_Arc::ID())
      aMiddlePoint = middlePointOnArc(aFeature);
    else if (aFeatureKind == SketchPlugin_Ellipse::ID())
      aMiddlePoint = pointOnEllipse(aFeature);
  }
  return aMiddlePoint;
}

void SketchAPI_Sketch::move(const ModelHighAPI_RefAttr& theMovedEntity,
                            const std::shared_ptr<GeomAPI_Pnt2d>& theTargetPoint)
{
  std::shared_ptr<ModelAPI_ObjectMovedMessage> aMessage(new ModelAPI_ObjectMovedMessage);
  theMovedEntity.fillMessage(aMessage);

  std::shared_ptr<GeomAPI_Pnt2d> anOriginalPosition;
  if (aMessage->movedAttribute())
    anOriginalPosition = pointCoordinates(aMessage->movedAttribute());
  else
    anOriginalPosition = middlePoint(aMessage->movedObject());

  if (!anOriginalPosition)
    return; // something has gone wrong, do not process movement

  aMessage->setOriginalPosition(anOriginalPosition);
  aMessage->setCurrentPosition(theTargetPoint);
  Events_Loop::loop()->send(aMessage);
}

void SketchAPI_Sketch::move(const ModelHighAPI_RefAttr& theMovedEntity,
                            double theTargetX, double theTargetY)
{
  std::shared_ptr<GeomAPI_Pnt2d> aTargetPoint(new GeomAPI_Pnt2d(theTargetX, theTargetY));
  move(theMovedEntity, aTargetPoint);
}

//--------------------------------------------------------------------------------------

std::shared_ptr<GeomAPI_Pnt2d> SketchAPI_Sketch::to2D(const std::shared_ptr<GeomAPI_Pnt>& thePoint)
{
  FeaturePtr aBase = feature();
  std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aBase->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aBase->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aBase->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));

  return thePoint->to2D(aC->pnt(), aX->dir(), aY);
}

//--------------------------------------------------------------------------------------

static bool isDifferent(GeomFacePtr theFace1, GeomFacePtr theFace2)
{
  // collect edges of the first face
  std::list<GeomShapePtr> anEdges1;
  for (GeomAPI_ShapeExplorer anExp(theFace1, GeomAPI_Shape::EDGE); anExp.more(); anExp.next())
    anEdges1.push_back(anExp.current());
  // compare edges of faces
  for (GeomAPI_ShapeExplorer anExp(theFace2, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
    GeomShapePtr aCurrent = anExp.current();
    bool isFound = false;
    std::list<GeomShapePtr>::iterator anIt1 = anEdges1.begin();
    for (; anIt1 != anEdges1.end(); ++anIt1)
      if (aCurrent->isSameGeometry(*anIt1)) {
        isFound = true;
        anEdges1.erase(anIt1);
        break;
      }
    if (!isFound)
      return true;
  }
  return !anEdges1.empty();
}

static bool isCustomFacesOrder(CompositeFeaturePtr theSketch)
{
  ResultConstructionPtr aSketchResult =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theSketch->lastResult());
  if (!aSketchResult)
    return false;

  std::shared_ptr<GeomAPI_PlanarEdges> aWires =
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aSketchResult->shape());
  if (!aWires)
    return false;

  // collect faces constructed by SketchBuilder algorithm
  GeomAlgoAPI_SketchBuilder aSketchBuilder(aWires->origin(), aWires->dirX(),
                                           aWires->norm(), aWires);
  const ListOfShape& aFaces = aSketchBuilder.faces();

  // compare faces stored in sketch with faces generated by SketchBuilder
  int aNbSketchFaces = aSketchResult->facesNum();
  int aFaceIndex = 0;
  for (ListOfShape::const_iterator aFIt = aFaces.begin();
       aFIt != aFaces.end() && aFaceIndex < aNbSketchFaces;
       ++aFIt, ++aFaceIndex) {
    GeomFacePtr aSketchFace = aSketchResult->face(aFaceIndex);
    GeomFacePtr aCurFace = (*aFIt)->face();
    if (isDifferent(aSketchFace, aCurFace))
      return true;
  }
  return false;
}

static void edgesOfSketchFaces(CompositeFeaturePtr theSketch,
                               std::list<std::list<ResultPtr> >& theEdges)
{
  ResultConstructionPtr aSketchResult =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theSketch->lastResult());
  if (!aSketchResult)
    return;

  // collect curves of the sketch
  std::map<GeomCurvePtr, ResultPtr, GeomAPI_Curve::Comparator> aCurves;
  int aSubNum = theSketch->numberOfSubs();
  for (int a = 0; a < aSubNum; ++a) {
    FeaturePtr aSub = theSketch->subFeature(a);
    const std::list<ResultPtr>& aResults = aSub->results();
    std::list<ResultPtr>::const_iterator aRes = aResults.cbegin();
    for (; aRes != aResults.cend(); aRes++) {
      GeomShapePtr aCurShape = (*aRes)->shape();
      if (aCurShape && aCurShape->isEdge())
        aCurves[untrimmedCurve(aCurShape)] = *aRes;
    }
  }

  // convert each face to the list of results of its edges
  int aFacesNum = aSketchResult->facesNum();
  for (int a = 0; a < aFacesNum; ++a) {
    theEdges.push_back(std::list<ResultPtr>());
    std::list<ResultPtr>& aCurEdges = theEdges.back();

    GeomFacePtr aFace = aSketchResult->face(a);
    for (GeomAPI_ShapeExplorer anExp(aFace, GeomAPI_Shape::EDGE);
         anExp.more(); anExp.next()) {
      GeomCurvePtr aCurrent = untrimmedCurve(anExp.current());
      aCurEdges.push_back(aCurves[aCurrent]);
    }
  }
}

//--------------------------------------------------------------------------------------

void SketchAPI_Sketch::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->value()) {
    theDumper << aBase << " = model.addSketch(" << aDocName <<
      ", " << anExternal << ")" << std::endl;
  } else {
    // Sketch is base on a plane.
    std::shared_ptr<GeomAPI_Pnt> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aBase->attribute(SketchPlugin_Sketch::ORIGIN_ID()))->pnt();
    std::shared_ptr<GeomAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aBase->attribute(SketchPlugin_Sketch::NORM_ID()))->dir();
    std::shared_ptr<GeomAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aBase->attribute(SketchPlugin_Sketch::DIRX_ID()))->dir();

    // Check the plane is coordinate plane
    std::string aPlaneName = defaultPlane(anOrigin, aNormal, aDirX);
    if(anExternal->context()) { // checking for selected planes
      if (!aPlaneName.empty()
          && anExternal->context()->data()
          && anExternal->context()->data()->name() == aPlaneName) {
        // dump sketch based on coordinate plane
        theDumper << aBase << " = model.addSketch(" << aDocName
                  << ", model.standardPlane(\"" << aPlaneName << "\"))" << std::endl;
      } else { // some other plane
        theDumper << aBase << " = model.addSketch(" << aDocName <<
          ", " << anExternal<< ")" << std::endl;
      }
    } else {
      if (aPlaneName.empty()) {
        // needs import additional module
        theDumper.importModule("GeomAPI");
        // dump plane parameters
        const std::string& aSketchName = theDumper.name(aBase);
        std::string anOriginName = aSketchName + "_origin";
        std::string aNormalName  = aSketchName + "_norm";
        std::string aDirXName    = aSketchName + "_dirx";
        // use "\n" instead of std::endl to avoid automatic dumping sketch here
        // and then dumplicate dumping it in the next line
        theDumper << anOriginName << " = " << anOrigin << "\n"
                  << aNormalName  << " = " << aNormal  << "\n"
                  << aDirXName    << " = " << aDirX    << "\n";
        // dump sketch based on arbitrary plane
        theDumper << aBase << " = model.addSketch(" << aDocName << ", GeomAPI_Ax3("
                  << anOriginName << ", " << aDirXName << ", " << aNormalName << "))" << std::endl;
      } else {
        // dump sketch based on coordinate plane
        theDumper << aBase << " = model.addSketch(" << aDocName
                  << ", model.defaultPlane(\"" << aPlaneName << "\"))" << std::endl;
      }
    }
  }

  // dump sketch's subfeatures
  CompositeFeaturePtr aCompFeat = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aBase);
  theDumper.processSubs(aCompFeat, true);

  // if face order differs to the order generated by SketchBuilder,
  // dump the list of faces for correct execution of the script
  if (isCustomFacesOrder(aCompFeat)) {
    std::list<std::list<ResultPtr> > aFaces;
    edgesOfSketchFaces(aCompFeat, aFaces);

    const std::string& aSketchName = theDumper.name(aBase);
    std::string aMethodName(".changeFacesOrder");
    std::string aSpaceShift(aSketchName.size() + aMethodName.size(), ' ');

    theDumper << aSketchName << aMethodName << "([";
    for (std::list<std::list<ResultPtr> >::iterator aFIt = aFaces.begin();
         aFIt != aFaces.end(); ++aFIt) {
      if (aFIt != aFaces.begin())
        theDumper << ",\n" << aSpaceShift << "  ";
      theDumper << *aFIt;
    }
    theDumper << "\n" << aSpaceShift << " ])" << std::endl;
  }
}
