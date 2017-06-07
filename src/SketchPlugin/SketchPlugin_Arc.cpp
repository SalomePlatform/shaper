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
#include <cmath>

const double tolerance = 1e-7;
const double paramTolerance = 1.e-4;
const double PI = 3.141592653589793238463;


SketchPlugin_Arc::SketchPlugin_Arc()
: SketchPlugin_SketchEntity()
{
  myParamBefore = 0.0;
}

void SketchPlugin_Arc::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(START_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());

  AttributeBooleanPtr isReversed = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
    data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId()));

  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  // set after all to avoid in attributeChanged reference to not existing attributes
  if (!isReversed->isInitialized()) {
    isReversed->setValue(false);
  }
}

void SketchPlugin_Arc::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return;
  }

  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END_ID()));
  if(!aCenterAttr->isInitialized() || !aStartAttr->isInitialized() || !anEndAttr->isInitialized()) {
    return;
  }

  // Make a visible point.
  SketchPlugin_Sketch::createPoint2DResult(this, sketch(), CENTER_ID(), 0);

  // Make a visible arc.
  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
  std::shared_ptr<GeomAPI_Pnt> aStart(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));
  std::shared_ptr<GeomAPI_Pnt> anEnd(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

  if (myParamBefore == 0) { // parameter has not been calculate yet
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
        new GeomAPI_Circ2d(aCenterAttr->pnt(), aStartAttr->pnt()));
    aCircleForArc->parameter(anEndAttr->pnt(), paramTolerance, myParamBefore);
  }

  GeomShapePtr anArcShape;
  if (fabs(myParamBefore - 2.0 * PI) < paramTolerance) {
    anArcShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aStart->distance(aCenter));
    myParamBefore = 0;
  } else {
    anArcShape = boolean(REVERSED_ID())->value() ?
      GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, anEnd, aStart, aNormal)
    : GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStart, anEnd, aNormal);
  }

  std::shared_ptr<ModelAPI_ResultConstruction> aResult = document()->createConstruction(data(), 1);
  aResult->setShape(anArcShape);
  aResult->setIsInHistory(false);
  setResult(aResult, 1);
}

void SketchPlugin_Arc::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if(!aData->isValid()) {
    return;
  }

  bool aWasBlocked = aData->blockSendAttributeUpdated(true);

  std::shared_ptr<GeomDataAPI_Point2D> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      attribute(CENTER_ID()));
  if(aCenter->isInitialized()) {
    aCenter->move(theDeltaX, theDeltaY);
  }

  std::shared_ptr<GeomDataAPI_Point2D> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      attribute(START_ID()));
  if(aStart->isInitialized()) {
    aStart->move(theDeltaX, theDeltaY);
  }

  std::shared_ptr<GeomDataAPI_Point2D> anEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      attribute(END_ID()));
  if(anEnd->isInitialized()) {
    anEnd->move(theDeltaX, theDeltaY);
  }

  aData->blockSendAttributeUpdated(aWasBlocked);
}

