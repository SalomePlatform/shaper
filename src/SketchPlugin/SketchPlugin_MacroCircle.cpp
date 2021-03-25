// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "SketchPlugin_MacroCircle.h"

#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Tools.h"
#include "SketchPlugin_MacroArcReentrantMessage.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Events.h>

#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>

#include <GeomAlgoAPI_Circ2dBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>


const double tolerance = 1e-7;

namespace {
  static const std::string& POINT_ID(int theIndex)
  {
    switch (theIndex) {
      case 1: return SketchPlugin_MacroCircle::FIRST_POINT_ID();
      case 2: return SketchPlugin_MacroCircle::SECOND_POINT_ID();
      case 3: return SketchPlugin_MacroCircle::THIRD_POINT_ID();
    }

    static const std::string DUMMY;
    return DUMMY;
  }
}


SketchPlugin_MacroCircle::SketchPlugin_MacroCircle()
: SketchPlugin_SketchEntity(),
  myRadius(0.0)
{
}

void SketchPlugin_MacroCircle::initAttributes()
{
  data()->addAttribute(CIRCLE_TYPE(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EDIT_CIRCLE_TYPE(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(CENTER_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(CENTER_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(PASSED_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(PASSED_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(FIRST_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SECOND_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(THIRD_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(FIRST_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SECOND_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(THIRD_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(CIRCLE_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  string(EDIT_CIRCLE_TYPE())->setValue("");

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CENTER_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PASSED_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FIRST_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SECOND_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), THIRD_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EDIT_CIRCLE_TYPE());
}

void SketchPlugin_MacroCircle::execute()
{
  FeaturePtr aCircle = createCircleFeature();

  std::string aType = string(CIRCLE_TYPE())->value();
  if (aType == CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS())
    constraintsForCircleByCenterAndPassed(aCircle);
  else if (aType == CIRCLE_TYPE_BY_THREE_POINTS())
    constraintsForCircleByThreePoints(aCircle);

  // message to init reentrant operation
  static Events_ID anId = SketchPlugin_MacroArcReentrantMessage::eventId();
  std::shared_ptr<SketchPlugin_MacroArcReentrantMessage> aMessage = std::shared_ptr
    <SketchPlugin_MacroArcReentrantMessage>(new SketchPlugin_MacroArcReentrantMessage(anId, this));

  std::string anEditType = string(EDIT_CIRCLE_TYPE())->value();
  aMessage->setTypeOfCreation(!anEditType.empty() ? anEditType : aType);
  aMessage->setCreatedFeature(aCircle);
  Events_Loop::loop()->send(aMessage);
}

// LCOV_EXCL_START
std::string SketchPlugin_MacroCircle::processEvent(
                                              const std::shared_ptr<Events_Message>& theMessage)
{
  std::string aFilledAttributeName;
  std::shared_ptr<SketchPlugin_MacroArcReentrantMessage> aReentrantMessage =
        std::dynamic_pointer_cast<SketchPlugin_MacroArcReentrantMessage>(theMessage);
  if (aReentrantMessage.get()) {
    std::string aCircleType = aReentrantMessage->typeOfCreation();

    string(CIRCLE_TYPE())->setValue(aCircleType);

    aFilledAttributeName = CIRCLE_TYPE();
    ObjectPtr anObject = aReentrantMessage->selectedObject();
    AttributePtr anAttribute = aReentrantMessage->selectedAttribute();
    std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = aReentrantMessage->clickedPoint();

    if (aClickedPoint.get() && (anObject.get() || anAttribute.get())) {
      std::string aReferenceAttributeName;
      if (aCircleType == CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS()) {
        aFilledAttributeName = CENTER_POINT_ID();
        aReferenceAttributeName = CENTER_POINT_REF_ID();
      }
      else {
        aFilledAttributeName = FIRST_POINT_ID();
        aReferenceAttributeName = FIRST_POINT_REF_ID();
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
              if (anID == CENTER_POINT_ID())
                anArcID = SketchPlugin_Circle::CENTER_ID();
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
          aFilledAttributeName = CIRCLE_TYPE();
        }
      }
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return aFilledAttributeName;
}
// LCOV_EXCL_STOP

void SketchPlugin_MacroCircle::constraintsForCircleByCenterAndPassed(FeaturePtr theCircleFeature)
{
  // Create constraints.
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, CENTER_POINT_REF_ID(),
      theCircleFeature->attribute(SketchPlugin_Circle::CENTER_ID()),
      ObjectPtr(), false);
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, PASSED_POINT_REF_ID(), AttributePtr(),
      theCircleFeature->lastResult(), true);
}

void SketchPlugin_MacroCircle::constraintsForCircleByThreePoints(FeaturePtr theCircleFeature)
{
  std::string aPointRef[3] = { FIRST_POINT_REF_ID(),
                               SECOND_POINT_REF_ID(),
                               THIRD_POINT_REF_ID() };

  // Create constraints.
  ResultPtr aCircleResult = theCircleFeature->lastResult();
  for (int i = 0; i < 3; ++i) {
    SketchPlugin_Tools::createCoincidenceOrTangency(
        this, aPointRef[i], AttributePtr(), aCircleResult, true);
  }
}

FeaturePtr SketchPlugin_MacroCircle::createCircleFeature()
{
  FeaturePtr aCircleFeature = sketch()->addFeature(SketchPlugin_Circle::ID());
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aCircleFeature->attribute(SketchPlugin_Circle::CENTER_ID()))->setValue(myCenter->x(),
                                                                             myCenter->y());
  aCircleFeature->real(SketchPlugin_Circle::RADIUS_ID())->setValue(myRadius);
  aCircleFeature->boolean(SketchPlugin_Circle::AUXILIARY_ID())
                ->setValue(boolean(AUXILIARY_ID())->value());
  aCircleFeature->execute();
  return aCircleFeature;
}

void SketchPlugin_MacroCircle::fillByCenterAndPassed()
{
  AttributePtr aCenterAttr = attribute(CENTER_POINT_ID());
  AttributePtr aPassedAttr = attribute(PASSED_POINT_ID());
  if (!aCenterAttr->isInitialized() || !aPassedAttr->isInitialized())
    return;

  // Calculate circle parameters
  AttributeRefAttrPtr aCenterRef = refattr(CENTER_POINT_REF_ID());
  std::shared_ptr<GeomAPI_Pnt2d> aCenter;
  std::shared_ptr<GeomAPI_Shape> aCurve;
  SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
      aCenterRef, aCenterAttr, aCurve, aCenter);
  if (!aCenter)
    aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aCenterAttr)->pnt();
  AttributeRefAttrPtr aPassedRef = refattr(PASSED_POINT_REF_ID());
  std::shared_ptr<GeomAPI_Pnt2d> aPassedPoint;
  std::shared_ptr<GeomAPI_Shape> aTangentCurve;
  SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
      aPassedRef, aPassedAttr, aTangentCurve, aPassedPoint);

  // Build a circle
  GeomAlgoAPI_Circ2dBuilder aCircBuilder(SketchPlugin_Sketch::plane(sketch()));
  aCircBuilder.setCenter(aCenter);
  if (aTangentCurve) {
    aCircBuilder.addTangentCurve(aTangentCurve);

    AttributePoint2DPtr aPassedPntAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aPassedAttr);
    if (aPassedPntAttr)
      aCircBuilder.setClosestPoint(aPassedPntAttr->pnt());
  } else
    aCircBuilder.addPassingPoint(aPassedPoint);

  std::shared_ptr<GeomAPI_Circ2d> aCircle = aCircBuilder.circle();
  if (aCircle) {
    myCenter = aCircle->center();
    myRadius = aCircle->radius();
  }
}

void SketchPlugin_MacroCircle::fillByThreePoints()
{
  std::string aPointAttr[3] = { FIRST_POINT_ID(),
                                SECOND_POINT_ID(),
                                THIRD_POINT_ID() };
  std::string aPointRef[3] = { FIRST_POINT_REF_ID(),
                               SECOND_POINT_REF_ID(),
                               THIRD_POINT_REF_ID() };

  GeomAlgoAPI_Circ2dBuilder aCircBuilder(SketchPlugin_Sketch::plane(sketch()));

  for (int aPntIndex = 0; aPntIndex < 3; ++aPntIndex) {
    AttributePtr aPassedAttr = attribute(aPointAttr[aPntIndex]);
    if (!aPassedAttr->isInitialized())
      break;

    AttributeRefAttrPtr aPassedRef = refattr(aPointRef[aPntIndex]);
    // calculate circle parameters
    std::shared_ptr<GeomAPI_Pnt2d> aPassedPoint;
    std::shared_ptr<GeomAPI_Shape> aTangentCurve;
    SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
        aPassedRef, aPassedAttr, aTangentCurve, aPassedPoint);

    if (aPassedPoint)
      aCircBuilder.addPassingPoint(aPassedPoint);
    else {
      aCircBuilder.addTangentCurve(aTangentCurve);
      AttributePoint2DPtr aPassedPointAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aPassedAttr);
      if (aPassedPointAttr)
        aCircBuilder.setClosestPoint(aPassedPointAttr->pnt());
    }
  }

  std::shared_ptr<GeomAPI_Circ2d> aCircle = aCircBuilder.circle();
  if (aCircle) {
    myCenter = aCircle->center();
    myRadius = aCircle->radius();
  }
}

void SketchPlugin_MacroCircle::fillByTwoPassedPoints()
{
  std::string aPointAttr[2] = { FIRST_POINT_ID(),
                                SECOND_POINT_ID() };
  std::string aPointRef[2] = { FIRST_POINT_REF_ID(),
                               SECOND_POINT_REF_ID() };

  GeomAlgoAPI_Circ2dBuilder aCircBuilder(SketchPlugin_Sketch::plane(sketch()));

  std::shared_ptr<GeomAPI_Pnt2d> aPassedPoints[2]; // there is possible only two passed points
  bool hasTangentCurve = false;
  int aPntIndex = 0;
  for (; aPntIndex < 2; ++aPntIndex) {
    AttributePtr aPassedAttr = attribute(aPointAttr[aPntIndex]);
    if (!aPassedAttr->isInitialized())
      break;

    AttributeRefAttrPtr aPassedRef = refattr(aPointRef[aPntIndex]);
    // calculate circle parameters
    std::shared_ptr<GeomAPI_Pnt2d> aPassedPoint;
    std::shared_ptr<GeomAPI_Shape> aTangentCurve;
    SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
        aPassedRef, aPassedAttr, aTangentCurve, aPassedPoint);

    if (aPassedPoint) {
      aCircBuilder.addPassingPoint(aPassedPoint);
      aPassedPoints[aPntIndex] = aPassedPoint;
    } else {
      hasTangentCurve = true;
      aCircBuilder.addTangentCurve(aTangentCurve);
      // if the circle is tangent to any curve,
      // the third point will be initialized by the tangent point
      aCircBuilder.addPassingPoint(
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aPassedAttr)->pnt());
    }
  }
  if (aPntIndex <= 1)
    return;

  std::shared_ptr<GeomAPI_Circ2d> aCircle;

  if (hasTangentCurve)
    aCircle = aCircBuilder.circle();
  else {
    // the circle is defined by two points, calculate its parameters manually
    std::shared_ptr<GeomAPI_Pnt2d> aCenter(new GeomAPI_Pnt2d(
        (aPassedPoints[0]->x() + aPassedPoints[1]->x()) * 0.5,
        (aPassedPoints[0]->y() + aPassedPoints[1]->y()) * 0.5));
    aCircle = std::shared_ptr<GeomAPI_Circ2d>(new GeomAPI_Circ2d(aCenter, aPassedPoints[0]));
  }

  if (aCircle) {
    myCenter = aCircle->center();
    myRadius = aCircle->radius();
  }
}

