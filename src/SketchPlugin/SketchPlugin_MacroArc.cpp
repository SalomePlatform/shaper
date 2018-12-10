// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "SketchPlugin_MacroArc.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_ConstraintTangent.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Tools.h"
#include "SketchPlugin_MacroArcReentrantMessage.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Curve.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XY.h>
#include <GeomAPI_ShapeIterator.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>

#include <GeomAlgoAPI_Circ2dBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

// for sqrt on Linux
#include <math.h>

const double tolerance = 1e-7;
const double paramTolerance = 1.e-4;
const double PI = 3.141592653589793238463;

static void projectPointOnCircle(AttributePoint2DPtr& thePoint, const GeomAPI_Circ2d& theCircle)
{
  std::shared_ptr<GeomAPI_Pnt2d> aProjection = theCircle.project(thePoint->pnt());
  if(aProjection.get())
    thePoint->setValue(aProjection);
}

static void intersectShapeAndCircle(const GeomShapePtr& theShape,
                                    const GeomAPI_Circ2d& theCircle,
                                    const SketchPlugin_Sketch* theSketch,
                                    AttributePoint2DPtr& theIntersection)
{
  if (!theShape->isEdge())
    return projectPointOnCircle(theIntersection, theCircle);

  // convert shape to unbounded
  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(theShape));
  if (anEdge->isLine()) {
    static const double HALF_SIZE = 1.e6;
    std::shared_ptr<GeomAPI_XYZ> aLoc = anEdge->line()->location()->xyz();
    std::shared_ptr<GeomAPI_XYZ> aDir = anEdge->line()->direction()->xyz();

    std::shared_ptr<GeomAPI_Pnt> aStart(
        new GeomAPI_Pnt(aLoc->added(aDir->multiplied(-HALF_SIZE))));
    std::shared_ptr<GeomAPI_Pnt> aEnd(
        new GeomAPI_Pnt(aLoc->added(aDir->multiplied(HALF_SIZE))));
    anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, aEnd);
  } else if (anEdge->isArc()) {
    std::shared_ptr<GeomAPI_Circ> aCircle = anEdge->circle();
    anEdge = GeomAlgoAPI_EdgeBuilder::lineCircle(
        aCircle->center(), aCircle->normal(), aCircle->radius());
  }

  // convert 2D circle to 3D object
  std::shared_ptr<GeomAPI_Pnt2d> aCenter2d = theCircle.center();
  std::shared_ptr<GeomAPI_Pnt> aCenter(theSketch->to3D(aCenter2d->x(), aCenter2d->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      const_cast<SketchPlugin_Sketch*>(theSketch)->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

  GeomShapePtr aCircleShape =
      GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, theCircle.radius());

  GeomShapePtr anInter = anEdge->intersect(aCircleShape);
  std::shared_ptr<GeomAPI_Pnt2d> anInterPnt;
  if (!anInter)
    return projectPointOnCircle(theIntersection, theCircle);
  if (anInter->isVertex()) {
    std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(anInter));
    anInterPnt = theSketch->to2D(aVertex->point());
  } else if (anInter->isCompound()) {
    double aMinDist = 1.e300;

    GeomAPI_ShapeIterator anIt(anInter);
    for (; anIt.more(); anIt.next()) {
      GeomShapePtr aCurrent = anIt.current();
      if (!aCurrent->isVertex())
        continue;
      std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aCurrent));
      std::shared_ptr<GeomAPI_Pnt2d> aPnt = theSketch->to2D(aVertex->point());
      double aDist = aPnt->distance(theIntersection->pnt());
      if (aDist < aMinDist) {
        aMinDist = aDist;
        anInterPnt = aPnt;
      }
    }
  }
  if(anInterPnt.get()) {
    theIntersection->setValue(anInterPnt);
  }
}


SketchPlugin_MacroArc::SketchPlugin_MacroArc()
: SketchPlugin_SketchEntity(),
  myParamBefore(0.0)
{
}

