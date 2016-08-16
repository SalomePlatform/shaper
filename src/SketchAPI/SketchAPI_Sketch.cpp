// Name   : SketchAPI_Sketch.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Sketch.h"
//--------------------------------------------------------------------------------------
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintFillet.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketcherPrs_Tools.h>
//--------------------------------------------------------------------------------------
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_RefAttr.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
#include "SketchAPI_Arc.h"
#include "SketchAPI_Circle.h"
#include "SketchAPI_IntersectionPoint.h"
#include "SketchAPI_Line.h"
#include "SketchAPI_Mirror.h"
#include "SketchAPI_Point.h"
#include "SketchAPI_Projection.h"
#include "SketchAPI_Rectangle.h"
#include "SketchAPI_Rotation.h"
#include "SketchAPI_Translation.h"
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
    const std::shared_ptr<ModelAPI_Feature> & theConstraint,
    const ModelHighAPI_Double & theValue)
{
  // TODO(spo): check somehow that the feature is a constraint or eliminate crash if the feature have no real attribute VALUE
  fillAttribute(theValue, theConstraint->real(SketchPlugin_Constraint::VALUE()));

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
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, thePlane));
}

SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const ModelHighAPI_Selection & theExternal)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, theExternal));
}

SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const std::string & theExternalName)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, ModelHighAPI_Selection("FACE", theExternalName)));
}

SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    std::shared_ptr<ModelAPI_Object> thePlaneObject)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, thePlaneObject));
}


//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Point> SketchAPI_Sketch::addPoint(
    double theX, double theY)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Point::ID());
  return PointPtr(new SketchAPI_Point(aFeature, theX, theY));
}
std::shared_ptr<SketchAPI_Point> SketchAPI_Sketch::addPoint(
    const std::shared_ptr<GeomAPI_Pnt2d> & thePoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Point::ID());
  return PointPtr(new SketchAPI_Point(aFeature, thePoint));
}
std::shared_ptr<SketchAPI_Point> SketchAPI_Sketch::addPoint(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Point::ID());
  return PointPtr(new SketchAPI_Point(aFeature, theExternal));
}
std::shared_ptr<SketchAPI_Point> SketchAPI_Sketch::addPoint(const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Point::ID());
  return PointPtr(new SketchAPI_Point(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_IntersectionPoint> SketchAPI_Sketch::addIntersectionPoint(
    const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_IntersectionPoint::ID());
  return IntersectionPointPtr(new SketchAPI_IntersectionPoint(aFeature, theExternal));
}
std::shared_ptr<SketchAPI_IntersectionPoint> SketchAPI_Sketch::addIntersectionPoint(
    const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_IntersectionPoint::ID());
  return IntersectionPointPtr(new SketchAPI_IntersectionPoint(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(double theX1, double theY1, double theX2, double theY2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theX1, theY1, theX2, theY2));
}
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theStartPoint, theEndPoint));
}
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Line::ID());
  LinePtr aLine(new SketchAPI_Line(aFeature, theExternal));
  setFixed(InterfacePtr(aLine));
  return aLine;
}
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Line::ID());
  LinePtr aLine(new SketchAPI_Line(aFeature, theExternalName));
  setFixed(InterfacePtr(aLine));
  return aLine;
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Rectangle> SketchAPI_Sketch::addRectangle(double theX1, double theY1, double theX2, double theY2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchAPI_Rectangle::ID());
  return RectanglePtr(new SketchAPI_Rectangle(aFeature, theX1, theY1, theX2, theY2));
}
std::shared_ptr<SketchAPI_Rectangle> SketchAPI_Sketch::addRectangle(
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchAPI_Rectangle::ID());
  return RectanglePtr(new SketchAPI_Rectangle(aFeature, theStartPoint, theEndPoint));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(double theCenterX,
                                                              double theCenterY,
                                                              double theRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theCenterX, theCenterY, theRadius));
}

std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                                              double theRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theCenter, theRadius));
}

std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(double theX1, double theY1,
                                                              double theX2, double theY2,
                                                              double theX3, double theY3)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theX1, theY1, theX2, theY2, theX3, theY3));
}

std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                                              const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                                              const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, thePoint1, thePoint2, thePoint3));
}

std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theExternal));
}

std::shared_ptr<SketchAPI_Circle> SketchAPI_Sketch::addCircle(const std::string & theExternalName)
{
  // TODO(spo): Add constraint SketchConstraintRigid like in PythonAPI. Is it necessary?
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Circle::ID());
  return CirclePtr(new SketchAPI_Circle(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(double theCenterX, double theCenterY,
                                                        double theStartX, double theStartY,
                                                        double theEndX, double theEndY,
                                                        bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature,
                                  theCenterX, theCenterY,
                                  theStartX, theStartY,
                                  theEndX, theEndY,
                                  theInversed));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                                        const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                                        bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theCenter, theStart, theEnd, theInversed));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(double theStartX, double theStartY,
                                                        double theEndX, double theEndY,
                                                        double thePassedX, double thePassedY)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature,
                                  theStartX, theStartY,
                                  theEndX, theEndY,
                                  thePassedX, thePassedY));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                                        const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theStart, theEnd, thePassed));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(const ModelHighAPI_RefAttr& theTangentPoint,
                                                        double theEndX, double theEndY,
                                                        bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theTangentPoint, theEndX, theEndY, theInversed));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(const ModelHighAPI_RefAttr& theTangentPoint,
                                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                                        bool theInversed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theTangentPoint, theEnd, theInversed));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theExternal));
}