bool SketchPlugin_Arc::isFixed()
{
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_Arc::attributeChanged(const std::string& theID)
{
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(END_ID()));

  // The second condition for unability to move external segments anywhere.
  if(theID == EXTERNAL_ID() || isFixed()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
    if(!aSelection) {
      // empty shape in selection shows that the shape is equal to context
      ResultPtr anExtRes = selection(EXTERNAL_ID())->context();
      if(anExtRes) {
        aSelection = anExtRes->shape();
      }
    }
    // update arguments due to the selection value
    if(aSelection && !aSelection->isNull() && aSelection->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge( new GeomAPI_Edge(aSelection));
      std::shared_ptr<GeomAPI_Circ> aCirc = anEdge->circle();
      if(aCirc.get()) {
        bool aWasBlocked = data()->blockSendAttributeUpdated(true);
        aCenterAttr->setValue(sketch()->to2D(aCirc->center()));
        aStartAttr->setValue(sketch()->to2D(anEdge->firstPoint()));
        anEndAttr->setValue(sketch()->to2D(anEdge->lastPoint()));
        data()->blockSendAttributeUpdated(aWasBlocked, false);

        std::shared_ptr<GeomAPI_Circ2d> aCircle2d =
          std::shared_ptr<GeomAPI_Circ2d>(new GeomAPI_Circ2d(aCenterAttr->pnt(),
                                                             aStartAttr->pnt()));

        double anEndParam = 0.0;
        aCircle2d->parameter(anEndAttr->pnt(), paramTolerance, anEndParam);
        myParamBefore = anEndParam;

        double aMidParam  = anEndParam / 2.0;
        std::shared_ptr<GeomAPI_Pnt2d> aMidPnt2d;
        aCircle2d->D0(aMidParam, aMidPnt2d);
        std::shared_ptr<GeomAPI_Pnt> aMinPnt = sketch()->to3D(aMidPnt2d->x(), aMidPnt2d->y());
        double aStartParam = 0.0;
        aCirc->parameter(anEdge->firstPoint(), paramTolerance, aStartParam);
        aCirc->parameter(aMinPnt, paramTolerance, aMidParam);
        aCirc->parameter(anEdge->lastPoint(), paramTolerance, anEndParam);

        // adjust period
        anEndParam -= aStartParam;
        aMidParam -= aStartParam;
        if (anEndParam < 0.0)
          anEndParam += 2.0 * PI;
        if (aMidParam < 0.0)
          aMidParam += 2.0 * PI;

        aWasBlocked = data()->blockSendAttributeUpdated(true);
        if(aMidParam < anEndParam) {
          setReversed(false);
        } else {
          setReversed(true);
        }
        data()->blockSendAttributeUpdated(aWasBlocked, false);
      }
    }
  } else if(theID == CENTER_ID() || theID == START_ID() || theID == END_ID()) {
    if(!aCenterAttr->isInitialized()
      || !aStartAttr->isInitialized()
      || !anEndAttr->isInitialized()) {
      return;
    }
    std::shared_ptr<GeomAPI_Pnt2d> aCenter = aCenterAttr->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aStart = aStartAttr->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> anEnd = anEndAttr->pnt();
    double aRadius = aCenter->distance(aStart);
    if (aRadius < tolerance)
      return;
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(new GeomAPI_Circ2d(aCenter, aStart));

    // Do not recalculate REVERSED flag if the arc is not consistent
    std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEnd);
    if (aProjection && anEnd->distance(aProjection) <= tolerance) {
      double aParameterNew = 0.0;
      if(aCircleForArc->parameter(anEnd, paramTolerance, aParameterNew)) {
        bool aWasBlocked = data()->blockSendAttributeUpdated(true);
        if(myParamBefore <= PI / 2.0 && aParameterNew >= PI * 1.5) {
          if(!boolean(REVERSED_ID())->value()) {
            boolean(REVERSED_ID())->setValue(true);
          }
        } else if(myParamBefore >= PI * 1.5 && aParameterNew <= PI / 2.0) {
          if(boolean(REVERSED_ID())->value()) {
            boolean(REVERSED_ID())->setValue(false);
          }
        }
        data()->blockSendAttributeUpdated(aWasBlocked, false);
      }
      if (fabs(aParameterNew) < paramTolerance ||
          fabs(aParameterNew - 2.0 * PI) < paramTolerance)
        aParameterNew = 2.0 * PI;
      myParamBefore = aParameterNew;
    }
  }

  double aRadius = 0;
  double anAngle = 0;
  if(aCenterAttr->isInitialized() && aStartAttr->isInitialized()) {
    aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
    if(anEndAttr->isInitialized()) {
      if(aStartAttr->pnt()->isEqual(anEndAttr->pnt())) {
        anAngle = 360;
      } else {
        GeomAPI_Circ2d aCircleForArc(aCenterAttr->pnt(), aStartAttr->pnt());
        double aStartParam, anEndParam;
        aCircleForArc.parameter(aStartAttr->pnt(), paramTolerance, aStartParam);
        aCircleForArc.parameter(anEndAttr->pnt(), paramTolerance, anEndParam);
        anAngle = (anEndParam - aStartParam) / PI * 180.0;
        if(isReversed()) anAngle = 360.0 - anAngle;
      }
    }
  }

  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  real(RADIUS_ID())->setValue(aRadius);
  real(ANGLE_ID())->setValue(anAngle);
  data()->blockSendAttributeUpdated(aWasBlocked, false);
}

void SketchPlugin_Arc::setReversed(bool isReversed)
{
  boolean(REVERSED_ID())->setValue(isReversed);
}

bool SketchPlugin_Arc::isReversed()
{
  return boolean(REVERSED_ID())->value();
}
