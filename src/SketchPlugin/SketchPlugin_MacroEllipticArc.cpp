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

#include <SketchPlugin_MacroEllipticArc.h>

#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_MacroArcReentrantMessage.h>
#include <SketchPlugin_Tools.h>
#include <SketchPlugin_Sketch.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Events.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_Ellipse2d.h>
#include <GeomAPI_Vertex.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>


const double TOLERANCE = 1.e-7;
const double paramTolerance = 1.e-4;
const double PI = 3.141592653589793238463;


SketchPlugin_MacroEllipticArc::SketchPlugin_MacroEllipticArc()
  : SketchPlugin_SketchEntity(),
    myMajorRadius(0.0),
    myMinorRadius(0.0),
    myParamDelta(0.0)
{
}

void SketchPlugin_MacroEllipticArc::initAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(CENTER_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(MAJOR_AXIS_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(MAJOR_AXIS_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(START_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(START_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(END_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(MAJOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(MINOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  boolean(REVERSED_ID())->setValue(false);

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CENTER_REF_ID());
  ModelAPI_Session::get()->validators()
      ->registerNotObligatory(getKind(), MAJOR_AXIS_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), START_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), END_POINT_REF_ID());
}

void SketchPlugin_MacroEllipticArc::execute()
{
  FeaturePtr anEllipse = createEllipticArcFeature();
  constraintsForEllipticArc(anEllipse);

  // message to init reentrant operation
  static Events_ID anId = SketchPlugin_MacroArcReentrantMessage::eventId();
  std::shared_ptr<SketchPlugin_MacroArcReentrantMessage> aMessage = std::shared_ptr
    <SketchPlugin_MacroArcReentrantMessage>(new SketchPlugin_MacroArcReentrantMessage(anId, this));
  aMessage->setCreatedFeature(anEllipse);
  Events_Loop::loop()->send(aMessage);
}

void SketchPlugin_MacroEllipticArc::attributeChanged(const std::string& theID)
{
  static const int NB_POINTS = 4;
  std::string aPointAttrName[NB_POINTS] = { CENTER_ID(),
                                            MAJOR_AXIS_POINT_ID(),
                                            START_POINT_ID(),
                                            END_POINT_ID() };
  std::string aPointRefName[NB_POINTS] = { CENTER_REF_ID(),
                                           MAJOR_AXIS_POINT_REF_ID(),
                                           START_POINT_REF_ID(),
                                           END_POINT_REF_ID() };

  int aNbInitialized = 0;
  GeomPnt2dPtr anEllipsePoints[NB_POINTS];

  for (int aPntIndex = 0; aPntIndex < NB_POINTS; ++aPntIndex) {
    AttributePtr aPointAttr = attribute(aPointAttrName[aPntIndex]);
    if (!aPointAttr->isInitialized())
      continue;

    AttributeRefAttrPtr aPointRef = refattr(aPointRefName[aPntIndex]);
    // calculate ellipse parameters
    GeomPnt2dPtr aPassedPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aPointAttr)->pnt();
    GeomShapePtr aTangentCurve;
    SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
      aPointRef, aPointAttr, aTangentCurve, aPassedPoint);

    anEllipsePoints[aNbInitialized++] = aPassedPoint;
  }

  if (aNbInitialized <= 1)
    return; // too few points for the ellipse

  myCenter    = anEllipsePoints[0];
  myMajorAxis = anEllipsePoints[1];
  myStartPnt  = anEllipsePoints[2];
  myEndPnt    = anEllipsePoints[3];

  if (myCenter->distance(myMajorAxis) < TOLERANCE)
    return; // ellipse is not valid

  std::shared_ptr<GeomAPI_Ellipse2d> anEllipse;
  if (aNbInitialized == 2) {
    GeomDir2dPtr aXDir(new GeomAPI_Dir2d(anEllipsePoints[1]->x() - anEllipsePoints[0]->x(),
                                         anEllipsePoints[1]->y() - anEllipsePoints[0]->y()));
    double aMajorRad = anEllipsePoints[1]->distance(anEllipsePoints[0]);
    anEllipse = std::shared_ptr<GeomAPI_Ellipse2d>(
        new GeomAPI_Ellipse2d(anEllipsePoints[0], aXDir, aMajorRad, 0.5 * aMajorRad));
  }
  else if (aNbInitialized >= 3) {
    anEllipse = std::shared_ptr<GeomAPI_Ellipse2d>(
      new GeomAPI_Ellipse2d(anEllipsePoints[0], anEllipsePoints[1], anEllipsePoints[2]));
  }

  if (!anEllipse || anEllipse->implPtr<void>() == 0)
    return;

  myMajorRadius = anEllipse->majorRadius();
  myMinorRadius = anEllipse->minorRadius();

  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  real(MAJOR_RADIUS_ID())->setValue(myMajorRadius);
  real(MINOR_RADIUS_ID())->setValue(myMinorRadius);
  data()->blockSendAttributeUpdated(aWasBlocked, false);

  // update the REVERSED flag
  if (myEndPnt) {
    double aParameterEnd = 0.0;
    GeomPnt2dPtr aEnd = anEllipse->project(myEndPnt);
    if (anEllipse->parameter(aEnd, paramTolerance, aParameterEnd)) {
      double aParamStart = 0.0;
      anEllipse->parameter(myStartPnt, paramTolerance, aParamStart);
      aParameterEnd -= aParamStart;

      if (myParamDelta > 0.0 && myParamDelta <= PI * 0.5 &&
          aParameterEnd < 0 && aParameterEnd >= -PI * 0.5) {
        boolean(REVERSED_ID())->setValue(true);
      }
      else if (myParamDelta < 0.0 && myParamDelta >= -PI * 0.5 &&
               aParameterEnd > 0.0 && aParameterEnd <= PI * 0.5) {
        boolean(REVERSED_ID())->setValue(false);
      }
    }
    myParamDelta = aParameterEnd;
  }
}

// LCOV_EXCL_START
std::string SketchPlugin_MacroEllipticArc::processEvent(
                                              const std::shared_ptr<Events_Message>& theMessage)
{
  std::string aFilledAttributeName;

  std::shared_ptr<SketchPlugin_MacroArcReentrantMessage> aReentrantMessage =
      std::dynamic_pointer_cast<SketchPlugin_MacroArcReentrantMessage>(theMessage);
  if (aReentrantMessage) {
    FeaturePtr aCreatedFeature = aReentrantMessage->createdFeature();

    ObjectPtr anObject = aReentrantMessage->selectedObject();
    AttributePtr anAttribute = aReentrantMessage->selectedAttribute();
    std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = aReentrantMessage->clickedPoint();

    if (aClickedPoint && (anObject || anAttribute)) {
      aFilledAttributeName = CENTER_ID();
      std::string aReferenceAttributeName = CENTER_REF_ID();

      // fill 2d point attribute
      AttributePoint2DPtr aPointAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(aFilledAttributeName));
      aPointAttr->setValue(aClickedPoint);

      // fill reference attribute
      AttributeRefAttrPtr aRefAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(attribute(aReferenceAttributeName));
      if (anAttribute) {
        if (!anAttribute->owner() || !anAttribute->owner()->data()->isValid()) {
          if (aCreatedFeature && anAttribute->id() == CENTER_ID())
            anAttribute = aCreatedFeature->attribute(SketchPlugin_EllipticArc::CENTER_ID());
        }
        aRefAttr->setAttr(anAttribute);
      }
      else if (anObject.get()) {
        // if attribute is NULL, only object is defined, it should be processed outside
        // the feature because it might be an external feature, that will be
        // removed/created again after restart operation
        // #2468 - Crash when sketching circles successively on a repetition
        aFilledAttributeName = "";
      }
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return aFilledAttributeName;
}
// LCOV_EXCL_STOP

FeaturePtr SketchPlugin_MacroEllipticArc::createEllipticArcFeature()
{
  GeomShapePtr anArc = getArcShape();
  GeomEllipsePtr anEllipse;
  GeomPointPtr aStartPoint, aEndPoint;
  if (anArc->isEdge()) {
    GeomEdgePtr anArcEdge = anArc->edge();
    aStartPoint = anArcEdge->firstPoint();
    aEndPoint = anArcEdge->lastPoint();
    if (boolean(REVERSED_ID())->value())
      std::swap(aStartPoint, aEndPoint);

    if (anArcEdge->isEllipse())
      anEllipse = anArcEdge->ellipse();
  }

  if (!anEllipse)
    return FeaturePtr();

  // Create and fill new EllipticArc feature
  SketchPlugin_Sketch* aSketch = sketch();
  FeaturePtr aEllipseFeature = aSketch->addFeature(SketchPlugin_EllipticArc::ID());

  AttributePoint2DPtr aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aEllipseFeature->attribute(SketchPlugin_EllipticArc::CENTER_ID()));
  aCenterAttr->setValue(aSketch->to2D(anEllipse->center()));

  AttributePoint2DPtr aFocusAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aEllipseFeature->attribute(SketchPlugin_EllipticArc::FIRST_FOCUS_ID()));
  aFocusAttr->setValue(aSketch->to2D(anEllipse->firstFocus()));

  AttributePoint2DPtr aStartAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aEllipseFeature->attribute(SketchPlugin_EllipticArc::START_POINT_ID()));
  aStartAttr->setValue(aSketch->to2D(aStartPoint));

  AttributePoint2DPtr aEndAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aEllipseFeature->attribute(SketchPlugin_EllipticArc::END_POINT_ID()));
  aEndAttr->setValue(aSketch->to2D(aEndPoint));

  aEllipseFeature->real(SketchPlugin_EllipticArc::MAJOR_RADIUS_ID())->setValue(myMajorRadius);
  aEllipseFeature->real(SketchPlugin_EllipticArc::MINOR_RADIUS_ID())->setValue(myMinorRadius);

  aEllipseFeature->boolean(SketchPlugin_EllipticArc::REVERSED_ID())->setValue(
      boolean(REVERSED_ID())->value());

  aEllipseFeature->boolean(SketchPlugin_EllipticArc::AUXILIARY_ID())->setValue(
      boolean(AUXILIARY_ID())->value());

  aEllipseFeature->execute();

  // create auxiliary points
  SketchPlugin_Tools::createAuxiliaryPointOnEllipse(
      aEllipseFeature, SketchPlugin_EllipticArc::CENTER_ID());
  SketchPlugin_Tools::createAuxiliaryPointOnEllipse(
      aEllipseFeature, SketchPlugin_EllipticArc::FIRST_FOCUS_ID());
  SketchPlugin_Tools::createAuxiliaryPointOnEllipse(
      aEllipseFeature, SketchPlugin_EllipticArc::SECOND_FOCUS_ID());
  SketchPlugin_Tools::createAuxiliaryPointOnEllipse(
      aEllipseFeature, SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID());
  SketchPlugin_Tools::createAuxiliaryPointOnEllipse(
      aEllipseFeature, SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID());
  SketchPlugin_Tools::createAuxiliaryPointOnEllipse(
      aEllipseFeature, SketchPlugin_EllipticArc::MINOR_AXIS_START_ID());
  SketchPlugin_Tools::createAuxiliaryPointOnEllipse(
      aEllipseFeature, SketchPlugin_EllipticArc::MINOR_AXIS_END_ID());
  // create auxiliary axes
  SketchPlugin_Tools::createAuxiliaryAxisOfEllipse(aEllipseFeature,
                      SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID(),
                      SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID());
  SketchPlugin_Tools::createAuxiliaryAxisOfEllipse(aEllipseFeature,
                      SketchPlugin_EllipticArc::MINOR_AXIS_START_ID(),
                      SketchPlugin_EllipticArc::MINOR_AXIS_END_ID());

  return aEllipseFeature;
}