std::shared_ptr<SketchAPI_Arc> SketchAPI_Sketch::addArc(const std::string & theExternalName)
{
  // TODO(spo): Add constraint SketchConstraintRigid like in PythonAPI. Is it necessary?
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Arc::ID());
  return ArcPtr(new SketchAPI_Arc(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Projection> SketchAPI_Sketch::addProjection(
    const ModelHighAPI_Selection & theExternalFeature)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Projection::ID());
  return ProjectionPtr(new SketchAPI_Projection(aFeature, theExternalFeature));
}

std::shared_ptr<SketchAPI_Projection> SketchAPI_Sketch::addProjection(
    const std::string & theExternalName)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Projection::ID());
  return ProjectionPtr(new SketchAPI_Projection(aFeature, theExternalName));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Mirror> SketchAPI_Sketch::addMirror(
    const ModelHighAPI_RefAttr & theMirrorLine,
    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_ConstraintMirror::ID());
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
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_MultiTranslation::ID());
  return TranslationPtr(new SketchAPI_Translation(aFeature, theObjects, thePoint1, thePoint2, theNumberOfObjects, theFullValue));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Rotation> SketchAPI_Sketch::addRotation(
    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
    const ModelHighAPI_RefAttr & theCenter,
    const ModelHighAPI_Double & theAngle,
    const ModelHighAPI_Integer & theNumberOfObjects,
    bool theFullValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_MultiRotation::ID());
  return RotationPtr(new SketchAPI_Rotation(aFeature, theObjects, theCenter, theAngle, theNumberOfObjects, theFullValue));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setAngle(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintAngle::ID());
  fillAttribute(SketcherPrs_Tools::ANGLE_DIRECT,
      aFeature->integer(SketchPlugin_ConstraintAngle::TYPE_ID()));
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setAngleComplementary(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintAngle::ID());
  fillAttribute(SketcherPrs_Tools::ANGLE_COMPLEMENTARY,
      aFeature->integer(SketchPlugin_ConstraintAngle::TYPE_ID()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
//  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setAngleBackward(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintAngle::ID());
  fillAttribute(SketcherPrs_Tools::ANGLE_BACKWARD,
      aFeature->integer(SketchPlugin_ConstraintAngle::TYPE_ID()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
//  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setCoincident(
    const ModelHighAPI_RefAttr & thePoint1,
    const ModelHighAPI_RefAttr & thePoint2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  fillAttribute(thePoint1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(thePoint2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setCollinear(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintCollinear::ID());
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setDistance(
    const ModelHighAPI_RefAttr & thePoint,
    const ModelHighAPI_RefAttr & thePointOrLine,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintDistance::ID());
  fillAttribute(thePoint, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(thePointOrLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setEqual(
    const ModelHighAPI_RefAttr & theObject1,
    const ModelHighAPI_RefAttr & theObject2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintEqual::ID());
  fillAttribute(theObject1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theObject2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setFillet(
    const std::list<ModelHighAPI_RefAttr> & thePoints,
    const ModelHighAPI_Double & theRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintFillet::ID());
  fillAttribute(thePoints, aFeature->data()->refattrlist(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theRadius, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setFixed(
    const ModelHighAPI_RefAttr & theObject)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintRigid::ID());
  fillAttribute(theObject, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setHorizontal(
    const ModelHighAPI_RefAttr & theLine)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintHorizontal::ID());
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setLength(
    const ModelHighAPI_RefAttr & theLine,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintLength::ID());
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setMiddlePoint(
    const ModelHighAPI_RefAttr & thePoint,
    const ModelHighAPI_RefAttr & theLine)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintMiddle::ID());
  fillAttribute(thePoint, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setParallel(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintParallel::ID());
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setPerpendicular(
    const ModelHighAPI_RefAttr & theLine1,
    const ModelHighAPI_RefAttr & theLine2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintPerpendicular::ID());
  fillAttribute(theLine1, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theLine2, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setRadius(
    const ModelHighAPI_RefAttr & theCircleOrArc,
    const ModelHighAPI_Double & theValue)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintRadius::ID());
  fillAttribute(theCircleOrArc, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theValue, aFeature->real(SketchPlugin_Constraint::VALUE()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setTangent(
    const ModelHighAPI_RefAttr & theLine,
    const ModelHighAPI_RefAttr & theCircle)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintTangent::ID());
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  fillAttribute(theCircle, aFeature->refattr(SketchPlugin_Constraint::ENTITY_B()));
  aFeature->execute();
  return aFeature;
}

std::shared_ptr<ModelAPI_Feature> SketchAPI_Sketch::setVertical(
    const ModelHighAPI_RefAttr & theLine)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      compositeFeature()->addFeature(SketchPlugin_ConstraintVertical::ID());
  fillAttribute(theLine, aFeature->refattr(SketchPlugin_Constraint::ENTITY_A()));
  aFeature->execute();
  return aFeature;
}

//--------------------------------------------------------------------------------------

void SketchAPI_Sketch::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->value()) {
    theDumper << aBase << " = model.addSketch(" << aDocName << ", " << anExternal << ")" << std::endl;
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
    if (anExternal->context()) { // checking for selected planes
      if (!aPlaneName.empty()) {
        // dump sketch based on coordinate plane
        theDumper << aBase << " = model.addSketch(" << aDocName
                  << ", model.standardPlane(\"" << aPlaneName << "\"))" << std::endl;
      } else { // some other plane
        theDumper << aBase << " = model.addSketch(" << aDocName << ", " << anExternal<< ")" << std::endl;
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
}
