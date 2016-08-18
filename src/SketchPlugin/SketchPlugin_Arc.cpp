// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Arc.cpp
// Created:     26 Apr 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Sketch.h"
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintTangent.h>

#include <Events_Loop.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Ax2.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
// for sqrt on Linux
#include <math.h>

const double tolerance = 1e-7;
const double paramTolerance = 1.e-4;
const double PI =3.141592653589793238463;

namespace {
  static const std::string& POINT_ID(int theIndex)
  {
    switch (theIndex) {
    case 1: return SketchPlugin_Arc::START_ID();
    case 2: return SketchPlugin_Arc::END_ID();
    case 3: return SketchPlugin_Arc::PASSED_POINT_ID();
    }

    static const std::string DUMMY;
    return DUMMY;
  }
}



SketchPlugin_Arc::SketchPlugin_Arc()
    : SketchPlugin_SketchEntity()
{
  myStartUpdate = false;
  myEndUpdate = false;
  // default values
  myXEndBefore = 0;
  myYEndBefore = 0;

  myParamBefore = 0;
}

void SketchPlugin_Arc::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(START_ID(), GeomDataAPI_Point2D::typeId());
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
    GeomDataAPI_Point2D>(data()->addAttribute(END_ID(), GeomDataAPI_Point2D::typeId()));
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());

  AttributeBooleanPtr isInversed = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
    data()->addAttribute(INVERSED_ID(), ModelAPI_AttributeBoolean::typeId()));

  // get the initial values
  if (anEndAttr->isInitialized()) {
    myXEndBefore = anEndAttr->x();
    myYEndBefore = anEndAttr->y();
  }

  AttributeStringPtr anArcType = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
    data()->addAttribute(ARC_TYPE(), ModelAPI_AttributeString::typeId()));

  data()->addAttribute(PASSED_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(TANGENT_POINT_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  // set after all to avoid in attributeChanged reference to not existing attributes
  if (!isInversed->isInitialized())
    isInversed->setValue(false);
  anArcType->setValue(ARC_TYPE_CENTER_START_END());
}

void SketchPlugin_Arc::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  // result for the arc is set only when all obligatory attributes are initialized,
  // otherwise AIS object is used to visualize the arc's preview
  if (aSketch && isFeatureValid()) {
    bool hasResult = lastResult().get() != NULL;

    // compute a circle point in 3D view
    std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
    // compute the arc start point
    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(START_ID()));

    std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
    // make a visible point
    std::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
    std::shared_ptr<ModelAPI_ResultConstruction> aConstr1 = document()->createConstruction(
        data(), 0);
    aConstr1->setShape(aCenterPointShape);
    aConstr1->setIsInHistory(false);
    setResult(aConstr1, 0);

    // make a visible circle
    std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
    std::shared_ptr<GeomAPI_Pnt> aStartPoint(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));

    // compute and change the arc end point
    std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(END_ID()));
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
        new GeomAPI_Circ2d(aCenterAttr->pnt(), aStartAttr->pnt()));
    std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEndAttr->pnt());
    if (aProjection && anEndAttr->pnt()->distance(aProjection) > tolerance)
      anEndAttr->setValue(aProjection);
    std::shared_ptr<GeomAPI_Pnt> aEndPoint(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));
    AttributeBooleanPtr isInversed =
        std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attribute(INVERSED_ID()));

    std::shared_ptr<GeomAPI_Dir> anXDir(new GeomAPI_Dir(aStartPoint->xyz()->decreased(aCenter->xyz())));
    std::shared_ptr<GeomAPI_Ax2> anAx2(new GeomAPI_Ax2(aCenter, aNormal, anXDir));
    std::shared_ptr<GeomAPI_Circ> aCirc(new GeomAPI_Circ(anAx2, aCenter->distance(aStartPoint)));
    double aParameterNew = 0.0;
    if(aCirc->parameter(aEndPoint, paramTolerance, aParameterNew)) {
      if(0 <= myParamBefore && myParamBefore <= PI / 2.0
        && PI * 1.5 <= aParameterNew && aParameterNew <= PI * 2.0) {
          isInversed->setValue(true);
      } else if(PI * 1.5 <= myParamBefore && myParamBefore <= PI * 2.0
        && 0 <= aParameterNew && aParameterNew <= PI / 2.0) {
          isInversed->setValue(false);
      }
    }
    myParamBefore = aParameterNew;

    std::shared_ptr<GeomAPI_Shape> aCircleShape;
    if(!isInversed->value()) {
      aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStartPoint, aEndPoint, aNormal);
    } else {
      aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aEndPoint, aStartPoint, aNormal);
    }

    if (aCircleShape) {
      std::shared_ptr<ModelAPI_ResultConstruction> aConstr2 = document()->createConstruction(
          data(), 1);
      aConstr2->setShape(aCircleShape);
      aConstr2->setIsInHistory(false);
      setResult(aConstr2, 1);
    }
  }
}

