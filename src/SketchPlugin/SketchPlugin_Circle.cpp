// Copyright (C) 2014-2024  CEA, EDF
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
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>
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

#include <Events_Loop.h>

#include <cmath>

const double tolerance = 1e-7;
const double paramTolerance = 1.e-4;
const double PI = 3.141592653589793238463;

//*************************************************************************************
SketchPlugin_Circle::SketchPlugin_Circle()
: SketchPlugin_SketchEntity()
{
}

//*************************************************************************************
void SketchPlugin_Circle::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());

  // Attrs for construction point
  data()->addAttribute(CONSTRUCTION_POINT_REF_ID(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  AttributeDoublePtr anAngleAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>
     (data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId()));
  if(!anAngleAttr->isInitialized())
  {
    anAngleAttr->setValue(0.0);
  }

  AttributeBooleanPtr isAddConstrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>
     (data()->addAttribute(ADD_CONSTRUCTION_POINT_ID(), ModelAPI_AttributeBoolean::typeId()));
  if (!isAddConstrAttr->isInitialized())
  {
    isAddConstrAttr->setValue(false);
  }

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ADD_CONSTRUCTION_POINT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CONSTRUCTION_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ANGLE_ID());
}

//*************************************************************************************
void SketchPlugin_Circle::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch) {
    return;
  }

  //manage construction point movement/deletion
  bool isToAddPoint = boolean(ADD_CONSTRUCTION_POINT_ID())->value();
  AttributeReferencePtr aConstrRefAttr = reference((CONSTRUCTION_POINT_REF_ID()));
  if(aConstrRefAttr->isInitialized())
    isToAddPoint ? computeAngle(): removeConstructionPoint();

  // Compute a circle in 3D view.
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  AttributeDoublePtr aRadiusAttr = real(RADIUS_ID());
  if (!aCenterAttr->isInitialized() || !aRadiusAttr->isInitialized()) {
    return;
  }

  double aRadius = aRadiusAttr->value();
  if (aRadius < tolerance) {
    return;
  }

  // Make a visible center point.
  SketchPlugin_Sketch::createPoint2DResult(this, sketch(), CENTER_ID(), 0);

  // Make a visible circle.
  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

  AttributeDoublePtr anAngleAttr = real(ANGLE_ID());
  if (!anAngleAttr->isInitialized())
    anAngleAttr->setValue(0);
  double aValAn = anAngleAttr->value();

  std::shared_ptr<GeomAPI_Edge> aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aRadius, aValAn / 180 * PI);

  std::shared_ptr<ModelAPI_ResultConstruction> aResult = document()->createConstruction(data(), 1);
  aResult->setShape(aCircleShape);
  aResult->setIsInHistory(false);
  setResult(aResult, 1);

  if(isToAddPoint && !aConstrRefAttr->isInitialized())
    setConstructionPoint();
}

//*************************************************************************************
std::shared_ptr<GeomAPI_Edge> SketchPlugin_Circle::getCircleShape()
{
  std::shared_ptr<GeomAPI_Shape> aSelection;
  ResultPtr aCircleRes = lastResult();
  if (aCircleRes)
    aSelection = aCircleRes->shape();
  if (aSelection && !aSelection->isNull() && aSelection->isEdge()) 
  {
    return std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aSelection));
  }
  return {};
}

//*************************************************************************************
void SketchPlugin_Circle::computeAngle()
{
  if(!boolean(ADD_CONSTRUCTION_POINT_ID())->value()){
    return;
  }

  AttributeReferencePtr aConstrRefAttr = reference((CONSTRUCTION_POINT_REF_ID()));
  std::shared_ptr<GeomAPI_Edge> anCircEdge = getCircleShape();
  if(anCircEdge && aConstrRefAttr->isInitialized())
  {
    std::shared_ptr<GeomAPI_Circ> aCirc = anCircEdge->circle();

    // Get point and project it on line
    FeaturePtr aPointFeature = ModelAPI_Feature::feature(aConstrRefAttr->value());
    AttributePoint2DPtr aConstrPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
           aPointFeature->attribute(SketchPlugin_Point::COORD_ID()));
    GeomPointPtr aPntRot3D = aCirc->project( sketch()->to3D(aConstrPoint->x(), aConstrPoint->y()));

    // Compute new angle
    GeomPointPtr aNorm = sketch()->to3D(real(RADIUS_ID())->value(), 0);
    double aStartParam, anEndParam;
    aCirc->parameter(aPntRot3D, paramTolerance, anEndParam);
    aCirc->parameter(aNorm, paramTolerance, aStartParam);

    bool aWasBlocked = data()->blockSendAttributeUpdated(true);
    real(ANGLE_ID())->setValue((anEndParam - aStartParam) / PI * 180.0);
    data()->blockSendAttributeUpdated(aWasBlocked, false);
  }
}

//*************************************************************************************
void SketchPlugin_Circle::setConstructionPoint()
{
  if(!boolean(ADD_CONSTRUCTION_POINT_ID())->value()){
    return;
  }

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch) {
    return;
  }

  AttributeReferencePtr aConstrRefAttr = reference((CONSTRUCTION_POINT_REF_ID()));
  if(!aConstrRefAttr->isInitialized())
  {
    bool aWasBlocked = data()->blockSendAttributeUpdated(true);
    std::shared_ptr<GeomAPI_Edge> anCircEdge = getCircleShape();
    GeomPnt2dPtr aCircleSewPoint = aSketch->to2D(anCircEdge->firstPoint());

    FeaturePtr aPointFeature = aSketch->addFeature(SketchPlugin_Point::ID());
    auto aCurCircle = this->document()->currentFeature(true);
    aPointFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(aCurCircle);
    AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aPointFeature->attribute(SketchPlugin_Point::COORD_ID()));
    aCoord->setValue(aCircleSewPoint);
    aPointFeature->execute();
    aConstrRefAttr->setValue(aPointFeature);

    SketchPlugin_Tools::createConstraintAttrObject(aSketch,
           SketchPlugin_ConstraintCoincidence::ID(), aCoord, lastResult());
    data()->blockSendAttributeUpdated(aWasBlocked, false);
  }
}

//*************************************************************************************
void SketchPlugin_Circle::removeConstructionPoint()
{
  AttributeReferencePtr aConstrRefAttr = reference((CONSTRUCTION_POINT_REF_ID()));
  FeaturePtr aPointFeature = ModelAPI_Feature::feature(aConstrRefAttr->value());
  if (aPointFeature)
  {
    std::set<FeaturePtr> aDummySet;
    aDummySet.insert(aPointFeature);
    std::map<FeaturePtr, std::set<FeaturePtr>> aReferenceMap;
    ModelAPI_Tools::findAllReferences(aDummySet, aReferenceMap, false, true);
    std::set<FeaturePtr> aFeaturesRefsTo;
    ModelAPI_Tools::findRefsToFeatures(aDummySet, aReferenceMap, aFeaturesRefsTo);

    // remove all references to the point except the circle feature(this)
    for(auto aFeature : aFeaturesRefsTo)
    {
      if (aFeature->getKind() != this->ID())
        document()->removeFeature(aFeature);
    }
    document()->removeFeature(aPointFeature);
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  }
  aConstrRefAttr->setValue(FeaturePtr());
  aConstrRefAttr->reset();
  real(ANGLE_ID())->setValue(0.0);
}

//*************************************************************************************
bool SketchPlugin_Circle::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

//*************************************************************************************
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
      real(ANGLE_ID())->setValue(0.0);
    }
  }
}