void SketchPlugin_MacroArc::initAttributes()
{
  data()->addAttribute(ARC_TYPE(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(CENTER_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(START_POINT_1_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_POINT_1_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(START_POINT_2_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_POINT_2_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(PASSED_POINT_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(TANGENT_POINT_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(END_POINT_3_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(CENTER_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(START_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(END_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(PASSED_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(EDIT_ARC_TYPE_ID(), ModelAPI_AttributeString::typeId());

  boolean(REVERSED_ID())->setValue(false);
  string(EDIT_ARC_TYPE_ID())->setValue("");

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CENTER_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), START_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), END_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PASSED_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EDIT_ARC_TYPE_ID());
}

void SketchPlugin_MacroArc::attributeChanged(const std::string& theID)
{
  std::string anArcType = string(ARC_TYPE())->value();

  // If arc type switched reset according attributes.
  if(theID == ARC_TYPE()) {
    SketchPlugin_Tools::resetAttribute(this, CENTER_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, CENTER_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, START_POINT_1_ID());
    SketchPlugin_Tools::resetAttribute(this, START_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, END_POINT_1_ID());
    SketchPlugin_Tools::resetAttribute(this, END_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, START_POINT_2_ID());
    SketchPlugin_Tools::resetAttribute(this, END_POINT_2_ID());
    SketchPlugin_Tools::resetAttribute(this, PASSED_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, PASSED_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, TANGENT_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, END_POINT_3_ID());
    SketchPlugin_Tools::resetAttribute(this, REVERSED_ID());
    SketchPlugin_Tools::resetAttribute(this, RADIUS_ID());
    SketchPlugin_Tools::resetAttribute(this, ANGLE_ID());

    myCenter.reset();
    myStart.reset();
    myEnd.reset();
    boolean(REVERSED_ID())->setValue(false);
    myParamBefore = 0.0;
  } else if(anArcType == ARC_TYPE_BY_CENTER_AND_POINTS())
    fillByCenterAndTwoPassed();
  else if(anArcType == ARC_TYPE_BY_THREE_POINTS())
    fillByThreePassedPoints();
  else if(anArcType == ARC_TYPE_BY_TANGENT_EDGE())
    fillByTangentEdge();

  double aRadius = 0;
  double anAngle = 0;
  if(myCenter.get() && myStart.get()) {
    aRadius = myCenter->distance(myStart);
    if(myEnd.get()) {
      if(myStart->isEqual(myEnd)) {
        anAngle = 360;
      } else {
        GeomAPI_Circ2d aCircleForArc(myCenter, myStart);
        double aStartParam, anEndParam;
        aCircleForArc.parameter(myStart, paramTolerance, aStartParam);
        aCircleForArc.parameter(myEnd, paramTolerance, anEndParam);
        anAngle = (anEndParam - aStartParam) / PI * 180.0;
        if(boolean(REVERSED_ID())->value()) anAngle = 360.0 - anAngle;
      }
    }
  }

  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  if(myCenter.get()) {
    // center attribute is used in processEvent() to set reference to reentrant arc
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_POINT_ID()))
        ->setValue(myCenter);
  }
  real(RADIUS_ID())->setValue(aRadius);
  real(ANGLE_ID())->setValue(anAngle);
  data()->blockSendAttributeUpdated(aWasBlocked, false);
}

GeomShapePtr SketchPlugin_MacroArc::getArcShape(bool isBound)
{
  if(!myStart.get() || !myEnd.get() || !myCenter.get()) {
    return GeomShapePtr();
  }

  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return GeomShapePtr();
  }

  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(myCenter->x(), myCenter->y()));
  std::shared_ptr<GeomAPI_Pnt> aStart(aSketch->to3D(myStart->x(), myStart->y()));
  std::shared_ptr<GeomAPI_Pnt> anEnd(aSketch->to3D(myEnd->x(), myEnd->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir =
    std::dynamic_pointer_cast<GeomDataAPI_Dir>(aSketch->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

  GeomShapePtr anArcShape;
  if (isBound) {
    anArcShape = boolean(REVERSED_ID())->value() ?
        GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, anEnd, aStart, aNormal)
      : GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStart, anEnd, aNormal);
  } else {
    double aRadius = aCenter->distance(aStart);
    anArcShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aRadius);
  }

  return anArcShape;
}

AISObjectPtr SketchPlugin_MacroArc::getAISObject(AISObjectPtr thePrevious)
{
  GeomShapePtr anArcShape = getArcShape();
  if(!anArcShape.get())
    return AISObjectPtr();

  SketchPlugin_Sketch* aSketch = sketch();
  std::shared_ptr<GeomAPI_Pnt> aCenter = aSketch->to3D(myCenter->x(), myCenter->y());;
  GeomShapePtr aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);

  if(!aCenterPointShape.get())
    return AISObjectPtr();

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  aShapes.push_back(anArcShape);
  aShapes.push_back(aCenterPointShape);

  std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  AISObjectPtr anAIS = thePrevious;
  if(!anAIS.get()) {
    anAIS.reset(new GeomAPI_AISObject());
  }
  anAIS->createShape(aCompound);
  return anAIS;
}

void SketchPlugin_MacroArc::execute()
{
  FeaturePtr anArcFeature = createArcFeature();

  myCenter.reset();
  myStart.reset();
  myEnd.reset();

  // Create constraints.
  std::string anArcType = string(ARC_TYPE())->value();
  if(anArcType == ARC_TYPE_BY_CENTER_AND_POINTS()) {
    SketchPlugin_Tools::createCoincidenceOrTangency(this,
                                         CENTER_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                                         ObjectPtr(),
                                         false);
    SketchPlugin_Tools::createCoincidenceOrTangency(this,
                                         START_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::START_ID()),
                                         ObjectPtr(),
                                         false);
    SketchPlugin_Tools::createCoincidenceOrTangency(this,
                                         END_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::END_ID()),
                                         ObjectPtr(),
                                         false);
  } else if(anArcType == ARC_TYPE_BY_THREE_POINTS()) {
    SketchPlugin_Tools::createCoincidenceOrTangency(this,
                                         START_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::START_ID()),
                                         ObjectPtr(),
                                         false);
    SketchPlugin_Tools::createCoincidenceOrTangency(this,
                                         END_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::END_ID()),
                                         ObjectPtr(),
                                         false);
    SketchPlugin_Tools::createCoincidenceOrTangency(this,
                                         PASSED_POINT_REF_ID(),
                                         AttributePtr(),
                                         anArcFeature->lastResult(),
                                         true);
  } else if(anArcType == ARC_TYPE_BY_TANGENT_EDGE()) {
    // constraints for tangent arc
    SketchPlugin_Tools::createCoincidenceOrTangency(this,
                                         TANGENT_POINT_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::START_ID()),
                                         ObjectPtr(),
                                         false);
    FeaturePtr aTangent = sketch()->addFeature(SketchPlugin_ConstraintTangent::ID());
    AttributeRefAttrPtr aRefAttrA = aTangent->refattr(SketchPlugin_Constraint::ENTITY_A());
    AttributeRefAttrPtr aTgPntRefAttr = refattr(TANGENT_POINT_ID());
    FeaturePtr aTgFeature = ModelAPI_Feature::feature(aTgPntRefAttr->attr()->owner());
    aRefAttrA->setObject(aTgFeature->lastResult());
    AttributeRefAttrPtr aRefAttrB = aTangent->refattr(SketchPlugin_Constraint::ENTITY_B());
    aRefAttrB->setObject(anArcFeature->lastResult());
    // constraint for end point
    SketchPlugin_Tools::createCoincidenceOrTangency(this,
                                         END_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::END_ID()),
                                         ObjectPtr(),
                                         false);
  }

  // message to init reentrant operation
  static Events_ID anId = SketchPlugin_MacroArcReentrantMessage::eventId();
  std::shared_ptr<SketchPlugin_MacroArcReentrantMessage> aMessage = std::shared_ptr
    <SketchPlugin_MacroArcReentrantMessage>(new SketchPlugin_MacroArcReentrantMessage(anId, this));

  std::string anEditArcType = string(EDIT_ARC_TYPE_ID())->value();
  aMessage->setTypeOfCreation(!anEditArcType.empty() ? anEditArcType : anArcType);
  aMessage->setCreatedFeature(anArcFeature);
  Events_Loop::loop()->send(aMessage);
}