AISObjectPtr SketchPlugin_Arc::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // if the feature is valid, the execute() method should be performed, AIS object is empty
    if (!isFeatureValid()) {
      // compute a circle point in 3D view
      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
          GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));

      std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
      if (aCenterAttr->isInitialized()) {
        std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));

        std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
            GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::START_ID()));
        std::shared_ptr<GeomDataAPI_Point2D> aEndAttr = std::dynamic_pointer_cast<
            GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::END_ID()));
        AttributeStringPtr aTypeAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
            data()->attribute(ARC_TYPE()));

        if (aStartAttr->isInitialized()) {
          // make a visible circle
          std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
              aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
          bool aHasPlane = aNDir && !(aNDir->x() == 0 && aNDir->y() == 0 && aNDir->z() == 0);
          if (aHasPlane) {
            std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
            std::shared_ptr<GeomAPI_Pnt> aStartPoint(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));
            std::shared_ptr<GeomAPI_Pnt> aEndPoint = aStartPoint;
            if (aTypeAttr && aTypeAttr->isInitialized() &&
                aTypeAttr->value() == ARC_TYPE_THREE_POINTS() && aEndAttr->isInitialized() &&
                aEndAttr->pnt()->distance(aStartAttr->pnt()) > tolerance) {
              aEndPoint = aSketch->to3D(aEndAttr->x(), aEndAttr->y());
              std::shared_ptr<GeomDataAPI_Point2D> aPassedAttr = 
                std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(PASSED_POINT_ID()));
              if (!aPassedAttr->isInitialized()) { // calculate the appropriate center for the presentation
                // check that center is bad for the current start and end and must be recomputed
                std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(new GeomAPI_Circ2d(
                  aCenterAttr->pnt(), aStartAttr->pnt()));
                std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(aEndAttr->pnt());
                if (!aProjection.get() || aEndAttr->pnt()->distance(aProjection) > tolerance) {
                  std::shared_ptr<GeomAPI_XY> aDir = 
                    aEndAttr->pnt()->xy()->decreased(aStartAttr->pnt()->xy())->multiplied(0.5);
                  double x = aDir->x();
                  double y = aDir->y();
                  aDir->setX(x - y);
                  aDir->setY(y + x);
                  std::shared_ptr<GeomAPI_XY> aCenterXY = aStartAttr->pnt()->xy()->added(aDir);
                  aCenter = aSketch->to3D(aCenterXY->x(), aCenterXY->y());
                }
              }
            }
            AttributeBooleanPtr isInversed =
              std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attribute(INVERSED_ID()));

            std::shared_ptr<GeomAPI_Shape> aCircleShape = 
              (isInversed->isInitialized() && isInversed->value()) ?
                GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aEndPoint, aStartPoint, aNormal) :
                GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStartPoint, aEndPoint, aNormal);

            if (aCircleShape)
              aShapes.push_back(aCircleShape);
          }
        }
        // make a visible point
        std::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
        aShapes.push_back(aCenterPointShape);
      }
      if (!aShapes.empty()) {
        std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
        AISObjectPtr anAIS = thePrevious;
        if (!anAIS)
          anAIS = AISObjectPtr(new GeomAPI_AISObject);
        anAIS->createShape(aCompound);
        anAIS->setWidth(3);
        return anAIS;
      }
    }
  }
  return AISObjectPtr();
}

