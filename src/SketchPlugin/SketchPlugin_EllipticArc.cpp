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

#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_Sketch.h>

#include <GeomAlgoAPI_EdgeBuilder.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_Ellipse2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <cmath>

static const double tolerance = 1e-7;
static const double paramTolerance = 1.e-4;
static const double PI = 3.141592653589793238463;


SketchPlugin_EllipticArc::SketchPlugin_EllipticArc()
  : SketchPlugin_SketchEntity(),
    myParamDelta(0.0)
{
}

void SketchPlugin_EllipticArc::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(FIRST_FOCUS_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SECOND_FOCUS_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(MAJOR_AXIS_START_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(MAJOR_AXIS_END_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(MINOR_AXIS_START_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(MINOR_AXIS_END_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(MAJOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(MINOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(START_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_POINT_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SECOND_FOCUS_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), MAJOR_AXIS_START_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), MAJOR_AXIS_END_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), MINOR_AXIS_START_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), MINOR_AXIS_END_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), MAJOR_RADIUS_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), MINOR_RADIUS_ID());

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_EllipticArc::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return;
  }

  // Calculate all characteristics of the ellipse.
  fillCharacteristicPoints();

  // Make a visible ellipse.
  createEllipticArc(aSketch);
}

bool SketchPlugin_EllipticArc::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_EllipticArc::attributeChanged(const std::string& theID) {
  // the second condition for unability to move external segments anywhere
  if (theID == EXTERNAL_ID() || isFixed()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
    if (!aSelection) {
      // empty shape in selection shows that the shape is equal to context
      ResultPtr anExtRes = selection(EXTERNAL_ID())->context();
      if (anExtRes)
        aSelection = anExtRes->shape();
    }
    // update arguments due to the selection value
    if (aSelection && !aSelection->isNull() && aSelection->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aSelection));
      std::shared_ptr<GeomAPI_Ellipse> anEllipse = anEdge->ellipse();

      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_ID()));
      aCenterAttr->setValue(sketch()->to2D(anEllipse->center()));

      std::shared_ptr<GeomDataAPI_Point2D> aFocusAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(FIRST_FOCUS_ID()));
      aFocusAttr->setValue(sketch()->to2D(anEllipse->firstFocus()));

      std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_POINT_ID()));
      aStartAttr->setValue(sketch()->to2D(anEdge->firstPoint()));

      std::shared_ptr<GeomDataAPI_Point2D> aEndAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_ID()));
      aEndAttr->setValue(sketch()->to2D(anEdge->lastPoint()));

      real(MAJOR_RADIUS_ID())->setValue(anEllipse->majorRadius());
      real(MINOR_RADIUS_ID())->setValue(anEllipse->minorRadius());
    }
  }
  else if (theID == CENTER_ID() || theID == FIRST_FOCUS_ID() ||
           theID == START_POINT_ID() || theID == END_POINT_ID())
    fillCharacteristicPoints();
}

static void calculateRadii(const GeomPnt2dPtr& theCenter,
                           const GeomPnt2dPtr& theFocus,
                           const GeomPnt2dPtr& thePassed,
                           double& theMajorRadius,
                           double& theMinorRadius)
{
  GeomPnt2dPtr aSecondFocus(new GeomAPI_Pnt2d(
      theCenter->xy()->multiplied(2.0)->decreased(theFocus->xy())));
  theMajorRadius = 0.5 * (thePassed->distance(theFocus) + thePassed->distance(aSecondFocus));

  GeomDir2dPtr aXAxis(new GeomAPI_Dir2d(theFocus->x() - theCenter->x(),
                                        theFocus->y() - theCenter->y()));
  std::shared_ptr<GeomAPI_XY> aPassedVec = thePassed->xy()->decreased(theCenter->xy());

  double x = aPassedVec->dot(aXAxis->xy()) / theMajorRadius;
  double y = abs(aPassedVec->cross(aXAxis->xy()));
  theMinorRadius = y / sqrt(1.0 - x * x);
}