void SketchPlugin_MacroEllipticArc::constraintsForEllipticArc(FeaturePtr theEllipticArc)
{
  // tangency on-the-fly is not applicable for elliptic arcs
  static const bool isTangencyApplicable = false;
  // Create constraints.
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, CENTER_REF_ID(),
      theEllipticArc->attribute(SketchPlugin_EllipticArc::CENTER_ID()),
      ObjectPtr(), isTangencyApplicable);
  // make coincidence only if PASSED_POINT_REF_ID() refers a point but not an object
  if (!refattr(MAJOR_AXIS_POINT_REF_ID())->isObject()) {
    SketchPlugin_Tools::createCoincidenceOrTangency(
        this, MAJOR_AXIS_POINT_REF_ID(),
        AttributePtr(),
        theEllipticArc->lastResult(), isTangencyApplicable);
  }
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, START_POINT_REF_ID(),
      theEllipticArc->attribute(SketchPlugin_EllipticArc::START_POINT_ID()),
      ObjectPtr(), isTangencyApplicable);
  SketchPlugin_Tools::createCoincidenceOrTangency(
      this, END_POINT_REF_ID(),
      theEllipticArc->attribute(SketchPlugin_EllipticArc::END_POINT_ID()),
      ObjectPtr(), isTangencyApplicable);
}