// LCOV_EXCL_START
std::string SketchPlugin_MacroArc::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  std::string aFilledAttributeName;
  std::shared_ptr<SketchPlugin_MacroArcReentrantMessage> aReentrantMessage =
        std::dynamic_pointer_cast<SketchPlugin_MacroArcReentrantMessage>(theMessage);
  if (aReentrantMessage.get()) {
    FeaturePtr aCreatedFeature = aReentrantMessage->createdFeature();
    std::string anArcType = aReentrantMessage->typeOfCreation();

    string(ARC_TYPE())->setValue(anArcType);

    aFilledAttributeName = ARC_TYPE();
    if(anArcType == ARC_TYPE_BY_TANGENT_EDGE()) {
      aFilledAttributeName = TANGENT_POINT_ID();
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                                        attribute(aFilledAttributeName));
      FeaturePtr aCreatedFeature = aReentrantMessage->createdFeature();
      aRefAttr->setAttr(aCreatedFeature->attribute(SketchPlugin_Arc::END_ID()));
    }
    else {
      ObjectPtr anObject = aReentrantMessage->selectedObject();
      AttributePtr anAttribute = aReentrantMessage->selectedAttribute();
      std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = aReentrantMessage->clickedPoint();

      if (aClickedPoint.get() && (anObject.get() || anAttribute.get())) {
        if (anArcType == ARC_TYPE_BY_CENTER_AND_POINTS() ||
            anArcType == ARC_TYPE_BY_THREE_POINTS()) {
          std::string aReferenceAttributeName;
          if (anArcType == ARC_TYPE_BY_CENTER_AND_POINTS()) {
            aFilledAttributeName = CENTER_POINT_ID();
            aReferenceAttributeName = CENTER_POINT_REF_ID();
          }
          else {
            aFilledAttributeName = START_POINT_2_ID();
            aReferenceAttributeName = START_POINT_REF_ID();
          }
          // fill 2d point attribute
          AttributePoint2DPtr aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                            attribute(aFilledAttributeName));
          aPointAttr->setValue(aClickedPoint);
          // fill reference attribute
          AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                                          attribute(aReferenceAttributeName));
          if (aRefAttr.get()) {
            if (anAttribute.get()) {
              if (!anAttribute->owner().get() || !anAttribute->owner()->data()->isValid()) {
                FeaturePtr aCreatedFeature = aReentrantMessage->createdFeature();
                if (aCreatedFeature.get()) {
                  std::string anID = anAttribute->id();
                  std::string anArcID;
                  if (anID == END_POINT_1_ID() || anID == END_POINT_2_ID() ||
                      anID == END_POINT_3_ID())
                    anArcID = SketchPlugin_Arc::END_ID();
                  else if (anID == START_POINT_1_ID() || anID ==START_POINT_2_ID())
                    anArcID = SketchPlugin_Arc::START_ID();
                  else if (anID == CENTER_POINT_ID())
                    anArcID = SketchPlugin_Arc::CENTER_ID();
                  anAttribute = aCreatedFeature->attribute(anArcID);
                }
              }
              aRefAttr->setAttr(anAttribute);
            }
            else if (anObject.get()) {
              // if attribute is NULL, only object is defined, it should be processed outside
              // the feature because it might be an external feature, that will be
              // removed/created again after restart operation
              // #2468 - Crash when sketching circles successively on a repetition
              aFilledAttributeName = ARC_TYPE();
            }
          }
        }
      }
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return aFilledAttributeName;
}
// LCOV_EXCL_STOP