bool SketchPlugin_EllipticArc::fillCharacteristicPoints()
{
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aFocusAttr =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(FIRST_FOCUS_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartPointAttr =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START_POINT_ID()));

  if (!aCenterAttr->isInitialized() ||
    !aFocusAttr->isInitialized() ||
    !aStartPointAttr->isInitialized()) {
    return false;
  }

  data()->blockSendAttributeUpdated(true);
  GeomPnt2dPtr aCenter2d = aCenterAttr->pnt();
  GeomPnt2dPtr aFocus2d = aFocusAttr->pnt();
  GeomPnt2dPtr aStart2d = aStartPointAttr->pnt();

  double aMajorRadius = 0.0, aMinorRadius = 0.0;
  calculateRadii(aCenter2d, aFocus2d, aStart2d, aMajorRadius, aMinorRadius);
  if (aMinorRadius < tolerance)
    return false;
  real(MAJOR_RADIUS_ID())->setValue(aMajorRadius);
  real(MINOR_RADIUS_ID())->setValue(aMinorRadius);

  GeomDir2dPtr aMajorDir2d(new GeomAPI_Dir2d(aFocus2d->x() - aCenter2d->x(),
    aFocus2d->y() - aCenter2d->y()));
  GeomDir2dPtr aMinorDir2d(new GeomAPI_Dir2d(-aMajorDir2d->y(), aMajorDir2d->x()));

  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(SECOND_FOCUS_ID()))
    ->setValue(2.0 * aCenter2d->x() - aFocus2d->x(), 2.0 * aCenter2d->y() - aFocus2d->y());
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(MAJOR_AXIS_START_ID()))
    ->setValue(aCenter2d->x() - aMajorDir2d->x() * aMajorRadius,
      aCenter2d->y() - aMajorDir2d->y() * aMajorRadius);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(MAJOR_AXIS_END_ID()))
    ->setValue(aCenter2d->x() + aMajorDir2d->x() * aMajorRadius,
      aCenter2d->y() + aMajorDir2d->y() * aMajorRadius);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(MINOR_AXIS_START_ID()))
    ->setValue(aCenter2d->x() - aMinorDir2d->x() * aMinorRadius,
      aCenter2d->y() - aMinorDir2d->y() * aMinorRadius);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(MINOR_AXIS_END_ID()))
    ->setValue(aCenter2d->x() + aMinorDir2d->x() * aMinorRadius,
      aCenter2d->y() + aMinorDir2d->y() * aMinorRadius);

  std::shared_ptr<GeomDataAPI_Point2D> aEndPointAttr =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END_POINT_ID()));
  if (aEndPointAttr->isInitialized()) {
    // recalculate REVERSED flag
    std::shared_ptr<GeomAPI_Ellipse2d> anEllipseForArc(
        new GeomAPI_Ellipse2d(aCenter2d, aMajorDir2d, aMajorRadius, aMinorRadius));
    GeomPnt2dPtr anEnd = aEndPointAttr->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aProjection = anEllipseForArc->project(anEnd);
    double aParamStart = 0.0, aParamEnd = 0.0;
    if (aProjection && anEnd->distance(aProjection) <= tolerance &&
        anEllipseForArc->parameter(anEnd, paramTolerance, aParamEnd)) {
      // do not recalculate REVERSED flag if the arc is not consistent
      anEllipseForArc->parameter(aStart2d, paramTolerance, aParamStart);
      aParamEnd -= aParamStart;

      if (myParamDelta >= 0.0 && myParamDelta <= PI * 0.5 &&
          aParamEnd < 0 && aParamEnd >= -PI * 0.5) {
        boolean(REVERSED_ID())->setValue(true);
      }
      else if (myParamDelta <= 0.0 && myParamDelta >= -PI * 0.5 &&
               aParamEnd > 0.0 && aParamEnd <= PI * 0.5) {
        boolean(REVERSED_ID())->setValue(false);
      }
    }
    myParamDelta = aParamEnd;
  }
  data()->blockSendAttributeUpdated(false);

  return true;
}

void SketchPlugin_EllipticArc::createEllipticArc(SketchPlugin_Sketch* theSketch)
{
  // Compute a ellipse in 3D view.
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aFocusAttr =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(FIRST_FOCUS_ID()));

  double aMajorRadius = real(MAJOR_RADIUS_ID())->value();
  double aMinorRadius = real(MINOR_RADIUS_ID())->value();

  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    theSketch->attribute(SketchPlugin_Sketch::NORM_ID()));

  GeomPointPtr aCenter(theSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
  GeomPointPtr aFocus(theSketch->to3D(aFocusAttr->x(), aFocusAttr->y()));
  GeomDirPtr aNormal = aNDir->dir();
  std::shared_ptr<GeomAPI_Shape> anEllipseShape;
  if (aFocus->distance(aCenter) > tolerance) {
    GeomDirPtr aMajorAxis(new GeomAPI_Dir(aFocus->x() - aCenter->x(),
        aFocus->y() - aCenter->y(), aFocus->z() - aCenter->z()));

    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START_POINT_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END_POINT_ID()));

    GeomPointPtr aStartPnt(theSketch->to3D(aStartAttr->x(), aStartAttr->y()));
    GeomPointPtr aEndPnt(theSketch->to3D(aEndAttr->x(), aEndAttr->y()));
    if (boolean(REVERSED_ID())->value())
      std::swap(aStartPnt, aEndPnt);

    anEllipseShape = GeomAlgoAPI_EdgeBuilder::ellipticArc(aCenter, aNormal, aMajorAxis,
        aMajorRadius, aMinorRadius, aStartPnt, aEndPnt);
  }
  else {
    // build circle instead of ellipse
    anEllipseShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aMajorRadius);
  }

  ResultConstructionPtr aResult = document()->createConstruction(data());
  aResult->setShape(anEllipseShape);
  aResult->setIsInHistory(false);
  setResult(aResult);
}