void SketchPlugin_Arc::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  aData->blockSendAttributeUpdated(true);

  myStartUpdate = true;
  myEndUpdate = true;
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::START_ID()));
  aPoint2->move(theDeltaX, theDeltaY);

  std::shared_ptr<GeomDataAPI_Point2D> aPoint3 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::END_ID()));
  aPoint3->move(theDeltaX, theDeltaY);
  myStartUpdate = false;
  myEndUpdate = false;

  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::CENTER_ID()));
  aPoint1->move(theDeltaX, theDeltaY);

  std::shared_ptr<GeomDataAPI_Point2D> aPassedPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(PASSED_POINT_ID()));
  aPassedPoint->move(theDeltaX, theDeltaY);
  aData->blockSendAttributeUpdated(false);
}

bool SketchPlugin_Arc::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

bool SketchPlugin_Arc::isFeatureValid()
{
  AttributeStringPtr anArcTypeAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeString>(data()->attribute(ARC_TYPE()));
  if (!anArcTypeAttr)
    return false;
  std::string anArcType = anArcTypeAttr->value();

  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::END_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPassedAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(PASSED_POINT_ID()));

  bool isValid = false;
  if (anArcType == ARC_TYPE_THREE_POINTS())
    isValid = aStartAttr->isInitialized() && anEndAttr->isInitialized() && aPassedAttr->isInitialized();
  else
    isValid = aCenterAttr->isInitialized() && aStartAttr->isInitialized() && anEndAttr->isInitialized();

  return isValid;
}

static inline void adjustPeriod(double& theParam)
{
  static const double PERIOD = 2.0 * PI;
  while (theParam < 0.0) theParam += PERIOD;
  while (theParam >= PERIOD) theParam -= PERIOD;
}