FeaturePtr SketchPlugin_MacroArc::createArcFeature()
{
  FeaturePtr anArcFeature = sketch()->addFeature(SketchPlugin_Arc::ID());
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->setValue(myCenter);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::START_ID()))->setValue(myStart);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::END_ID()))->setValue(myEnd);
  anArcFeature->boolean(SketchPlugin_Arc::REVERSED_ID())
                ->setValue(boolean(REVERSED_ID())->value());
  anArcFeature->boolean(SketchPlugin_Arc::AUXILIARY_ID())
                ->setValue(boolean(AUXILIARY_ID())->value());
  anArcFeature->execute();

  return anArcFeature;
}

void SketchPlugin_MacroArc::fillByCenterAndTwoPassed()
{
  AttributePoint2DPtr aCenterPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_POINT_ID()));
  if (!aCenterPointAttr->isInitialized())
      return;

  AttributePoint2DPtr aStartPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_POINT_1_ID()));
  if (!aStartPointAttr->isInitialized())
    return;

  myCenter = aCenterPointAttr->pnt();
  myStart = aStartPointAttr->pnt();
  myEnd = myStart;

  AttributePoint2DPtr anEndPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_1_ID()));
  if (!anEndPointAttr->isInitialized())
    return;

  GeomAPI_Circ2d aCircleForArc(myCenter, myStart);
  // avoid degerated arc, when the center and the start points are equal
  if (!aCircleForArc.implPtr<void*>())
    return;

  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  // check the end point is referred to another feature
  GeomShapePtr aRefShape;
  AttributeRefAttrPtr aEndPointRefAttr = refattr(END_POINT_REF_ID());
  if (aEndPointRefAttr && aEndPointRefAttr->isInitialized()) {
    if (aEndPointRefAttr->isObject()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aEndPointRefAttr->object());
      if (aFeature)
        aRefShape = aFeature->lastResult()->shape();
    }
  }
  if (aRefShape) {
    // Calculate end point as an intersection between circle and another shape
    intersectShapeAndCircle(aRefShape, aCircleForArc, sketch(), anEndPointAttr);
  } else {
    // End point should be a projection on circle.
    projectPointOnCircle(anEndPointAttr, aCircleForArc);
  }
  data()->blockSendAttributeUpdated(aWasBlocked, false);
  myEnd = anEndPointAttr->pnt();

  // update the REVERSED flag
  recalculateReversedFlagByEnd(aCircleForArc);
}

