// Copyright (C) 2014-2023  CEA, EDF
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

#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Tools.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_ConstraintCoincidenceInternal.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

#include <cmath>

const double tolerance = 1e-7;
const double PI = 3.141592653589793238463;


SketchPlugin_Circle::SketchPlugin_Circle()
: SketchPlugin_SketchEntity()
{
}

void SketchPlugin_Circle::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(ROTATE_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(ROTATE_REF_ID(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  // While exist addCircle without creating point.
  AttributeIntegerPtr aVerAttr = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(
    data()->addAttribute(VERSION_ID(), ModelAPI_AttributeInteger::typeId()));
  aVerAttr->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VERSION_ID());
  if (!aVerAttr->isInitialized()) {
    // this is a newly created feature (not read from file),
    // so, initialize the latest version
    aVerAttr->setValue(SketchPlugin_Circle::THE_VERSION_1);
  }

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ROTATE_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ROTATE_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ANGLE_ID());
}

void SketchPlugin_Circle::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return;
  }

  // Compute a circle in 3D view.
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  AttributeDoublePtr aRadiusAttr = real(RADIUS_ID());
  AttributeDoublePtr anAngleAttr = real(ANGLE_ID());
  if(!aCenterAttr->isInitialized() || !aRadiusAttr->isInitialized()) {
    return;
  }

  double aRadius = aRadiusAttr->value();
  if(aRadius < tolerance) {
    return;
  }

  if (!anAngleAttr->isInitialized())
    anAngleAttr->setValue(0);

  // Make a visible center point.
  SketchPlugin_Sketch::createPoint2DResult(this, sketch(), CENTER_ID(), 0);

  // Make a visible circle.
  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

  std::shared_ptr<GeomAPI_Edge> aCircleShape;
  if (integer(VERSION_ID())->value() > SketchPlugin_Circle::THE_VERSION_0)
  {
    double aValAn = anAngleAttr->value();
    aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aRadius, aValAn / 180 * PI);

    // Update corrdinates for point on circle
    bool aWasBlocked = data()->blockSendAttributeUpdated(true);
    GeomPnt2dPtr aCircleSewPoint = aSketch->to2D(aCircleShape->firstPoint());
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(ROTATE_ID()))
      ->setValue(aCircleSewPoint->x(), aCircleSewPoint->y());
    data()->blockSendAttributeUpdated(aWasBlocked, false);
  }
  else
    aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aRadius);

  std::shared_ptr<ModelAPI_ResultConstruction> aResult = document()->createConstruction(data(), 1);
  aResult->setShape(aCircleShape);
  aResult->setIsInHistory(false);
  setResult(aResult, 1);
}

bool SketchPlugin_Circle::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_Circle::attributeChanged(const std::string& theID) {
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
      std::shared_ptr<GeomAPI_Edge> anEdge( new GeomAPI_Edge(aSelection));
      std::shared_ptr<GeomAPI_Circ> aCirc = anEdge->circle();
      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_ID()));
      aCenterAttr->setValue(sketch()->to2D(aCirc->center()));

      real(RADIUS_ID())->setValue(aCirc->radius());
      real(ANGLE_ID())->setValue(0);
    }
  }
  else if (integer(VERSION_ID())->value() > SketchPlugin_Circle::THE_VERSION_0 &&
           theID == ROTATE_ID())
  {
    // get cricle from results
    std::shared_ptr<GeomAPI_Shape> aSelection;
    ResultPtr aCircleRes = lastResult();
    if (aCircleRes)
      aSelection = aCircleRes->shape();
    if (aSelection && !aSelection->isNull() && aSelection->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aSelection));
      std::shared_ptr<GeomAPI_Circ> aCirc = anEdge->circle();

      // Get point and project it on line
      std::shared_ptr<GeomDataAPI_Point2D> aNewPntRot =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(ROTATE_ID()));
      GeomPointPtr aPntRot3D = aCirc->project( sketch()->to3D(aNewPntRot->x(), aNewPntRot->y()));

      // Compute new angle
      GeomPointPtr aNorm = sketch()->to3D(real(RADIUS_ID())->value(), 0);
      double aStartParam, anEndParam;
      aCirc->parameter(aPntRot3D, 1.e-4, anEndParam);
      aCirc->parameter(aNorm, 1.e-4, aStartParam);

      bool aWasBlocked = data()->blockSendAttributeUpdated(true);
      real(ANGLE_ID())->setValue((anEndParam - aStartParam) / PI * 180.0);
      data()->blockSendAttributeUpdated(aWasBlocked, false);
    }
  }
}