AISObjectPtr SketchPlugin_MacroCircle::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch || !myCenter || myRadius == 0) {
    return AISObjectPtr();
  }

  // Compute a circle in 3D view.
  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(myCenter->x(), myCenter->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir =
      std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  GeomShapePtr aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, myRadius);
  GeomShapePtr aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
  if(!aCircleShape.get() || !aCenterPointShape.get()) {
    return AISObjectPtr();
  }

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  aShapes.push_back(aCircleShape);
  aShapes.push_back(aCenterPointShape);

  std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  AISObjectPtr anAIS = thePrevious;
  if(!anAIS.get()) {
    anAIS.reset(new GeomAPI_AISObject());
  }
  anAIS->createShape(aCompound);

  // Modify attributes
  SketchPlugin_Tools::customizeFeaturePrs(anAIS, boolean(AUXILIARY_ID())->value());

  return anAIS;
}

void SketchPlugin_MacroCircle::attributeChanged(const std::string& theID) {
  // If circle type switched reset all attributes.
  if(theID == CIRCLE_TYPE()) {
    SketchPlugin_Tools::resetAttribute(this, CENTER_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, CENTER_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, PASSED_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, PASSED_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, FIRST_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, FIRST_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, SECOND_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, SECOND_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, THIRD_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, THIRD_POINT_REF_ID());
  } else if(theID == CENTER_POINT_ID() || theID == PASSED_POINT_ID() ||
            theID == CENTER_POINT_REF_ID() || theID == PASSED_POINT_REF_ID())
    fillByCenterAndPassed();
  else if(theID == FIRST_POINT_ID() || theID == FIRST_POINT_REF_ID() ||
          theID == SECOND_POINT_ID() || theID == SECOND_POINT_REF_ID() ||
          theID == THIRD_POINT_ID() || theID == THIRD_POINT_REF_ID()) {
    std::shared_ptr<GeomAPI_Pnt2d> aPoints[3];
    int aNbInitialized = 0;
    for(int i = 1; i <= 3; ++i) {
      std::shared_ptr<GeomDataAPI_Point2D> aCurPnt =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(POINT_ID(i)));
      if(aCurPnt->isInitialized())
        aPoints[aNbInitialized++] = aCurPnt->pnt();
    }

    if(aNbInitialized == 1)
      return;
    else if(aNbInitialized == 2)
      fillByTwoPassedPoints();
    else
      fillByThreePoints();
  }

  AttributeDoublePtr aRadiusAttr = real(CIRCLE_RADIUS_ID());
  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  if(myCenter.get()) {
    // center attribute is used in processEvent() to set reference to reentrant arc
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_POINT_ID()))
        ->setValue(myCenter);
  }
  aRadiusAttr->setValue(myRadius);
  data()->blockSendAttributeUpdated(aWasBlocked, false);
}
