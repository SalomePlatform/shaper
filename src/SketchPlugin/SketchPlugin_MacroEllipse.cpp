// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#include <SketchPlugin_MacroEllipse.h>

#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_MacroArcReentrantMessage.h>
#include <SketchPlugin_Tools.h>
#include <SketchPlugin_Sketch.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Events.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Ellipse2d.h>
#include <GeomAPI_Vertex.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>


SketchPlugin_MacroEllipse::SketchPlugin_MacroEllipse()
: SketchPlugin_SketchEntity(),
  myMajorRadius(0.0),
  myMinorRadius(0.0)
{
}

void SketchPlugin_MacroEllipse::initAttributes()
{
  data()->addAttribute(ELLIPSE_TYPE(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EDIT_ELLIPSE_TYPE(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(FIRST_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(FIRST_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SECOND_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SECOND_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(PASSED_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(PASSED_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(MAJOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(MINOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  string(EDIT_ELLIPSE_TYPE())->setValue("");

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FIRST_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SECOND_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PASSED_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EDIT_ELLIPSE_TYPE());
}

void SketchPlugin_MacroEllipse::execute()
{
  FeaturePtr anEllipse = createEllipseFeature();

  std::string aType = string(ELLIPSE_TYPE())->value();
  if (aType == ELLIPSE_TYPE_BY_CENTER_AXIS_POINT())
    constraintsForEllipseByCenterAxisAndPassed(anEllipse);
  else if (aType == ELLIPSE_TYPE_BY_AXIS_AND_POINT())
    constraintsForEllipseByMajoxAxisAndPassed(anEllipse);

  // message to init reentrant operation
  static Events_ID anId = SketchPlugin_MacroArcReentrantMessage::eventId();
  std::shared_ptr<SketchPlugin_MacroArcReentrantMessage> aMessage = std::shared_ptr
    <SketchPlugin_MacroArcReentrantMessage>(new SketchPlugin_MacroArcReentrantMessage(anId, this));

  std::string anEditType = string(EDIT_ELLIPSE_TYPE())->value();
  aMessage->setTypeOfCreation(!anEditType.empty() ? anEditType : aType);
  aMessage->setCreatedFeature(anEllipse);
  Events_Loop::loop()->send(aMessage);
}

void SketchPlugin_MacroEllipse::attributeChanged(const std::string& theID)
{
  static const int NB_POINTS = 3;
  std::string aPointAttrName[NB_POINTS] = { FIRST_POINT_ID(),
                                            SECOND_POINT_ID(),
                                            PASSED_POINT_ID() };
  std::string aPointRefName[NB_POINTS] = { FIRST_POINT_REF_ID(),
                                           SECOND_POINT_REF_ID(),
                                           PASSED_POINT_REF_ID() };

  // type of ellipse switched, thus reset all attributes
  if (theID == ELLIPSE_TYPE()) {
    for (int aPntIndex = 0; aPntIndex < NB_POINTS; ++aPntIndex) {
      SketchPlugin_Tools::resetAttribute(this, aPointAttrName[aPntIndex]);
      SketchPlugin_Tools::resetAttribute(this, aPointRefName[aPntIndex]);
    }
  }
  else {
    int aNbInitialized = 0;
    GeomPnt2dPtr anEllipsePoints[NB_POINTS];

    for (int aPntIndex = 0; aPntIndex < NB_POINTS; ++aPntIndex) {
      AttributePtr aPointAttr = attribute(aPointAttrName[aPntIndex]);
      if (!aPointAttr->isInitialized())
        continue;

      AttributeRefAttrPtr aPointRef = refattr(aPointRefName[aPntIndex]);
      // calculate ellipse parameters
      GeomPnt2dPtr aPassedPoint;
      GeomShapePtr aTangentCurve;
      SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
        aPointRef, aPointAttr, aTangentCurve, aPassedPoint);

      anEllipsePoints[aNbInitialized++] = aPassedPoint;
    }

    if (aNbInitialized <= 1)
      return; // too few points for the ellipse

    if (string(ELLIPSE_TYPE())->value() == ELLIPSE_TYPE_BY_AXIS_AND_POINT()) {
      // ellipse is given by major axis and passing point,
      // recalculate the first point to be a center
      anEllipsePoints[0]->setX(0.5 * (anEllipsePoints[0]->x() + anEllipsePoints[1]->x()));
      anEllipsePoints[0]->setY(0.5 * (anEllipsePoints[0]->y() + anEllipsePoints[1]->y()));
    }

    std::shared_ptr<GeomAPI_Ellipse2d> anEllipse;
    if (aNbInitialized == 2) {
      GeomDir2dPtr aXDir(new GeomAPI_Dir2d(anEllipsePoints[1]->x() - anEllipsePoints[0]->x(),
                                           anEllipsePoints[1]->y() - anEllipsePoints[0]->y()));
      double aMajorRad = anEllipsePoints[1]->distance(anEllipsePoints[0]);
      anEllipse = std::shared_ptr<GeomAPI_Ellipse2d>(
          new GeomAPI_Ellipse2d(anEllipsePoints[0], aXDir, aMajorRad, 0.5 * aMajorRad));
    }
    else if (aNbInitialized == 3) {
      anEllipse = std::shared_ptr<GeomAPI_Ellipse2d>(
        new GeomAPI_Ellipse2d(anEllipsePoints[0], anEllipsePoints[1], anEllipsePoints[2]));
    }

    if (!anEllipse || anEllipse->implPtr<void>() == 0)
      return;

    myCenter = anEllipse->center();
    myFocus = anEllipse->firstFocus();
    myMajorRadius = anEllipse->majorRadius();
    myMinorRadius = anEllipse->minorRadius();

    AttributeDoublePtr aMajorRadiusAttr = real(MAJOR_RADIUS_ID());
    AttributeDoublePtr aMinorRadiusAttr = real(MINOR_RADIUS_ID());

    bool aWasBlocked = data()->blockSendAttributeUpdated(true);
    // center attribute is used in processEvent() to set reference to reentrant arc
////    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(FIRST_POINT_ID()))
////        ->setValue(myCenter);
    aMajorRadiusAttr->setValue(myMajorRadius);
    aMinorRadiusAttr->setValue(myMinorRadius);
    data()->blockSendAttributeUpdated(aWasBlocked, false);
  }
}

// LCOV_EXCL_START
std::string SketchPlugin_MacroEllipse::processEvent(
                                              const std::shared_ptr<Events_Message>& theMessage)
{
  std::string aFilledAttributeName;

  std::shared_ptr<SketchPlugin_MacroArcReentrantMessage> aReentrantMessage =
      std::dynamic_pointer_cast<SketchPlugin_MacroArcReentrantMessage>(theMessage);
  if (aReentrantMessage) {
    FeaturePtr aCreatedFeature = aReentrantMessage->createdFeature();
    std::string anEllipseType = aReentrantMessage->typeOfCreation();

    string(ELLIPSE_TYPE())->setValue(anEllipseType);

    aFilledAttributeName = ELLIPSE_TYPE();
    ObjectPtr anObject = aReentrantMessage->selectedObject();
    AttributePtr anAttribute = aReentrantMessage->selectedAttribute();
    std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = aReentrantMessage->clickedPoint();

    if (aClickedPoint && (anObject || anAttribute)) {
      aFilledAttributeName = FIRST_POINT_ID();
      std::string aReferenceAttributeName = FIRST_POINT_REF_ID();

      // fill 2d point attribute
      AttributePoint2DPtr aPointAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(aFilledAttributeName));
      aPointAttr->setValue(aClickedPoint);

      // fill reference attribute
      AttributeRefAttrPtr aRefAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(attribute(aReferenceAttributeName));
      if (anAttribute) {
        if (!anAttribute->owner() || !anAttribute->owner()->data()->isValid()) {
          if (aCreatedFeature && anAttribute->id() == FIRST_POINT_ID())
            anAttribute = aCreatedFeature->attribute(
                anEllipseType == ELLIPSE_TYPE_BY_CENTER_AXIS_POINT() ?
                SketchPlugin_Ellipse::CENTER_ID() :
                SketchPlugin_Ellipse::MAJOR_AXIS_START_ID());
        }
        aRefAttr->setAttr(anAttribute);
      }
      else if (anObject.get()) {
        // if attribute is NULL, only object is defined, it should be processed outside
        // the feature because it might be an external feature, that will be
        // removed/created again after restart operation
        // #2468 - Crash when sketching circles successively on a repetition
        aFilledAttributeName = ELLIPSE_TYPE();
      }
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return aFilledAttributeName;
}
// LCOV_EXCL_STOP

void SketchPlugin_MacroEllipse::constraintsForEllipseByCenterAxisAndPassed(
    FeaturePtr theEllipseFeature)
{
  // tangency on-the-fly is not applicable for ellipses
  static const bool isTangencyApplicable = false;
  // Create constraints.
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, FIRST_POINT_REF_ID(),
      theEllipseFeature->attribute(SketchPlugin_Ellipse::CENTER_ID()),
      ObjectPtr(), isTangencyApplicable);
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, SECOND_POINT_REF_ID(),
      theEllipseFeature->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_END_ID()),
      ObjectPtr(), isTangencyApplicable);
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, PASSED_POINT_REF_ID(), AttributePtr(),
      theEllipseFeature->lastResult(), isTangencyApplicable);
}

void SketchPlugin_MacroEllipse::constraintsForEllipseByMajoxAxisAndPassed(
    FeaturePtr theEllipseFeature)
{
  // tangency on-the-fly is not applicable for ellipses
  static const bool isTangencyApplicable = false;
  // Create constraints.
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, FIRST_POINT_REF_ID(),
      theEllipseFeature->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_START_ID()),
      ObjectPtr(), isTangencyApplicable);
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, SECOND_POINT_REF_ID(),
      theEllipseFeature->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_END_ID()),
      ObjectPtr(), isTangencyApplicable);
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, PASSED_POINT_REF_ID(), AttributePtr(),
      theEllipseFeature->lastResult(), isTangencyApplicable);
}