void SketchPlugin_Arc::attributeChanged(const std::string& theID)
{
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(END_ID()));
  // the second condition for unability to move external segments anywhere
  if (theID == EXTERNAL_ID() || isFixed()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
    // update arguments due to the selection value
    if (aSelection && !aSelection->isNull() && aSelection->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge( new GeomAPI_Edge(aSelection));
      std::shared_ptr<GeomAPI_Circ> aCirc = anEdge->circle();
      if (aCirc.get()) {
        aStartAttr->setValue(sketch()->to2D(anEdge->firstPoint()));
        anEndAttr->setValue(sketch()->to2D(anEdge->lastPoint()));
        aCenterAttr->setValue(sketch()->to2D(aCirc->center()));

        data()->real(RADIUS_ID())->setValue(aCirc->radius());
        double aStartAngle, aEndAngle;
        anEdge->getRange(aStartAngle, aEndAngle);
        data()->real(ANGLE_ID())->setValue(aEndAngle - aStartAngle);
      }
    }
    return;
  }
  AttributeStringPtr aTypeAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeString>(attribute(ARC_TYPE()));

  // this is before others since here end attribute may be changed, but with the special behavior
  if (aTypeAttr->value() == ARC_TYPE_TANGENT() && (theID == TANGENT_POINT_ID() || theID == END_ID())) {
    SketchPlugin_Sketch* aSketch = sketch();
    AttributeRefAttrPtr aTangPtAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        data()->attribute(TANGENT_POINT_ID()));

    if (aTangPtAttr->isInitialized() && anEndAttr->isInitialized()) {
      // compute orthogonal direction
      std::shared_ptr<GeomAPI_Dir2d> anOrthoDir;
      std::shared_ptr<GeomDataAPI_Point2D> aTangentPoint =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aTangPtAttr->attr());
      std::shared_ptr<GeomAPI_Pnt2d> aTangPnt2d = aTangentPoint->pnt();
      if (aTangPnt2d->isEqual(anEndAttr->pnt()))
        return;
      FeaturePtr aTangFeature = ModelAPI_Feature::feature(aTangentPoint->owner());
      std::shared_ptr<GeomAPI_Edge> aTangEdge = std::dynamic_pointer_cast<GeomAPI_Edge>(
          aTangFeature->lastResult()->shape());
      if (aTangEdge->isLine()) {
        std::shared_ptr<GeomAPI_Dir> aDir = aTangEdge->line()->direction();
        std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(aDir->x(), aDir->y(), aDir->z()));
        std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = aSketch->to2D(aPnt);
        anOrthoDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(-aPnt2d->y(), aPnt2d->x()));
      }
      else if (aTangEdge->isArc()) {
        std::shared_ptr<GeomAPI_Pnt> aCenter = aTangEdge->circle()->center();
        std::shared_ptr<GeomAPI_Pnt2d> aCenter2d = aSketch->to2D(aCenter);
        anOrthoDir = std::shared_ptr<GeomAPI_Dir2d>(
            new GeomAPI_Dir2d(aTangPnt2d->xy()->decreased(aCenter2d->xy())));
      }

      // compute parameters of the middle perpendicular
      std::shared_ptr<GeomAPI_XY> aEndPntCoord = anEndAttr->pnt()->xy();
      std::shared_ptr<GeomAPI_XY> aTempDir = aEndPntCoord->decreased(aTangPnt2d->xy());
      std::shared_ptr<GeomAPI_Dir2d> aMidDir(new GeomAPI_Dir2d(-aTempDir->y(), aTempDir->x()));
      std::shared_ptr<GeomAPI_Pnt2d> aMidPnt(
          new GeomAPI_Pnt2d(aEndPntCoord->added(aTangPnt2d->xy())->multiplied(0.5)));

      // compute center of arc by calculating intersection of orthogonal line and middle perpendicular
      std::shared_ptr<GeomAPI_Lin2d> anOrthoLine(new GeomAPI_Lin2d(aTangPnt2d, anOrthoDir));
      std::shared_ptr<GeomAPI_Lin2d> aMiddleLine(new GeomAPI_Lin2d(aMidPnt, aMidDir));
      std::shared_ptr<GeomAPI_Pnt2d> aCenter = anOrthoLine->intersect(aMiddleLine);
      if (aCenter) {
        data()->blockSendAttributeUpdated(true);
        aCenterAttr->setValue(aCenter);
        aStartAttr->setValue(aTangPnt2d);
        data()->blockSendAttributeUpdated(false);
      }

      tangencyArcConstraints();
    }
    return;
  }

  // if changed the base attributes, update all other (is necessary) without recursion
  if (theID == CENTER_ID() || theID == START_ID() || theID == END_ID() || theID == ARC_TYPE()) {
    if (!isFeatureValid())
      return;
    std::shared_ptr<GeomAPI_Pnt2d> aCenter = aCenterAttr->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aStart = aStartAttr->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> anEnd = anEndAttr->pnt();
    double aRadius = aCenter->distance(aStart);
    if (aRadius < tolerance)
      return;
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(new GeomAPI_Circ2d(aCenter, aStart));

    data()->blockSendAttributeUpdated(true);
    if (theID == END_ID() && isStable()) {
      // The arc is under construction, so its end point projected
      // on the circle formed by center and start points
      std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEnd);
      if (aProjection && anEnd->distance(aProjection) > tolerance) {
        anEndAttr->setValue(aProjection);
        anEnd = aProjection;
      }
    }
    // update all other attributes due to the base attributes values
    if (aTypeAttr->value() == ARC_TYPE_THREE_POINTS()) { // update passed point due to start, end and center
      if (aCenter->distance(aStart) > tolerance && aCenter->distance(anEnd) > tolerance) {
        // project passed point t othe circle
        std::shared_ptr<GeomDataAPI_Point2D> aPassedAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(PASSED_POINT_ID()));
        if (aPassedAttr->isInitialized()) {
          std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(aPassedAttr->pnt());
          if (aProjection && aPassedAttr->pnt()->distance(aProjection) > tolerance) {
            aPassedAttr->setValue(aProjection);
          }
        } else { // initialize it by some middle - value
          std::shared_ptr<GeomAPI_Dir2d> aStartDir(new GeomAPI_Dir2d(
            aStart->xy()->decreased(aCenter->xy())));
          std::shared_ptr<GeomAPI_Dir2d> aEndDir(new GeomAPI_Dir2d(
            anEnd->xy()->decreased(aCenter->xy())));
          std::shared_ptr<GeomAPI_XY> aMidDirXY = aStartDir->xy()->added(aEndDir->xy());
          if (aMidDirXY->dot(aMidDirXY) < tolerance * tolerance) {
            // start and end directions are opposite, so middle direction will be orthogonal
            aMidDirXY->setX(-aStartDir->y());
            aMidDirXY->setY(aStartDir->x());
          }
          std::shared_ptr<GeomAPI_Dir2d> aMidDir(new GeomAPI_Dir2d(aMidDirXY));
          if ((aStartDir->cross(aMidDir) > 0) ^ !isReversed())
            aMidDir->reverse();
          std::shared_ptr<GeomAPI_XY> aPassedPnt = 
            aCenter->xy()->added(aMidDir->xy()->multiplied(aCenter->distance(aStart)));
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(PASSED_POINT_ID()))->
            setValue(aPassedPnt->x(), aPassedPnt->y());
        }
      }
    }
    // update radius and angle
    AttributeDoublePtr aRadiusAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        data()->attribute(RADIUS_ID()));
    aRadiusAttr->setValue(aRadius);
    AttributeDoublePtr anAngleAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        data()->attribute(ANGLE_ID()));
    std::shared_ptr<GeomAPI_Circ2d> aCircle(new GeomAPI_Circ2d(aCenter, aStart));
    double aStartParam, aEndParam;
    aCircle->parameter(aStart, paramTolerance, aStartParam);
    aCircle->parameter(anEnd, paramTolerance, aEndParam);
    adjustPeriod(aStartParam);
    adjustPeriod(aEndParam);
    if (aTypeAttr->value() == ARC_TYPE_THREE_POINTS()) { // use the passed point for the angle calculation
      std::shared_ptr<GeomDataAPI_Point2D> aPassedAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(PASSED_POINT_ID()));
      double aPassedParam;
      aCircle->parameter(aPassedAttr->pnt(), paramTolerance, aPassedParam);
      adjustPeriod(aPassedParam);
      double aNewAngle = aPassedParam >= aStartParam && aPassedParam <= aEndParam ?
        ((aEndParam - aStartParam) * 180.0 / PI) :
        ((aEndParam - aStartParam - 2.0 * PI) * 180.0 / PI);
      if (!anAngleAttr->isInitialized() || fabs(aNewAngle - anAngleAttr->value()) > tolerance)
        anAngleAttr->setValue(aNewAngle);
    } else {
      double aNewAngle = (aEndParam - aStartParam) * 180.0 / PI;
      if (!anAngleAttr->isInitialized() || fabs(aNewAngle - anAngleAttr->value()) > tolerance)
        anAngleAttr->setValue(aNewAngle);
    }
    // do not need to inform that other parameters were changed in this basis mode: these arguments
    // change is enough
    data()->blockSendAttributeUpdated(false, false);
    return;
  }

  if (theID == PASSED_POINT_ID()) {
    data()->blockSendAttributeUpdated(true);

    std::shared_ptr<GeomAPI_Pnt2d> aPoints[3];
    int aNbInitialized = 0;
    for (int i = 1; i <= 3; ++i) {
      std::shared_ptr<GeomDataAPI_Point2D> aCurPnt =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(POINT_ID(i)));
      if (aCurPnt->isInitialized())
        aPoints[aNbInitialized++] = aCurPnt->pnt();
    }

    if (aNbInitialized == 3) {
      std::shared_ptr<GeomAPI_Circ2d> aCircle(
          new GeomAPI_Circ2d(aPoints[0], aPoints[1], aPoints[2]));

      std::shared_ptr<GeomAPI_Pnt2d> aCenter = aCircle->center();
      if (aCenter) {
        aCenterAttr->setValue(aCenter);
      }
    }
    data()->blockSendAttributeUpdated(false);
    return;
  }

  if (theID == RADIUS_ID()) {
    if (!aStartAttr->isInitialized() || !anEndAttr->isInitialized() || !aCenterAttr->isInitialized())
      return;
    // move center and passed point
    std::shared_ptr<GeomAPI_Pnt2d> aStart = aStartAttr->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> anEnd = anEndAttr->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aCenter = aCenterAttr->pnt();
    if (aStart->distance(aCenter) < tolerance || anEnd->distance(aCenter) < tolerance)
      return;
    AttributeDoublePtr aRadiusAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        data()->attribute(RADIUS_ID()));
    double aRadius = aRadiusAttr->value();

    data()->blockSendAttributeUpdated(true);
    std::shared_ptr<GeomAPI_Dir2d> aStartDir(new GeomAPI_Dir2d(aStart->xy()->decreased(aCenter->xy())));
    std::shared_ptr<GeomAPI_XY> aNewStart = aStartDir->xy()->multiplied(aRadius)->added(aCenter->xy());
    aStartAttr->setValue(aNewStart->x(), aNewStart->y());
    std::shared_ptr<GeomAPI_Dir2d> anEndDir(new GeomAPI_Dir2d(anEnd->xy()->decreased(aCenter->xy())));
    std::shared_ptr<GeomAPI_XY> aNewEnd = anEndDir->xy()->multiplied(aRadius)->added(aCenter->xy());
    anEndAttr->setValue(aNewEnd->x(), aNewEnd->y());
    data()->blockSendAttributeUpdated(false);
    return;
  }
  if (theID == ANGLE_ID()) {
    if (!aStartAttr->isInitialized() || !aCenterAttr->isInitialized())
      return;
    AttributeDoublePtr anAngleAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        data()->attribute(ANGLE_ID()));
    data()->blockSendAttributeUpdated(true);
    // move end point and passed point
    std::shared_ptr<GeomAPI_XY> aCenter = aCenterAttr->pnt()->xy();
    double anAngle = anAngleAttr->value() * PI / 180.0;
    double sinA = sin(anAngle);
    double cosA = cos(anAngle);
    std::shared_ptr<GeomAPI_XY> aStartDir = aStartAttr->pnt()->xy()->decreased(aCenter);
    std::shared_ptr<GeomAPI_XY> aDir(new GeomAPI_XY(
        aStartDir->x() * cosA - aStartDir->y() * sinA,
        aStartDir->x() * sinA + aStartDir->y() * cosA));
    anEndAttr->setValue(aCenter->x() + aDir->x(), aCenter->y() + aDir->y());
    data()->blockSendAttributeUpdated(false);
    return;
  }
}

