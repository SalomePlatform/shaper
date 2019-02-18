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

// File:        SketchPlugin_MacroEllipse.cpp
// Created:     26 April 2017
// Author:      Artem ZHIDKOV

#include <SketchPlugin_MacroEllipse.h>

#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_Tools.h>
#include <SketchPlugin_Sketch.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_EventReentrantMessage.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Events.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
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
  data()->addAttribute(CENTER_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(CENTER_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(MAJOR_AXIS_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(MAJOR_AXIS_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(PASSED_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(PASSED_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(MAJOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(MINOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CENTER_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->
      registerNotObligatory(getKind(), MAJOR_AXIS_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PASSED_POINT_REF_ID());
}

void SketchPlugin_MacroEllipse::execute()
{
  FeaturePtr anEllipse = createEllipseFeature();
  constraintsForEllipse(anEllipse);

  // message to init reentrant operation
  static Events_ID anId = ModelAPI_EventReentrantMessage::eventId();
  ReentrantMessagePtr aMessage(new ModelAPI_EventReentrantMessage(anId, this));
  aMessage->setCreatedFeature(anEllipse);
  Events_Loop::loop()->send(aMessage);
}

void SketchPlugin_MacroEllipse::attributeChanged(const std::string& theID)
{
  static const int NB_POINTS = 3;
  std::string aPointAttrName[NB_POINTS] = { CENTER_POINT_ID(),
                                            MAJOR_AXIS_POINT_ID(),
                                            PASSED_POINT_ID() };
  std::string aPointRefName[NB_POINTS] = { CENTER_POINT_REF_ID(),
                                           MAJOR_AXIS_POINT_REF_ID(),
                                           PASSED_POINT_REF_ID() };

  int aNbInitialized = 0;
  std::shared_ptr<GeomAPI_Pnt2d> anEllipsePoints[NB_POINTS];

  for (int aPntIndex = 0; aPntIndex < NB_POINTS; ++aPntIndex) {
    AttributePtr aPointAttr = attribute(aPointAttrName[aPntIndex]);
    if (!aPointAttr->isInitialized())
      continue;

    AttributeRefAttrPtr aPointRef = refattr(aPointRefName[aPntIndex]);
    // calculate ellipse parameters
    std::shared_ptr<GeomAPI_Pnt2d> aPassedPoint;
    std::shared_ptr<GeomAPI_Shape> aTangentCurve;
    SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
        aPointRef, aPointAttr, aTangentCurve, aPassedPoint);

    anEllipsePoints[aNbInitialized++] = aPassedPoint;
  }

  std::shared_ptr<GeomAPI_Ellipse2d> anEllipse;
  if (aNbInitialized == 2) {
    std::shared_ptr<GeomAPI_Dir2d> aXDir(new GeomAPI_Dir2d(
        anEllipsePoints[1]->x() - anEllipsePoints[0]->x(),
        anEllipsePoints[1]->y() - anEllipsePoints[0]->y()));
    double aMajorRad = anEllipsePoints[1]->distance(anEllipsePoints[0]);
    anEllipse = std::shared_ptr<GeomAPI_Ellipse2d>(
        new GeomAPI_Ellipse2d(anEllipsePoints[0], aXDir, aMajorRad, 0.5 * aMajorRad));
  } else if (aNbInitialized == 3) {
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
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_POINT_ID()))->setValue(myCenter);
  aMajorRadiusAttr->setValue(myMajorRadius);
  aMinorRadiusAttr->setValue(myMinorRadius);
  data()->blockSendAttributeUpdated(aWasBlocked, false);
}

std::string SketchPlugin_MacroEllipse::processEvent(
                                              const std::shared_ptr<Events_Message>& theMessage)
{
  std::string aFilledAttributeName;

  ReentrantMessagePtr aReentrantMessage =
        std::dynamic_pointer_cast<ModelAPI_EventReentrantMessage>(theMessage);
  if (aReentrantMessage) {
    FeaturePtr aCreatedFeature = aReentrantMessage->createdFeature();
    ObjectPtr anObject = aReentrantMessage->selectedObject();
    AttributePtr anAttribute = aReentrantMessage->selectedAttribute();
    std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = aReentrantMessage->clickedPoint();

    if (aClickedPoint && (anObject || anAttribute)) {
      aFilledAttributeName = CENTER_POINT_ID();
      std::string aReferenceAttributeName = CENTER_POINT_REF_ID();

      // fill 2d point attribute
      AttributePoint2DPtr aPointAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(aFilledAttributeName));
      aPointAttr->setValue(aClickedPoint);

      // fill reference attribute
      AttributeRefAttrPtr aRefAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(attribute(aReferenceAttributeName));
      if (anAttribute) {
        if (!anAttribute->owner() || !anAttribute->owner()->data()->isValid()) {
          if (aCreatedFeature && anAttribute->id() == CENTER_POINT_ID())
            anAttribute = aCreatedFeature->attribute(SketchPlugin_Ellipse::CENTER_ID());
        }
        aRefAttr->setAttr(anAttribute);
      }
      else if (anObject.get()) {
        // if presentation of previous reentrant macro arc is used, the object is invalid,
        // we should use result of previous feature of the message(Arc)
        if (!anObject->data()->isValid())
          anObject = aCreatedFeature->lastResult();
        aRefAttr->setObject(anObject);
      }
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return aFilledAttributeName;
}

void SketchPlugin_MacroEllipse::constraintsForEllipse(FeaturePtr theEllipseFeature)
{
  // Create constraints.
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, CENTER_POINT_REF_ID(),
      theEllipseFeature->attribute(SketchPlugin_Ellipse::CENTER_ID()),
      ObjectPtr(), false);
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, MAJOR_AXIS_POINT_REF_ID(), AttributePtr(),
      theEllipseFeature->lastResult(), true);
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, PASSED_POINT_REF_ID(), AttributePtr(),
      theEllipseFeature->lastResult(), true);
}

FeaturePtr SketchPlugin_MacroEllipse::createEllipseFeature()
{
  FeaturePtr aEllipseFeature = sketch()->addFeature(SketchPlugin_Ellipse::ID());

  AttributePoint2DPtr aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aEllipseFeature->attribute(SketchPlugin_Ellipse::CENTER_ID()));
  aCenterAttr->setValue(myCenter->x(), myCenter->y());

  AttributePoint2DPtr aFocusAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aEllipseFeature->attribute(SketchPlugin_Ellipse::FOCUS_ID()));
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
  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(myCenter->x(), myCenter->y()));
  std::shared_ptr<GeomAPI_Pnt> aFocus(aSketch->to3D(myFocus->x(), myFocus->y()));
  std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  std::shared_ptr<GeomAPI_Dir> aMajorAxis(new GeomAPI_Dir(aFocus->x() - aCenter->x(),
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