void SketchPlugin_MacroArc::recalculateReversedFlagByEnd(const GeomAPI_Circ2d& theCurrentCircular)
{
  double aParameterNew = 0.0;
  if(theCurrentCircular.parameter(myEnd, paramTolerance, aParameterNew)) {
    if(myParamBefore <= PI / 2.0 && aParameterNew >= PI * 1.5) {
      boolean(REVERSED_ID())->setValue(true);
    } else if(myParamBefore >= PI * 1.5 && aParameterNew <= PI / 2.0) {
      boolean(REVERSED_ID())->setValue(false);
    }
  }
  myParamBefore = aParameterNew;
}

void SketchPlugin_MacroArc::fillByThreePassedPoints()
{
  AttributePoint2DPtr aStartPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_POINT_2_ID()));
  if (!aStartPointAttr->isInitialized())
    return;

  AttributePoint2DPtr anEndPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_2_ID()));
  if (!anEndPointAttr->isInitialized())
    return;

  myStart = aStartPointAttr->pnt();
  myEnd = anEndPointAttr->pnt();

  AttributePoint2DPtr aPassedPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(PASSED_POINT_ID()));
  if (aPassedPointAttr->isInitialized()) {
    std::shared_ptr<GeomAPI_Pnt2d> aPassedPnt;
    std::shared_ptr<GeomAPI_Shape> aTangentCurve;
    SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
        refattr(PASSED_POINT_REF_ID()), aPassedPointAttr, aTangentCurve, aPassedPnt);

    GeomAlgoAPI_Circ2dBuilder aCircBuilder(SketchPlugin_Sketch::plane(sketch()));
    aCircBuilder.addPassingPoint(myStart);
    aCircBuilder.addPassingPoint(myEnd);
    if (aTangentCurve) {
      aCircBuilder.addTangentCurve(aTangentCurve);
      aCircBuilder.setClosestPoint(aPassedPointAttr->pnt());
    } else
      aCircBuilder.addPassingPoint(aPassedPnt);

    std::shared_ptr<GeomAPI_Circ2d> aCircle = aCircBuilder.circle();
    if (!aCircle)
      return;
    myCenter = aCircle->center();

    aCircle = std::shared_ptr<GeomAPI_Circ2d>(new GeomAPI_Circ2d(myCenter, myStart));
    recalculateReversedFlagByPassed(*aCircle);
  } else
    myCenter.reset(new GeomAPI_Pnt2d(myStart->xy()->added(myEnd->xy())->multiplied(0.5)));
}