void SketchPlugin_Arc::setReversed(bool isReversed)
{
  std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attribute(INVERSED_ID()))->setValue(isReversed);
  myParamBefore = 0.0;
}

bool SketchPlugin_Arc::isReversed()
{
  return std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attribute(INVERSED_ID()))->value();
}

void SketchPlugin_Arc::tangencyArcConstraints()
{
  if (!lastResult())
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_ID()));
  AttributeRefAttrPtr aTangPtAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      attribute(TANGENT_POINT_ID()));
  if (!aTangPtAttr->attr())
    return;

  FeaturePtr aFeature = ModelAPI_Feature::feature(aStartAttr->owner());
  ObjectPtr aThisArc = aFeature->lastResult();
  aFeature = ModelAPI_Feature::feature(aTangPtAttr->attr()->owner());
  ObjectPtr aTangFeature = aFeature->lastResult();

  // trying to find constraints to fix the tangency of the arc
  std::set<FeaturePtr> aCoincidence;
  std::set<FeaturePtr> aTangency;

  AttributeRefAttrPtr aRefAttrA, aRefAttrB;
  std::set<AttributePtr> aRefs = data()->refsToMe();
  const std::set<AttributePtr>& aRefsToResult = lastResult()->data()->refsToMe();
  aRefs.insert(aRefsToResult.begin(), aRefsToResult.end());
  std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
  for (; aRefIt != aRefs.end(); ++aRefIt) {
    FeaturePtr aConstrFeature = ModelAPI_Feature::feature((*aRefIt)->owner());
    if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
      aRefAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
      aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrFeature->attribute(SketchPlugin_Constraint::ENTITY_B()));
      if ((aRefAttrA && aRefAttrA->attr() == aStartAttr) ||
          (aRefAttrB && aRefAttrB->attr() == aStartAttr))
        aCoincidence.insert(aConstrFeature);
    }
    else if (aConstrFeature->getKind() == SketchPlugin_ConstraintTangent::ID()) {
      aRefAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
      aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrFeature->attribute(SketchPlugin_Constraint::ENTITY_B()));
      if ((aRefAttrA && aRefAttrA->object() == aThisArc) ||
          (aRefAttrB && aRefAttrB->object() == aThisArc))
        aTangency.insert(aConstrFeature);
    }
  }
  // search applicable pair of constraints
  bool isFound = false;
  FeaturePtr aPrevCoincidence, aPrevTangency;
  std::set<FeaturePtr>::const_iterator aCIt, aTIt;
  for (aCIt = aCoincidence.begin(); aCIt != aCoincidence.end() && !isFound; ++aCIt) {
    aRefAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        (*aCIt)->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        (*aCIt)->attribute(SketchPlugin_Constraint::ENTITY_B()));
    AttributePtr anOtherPoint =
        aRefAttrA->attr() == aStartAttr ? aRefAttrB->attr() : aRefAttrA->attr();
    for (aTIt = aTangency.begin(); aTIt != aTangency.end() && !isFound; ++aTIt) {
      aRefAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          (*aTIt)->attribute(SketchPlugin_Constraint::ENTITY_A()));
      aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          (*aTIt)->attribute(SketchPlugin_Constraint::ENTITY_B()));
      ObjectPtr anOtherObject = aRefAttrA->object() == aThisArc ?
          aRefAttrB->object() : aRefAttrA->object();
      FeaturePtr anOtherFeature = ModelAPI_Feature::feature(anOtherObject);
      if (anOtherPoint->owner() == anOtherFeature) {
        isFound = true;
        aPrevCoincidence = *aCIt;
        aPrevTangency = *aTIt;
      }
    }
  }

  if (isFound) {
    // update previous constraints
    aRefAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aPrevCoincidence->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aPrevCoincidence->attribute(SketchPlugin_Constraint::ENTITY_B()));
    if (aRefAttrA->attr() == aStartAttr)
      aRefAttrB->setAttr(aTangPtAttr->attr());
    else
      aRefAttrA->setAttr(aTangPtAttr->attr());

    aRefAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aPrevTangency->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aPrevTangency->attribute(SketchPlugin_Constraint::ENTITY_B()));
    if (aRefAttrA->object() == aThisArc)
      aRefAttrB->setObject(aTangFeature);
    else
      aRefAttrA->setObject(aTangFeature);
  } else {
    // Wait all constraints being removed, then send update events
    static Events_ID aDeleteEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
    bool isDeleteFlushed = Events_Loop::loop()->isFlushed(aDeleteEvent);
    if (isDeleteFlushed)
      Events_Loop::loop()->setFlushed(aDeleteEvent, false);
    // Remove all obtained constraints which use current arc, because
    // there is no information which of them were used to build tangency arc.
    DocumentPtr aDoc = sketch()->document();
    std::set<FeaturePtr> aFeaturesToBeRemoved;
    for (aCIt = aCoincidence.begin(); aCIt != aCoincidence.end(); ++aCIt)
      aFeaturesToBeRemoved.insert(*aCIt);
    for (aTIt = aTangency.begin(); aTIt != aTangency.end(); ++aTIt)
      aFeaturesToBeRemoved.insert(*aTIt);
    ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);
    // Send events to update the sub-features by the solver.
    if (isDeleteFlushed)
      Events_Loop::loop()->setFlushed(aDeleteEvent, true);
    else
      Events_Loop::loop()->flush(aDeleteEvent);

    // Wait all constraints being created, then send update events
    static Events_ID aCreateEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
    bool isCreateFlushed = Events_Loop::loop()->isFlushed(aCreateEvent);
    if (isCreateFlushed)
      Events_Loop::loop()->setFlushed(aCreateEvent, false);

    // Create new constraints
    FeaturePtr aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
    aRefAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
    aRefAttrA->setAttr(aStartAttr);
    aRefAttrB->setAttr(aTangPtAttr->attr());
    aConstraint->execute();
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, aCreateEvent);

    aConstraint = sketch()->addFeature(SketchPlugin_ConstraintTangent::ID());
    aRefAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
    aRefAttrA->setObject(aThisArc);
    aRefAttrB->setObject(aTangFeature);
    aConstraint->execute();
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, aCreateEvent);

    // Send events to update the sub-features by the solver.
    if(isCreateFlushed)
      Events_Loop::loop()->setFlushed(aCreateEvent, true);
    else
      Events_Loop::loop()->flush(aCreateEvent);
  }
}