AISObjectPtr SketchPlugin_MacroEllipticArc::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch || !myCenter || !myMajorAxis)
    return AISObjectPtr();

  // Compute a elliptic arc in 3D view.
  GeomPointPtr aCenter(aSketch->to3D(myCenter->x(), myCenter->y()));
  GeomShapePtr aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
  GeomShapePtr anArcShape = getArcShape();
  if (!anArcShape.get() || !aCenterPointShape.get())
    return AISObjectPtr();

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  aShapes.push_back(anArcShape);
  aShapes.push_back(aCenterPointShape);

  std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS.reset(new GeomAPI_AISObject());
  anAIS->createShape(aCompound);
  SketchPlugin_Tools::customizeFeaturePrs(anAIS, boolean(AUXILIARY_ID())->value());
  return anAIS;
}

GeomShapePtr SketchPlugin_MacroEllipticArc::getArcShape()
{
  if (!myCenter.get() || !myMajorAxis.get())
    return GeomShapePtr();

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch || myCenter->distance(myMajorAxis) < 1.e-7)
    return GeomShapePtr();

  GeomPointPtr aCenter(aSketch->to3D(myCenter->x(), myCenter->y()));
  GeomPointPtr aMajorAxisPnt(aSketch->to3D(myMajorAxis->x(), myMajorAxis->y()));
  GeomDirPtr aMajorAxisDir(new GeomAPI_Dir(aMajorAxisPnt->x() - aCenter->x(),
                                           aMajorAxisPnt->y() - aCenter->y(),
                                           aMajorAxisPnt->z() - aCenter->z()));

  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aSketch->attribute(SketchPlugin_Sketch::NORM_ID()));
  GeomDirPtr aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

  GeomPointPtr aStart, anEnd;
  if (myStartPnt)
    aStart = aSketch->to3D(myStartPnt->x(), myStartPnt->y());
  if (myEndPnt)
    anEnd = aSketch->to3D(myEndPnt->x(), myEndPnt->y());

  GeomShapePtr anArcShape;
  if (anEnd) {
    if (boolean(REVERSED_ID())->value())
      std::swap(aStart, anEnd);

    anArcShape = GeomAlgoAPI_EdgeBuilder::ellipticArc(aCenter, aNormal, aMajorAxisDir,
        myMajorRadius, myMinorRadius, aStart, anEnd);
  }
  else {
    anArcShape = GeomAlgoAPI_EdgeBuilder::ellipse(aCenter, aNormal, aMajorAxisDir,
        myMajorRadius, myMinorRadius);
  }

  return anArcShape;
}