FeaturePtr SketchPlugin_MacroEllipse::createEllipseFeature()
{
  FeaturePtr aEllipseFeature = sketch()->addFeature(SketchPlugin_Ellipse::ID());

  AttributePoint2DPtr aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aEllipseFeature->attribute(SketchPlugin_Ellipse::CENTER_ID()));
  aCenterAttr->setValue(myCenter->x(), myCenter->y());

  AttributePoint2DPtr aFocusAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aEllipseFeature->attribute(SketchPlugin_Ellipse::FIRST_FOCUS_ID()));
  aFocusAttr->setValue(myFocus->x(), myFocus->y());

  aEllipseFeature->real(SketchPlugin_Ellipse::MAJOR_RADIUS_ID())->setValue(myMajorRadius);
  aEllipseFeature->real(SketchPlugin_Ellipse::MINOR_RADIUS_ID())->setValue(myMinorRadius);

  aEllipseFeature->boolean(SketchPlugin_Ellipse::AUXILIARY_ID())->setValue(
      boolean(AUXILIARY_ID())->value());

  aEllipseFeature->execute();
  return aEllipseFeature;
}

AISObjectPtr SketchPlugin_MacroEllipse::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch || !myCenter || myMajorRadius == 0)
    return AISObjectPtr();

  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  // Compute a ellipse in 3D view.
  GeomPointPtr aCenter(aSketch->to3D(myCenter->x(), myCenter->y()));
  GeomPointPtr aFocus(aSketch->to3D(myFocus->x(), myFocus->y()));
  GeomDirPtr aNormal = aNDir->dir();
  GeomDirPtr aMajorAxis(new GeomAPI_Dir(aFocus->x() - aCenter->x(),
      aFocus->y() - aCenter->y(), aFocus->z() - aCenter->z()));

  std::shared_ptr<GeomAPI_Shape> anEllipseShape =
      GeomAlgoAPI_EdgeBuilder::ellipse(aCenter, aNormal, aMajorAxis, myMajorRadius, myMinorRadius);
  GeomShapePtr aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
  if (!anEllipseShape.get() || !aCenterPointShape.get())
    return AISObjectPtr();

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  aShapes.push_back(anEllipseShape);
  aShapes.push_back(aCenterPointShape);

  std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS.reset(new GeomAPI_AISObject());
  anAIS->createShape(aCompound);
  return anAIS;
}