void SketchPlugin_MacroArc::recalculateReversedFlagByPassed(
    const GeomAPI_Circ2d& theCurrentCircular)
{
  AttributePoint2DPtr aPassedAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(PASSED_POINT_ID()));
  std::shared_ptr<GeomAPI_Pnt2d> aPassed = theCurrentCircular.project(aPassedAttr->pnt());

  double aEndParam, aPassedParam;
  theCurrentCircular.parameter(myEnd, paramTolerance, aEndParam);
  theCurrentCircular.parameter(aPassed, paramTolerance, aPassedParam);

  if(aPassedParam > aEndParam)
    boolean(REVERSED_ID())->setValue(true);
  else
    boolean(REVERSED_ID())->setValue(false);

  myParamBefore = aEndParam;
}

void SketchPlugin_MacroArc::fillByTangentEdge()
{
  AttributeRefAttrPtr aTangentAttr = refattr(TANGENT_POINT_ID());
  if (!aTangentAttr->isInitialized())
    return;

  AttributePoint2DPtr aTangentPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aTangentAttr->attr());
  if (!aTangentPointAttr->isInitialized())
    return;

  AttributePoint2DPtr anEndPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_3_ID()));
  if (!anEndPointAttr->isInitialized())
    return;

  myStart = aTangentPointAttr->pnt();
  myEnd = anEndPointAttr->pnt();
  if (myStart->isEqual(myEnd))
    return;

  // obtain a shape the tangent point belongs to
  FeaturePtr aTangentFeature = ModelAPI_Feature::feature(aTangentPointAttr->owner());
  std::shared_ptr<GeomAPI_Shape> aTangentShape = aTangentFeature->lastResult()->shape();

  GeomAlgoAPI_Circ2dBuilder aCircBuilder(SketchPlugin_Sketch::plane(sketch()));
  aCircBuilder.addPassingPoint(myStart);
  aCircBuilder.addPassingPoint(myEnd);
  aCircBuilder.addTangentCurve(aTangentShape);

  std::shared_ptr<GeomAPI_Circ2d> aCircle = aCircBuilder.circle();
  if (!aCircle)
    return;
  myCenter = aCircle->center();

  // rebuild circle to set start point equal to zero parameter
  aCircle = std::shared_ptr<GeomAPI_Circ2d>(new GeomAPI_Circ2d(myCenter, myStart));
  recalculateReversedFlagByEnd(*aCircle);
}
