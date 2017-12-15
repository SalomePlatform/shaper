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

#include <SketchPlugin_Projection.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintRigid.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomDataAPI_Point2D.h>

#include <cmath>

static const double tolerance = 1.e-7;

SketchPlugin_Projection::SketchPlugin_Projection()
    : SketchPlugin_SketchEntity(),
      myIsComputing(false)
{
}

void SketchPlugin_Projection::initDerivedClassAttributes()
{
  data()->addAttribute(EXTERNAL_FEATURE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PROJECTED_FEATURE_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->attribute(PROJECTED_FEATURE_ID())->setIsArgument(false);

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());

  data()->addAttribute(INCLUDE_INTO_RESULT(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), AUXILIARY_ID());
}

void SketchPlugin_Projection::execute()
{
  AttributeRefAttrPtr aRefAttr = data()->refattr(PROJECTED_FEATURE_ID());
  if (!aRefAttr || !aRefAttr->isInitialized())
    return;
  FeaturePtr aProjection = ModelAPI_Feature::feature(aRefAttr->object());

  if (!lastResult().get()) {
    bool hasProjResult = aProjection->lastResult().get() != NULL;
    ResultConstructionPtr aConstr = document()->createConstruction(data());
    if (hasProjResult)
      aConstr->setShape(aProjection->lastResult()->shape());
    aConstr->setIsInHistory(false);
    aConstr->setDisplayed(false);
    setResult(aConstr);

    if (hasProjResult)
      aProjection->selection(EXTERNAL_ID())->setValue(lastResult(), lastResult()->shape());
  }

  // is sketch plane is changed (issue 1791), attribute of projection is not changed, but
  // projection must be fully recomputed
  computeProjection(EXTERNAL_FEATURE_ID());
}

void SketchPlugin_Projection::attributeChanged(const std::string& theID)
{
  if ((theID == EXTERNAL_FEATURE_ID() || theID == EXTERNAL_ID()) && !myIsComputing) {
    myIsComputing = true;
    computeProjection(theID);
    myIsComputing = false;
  }
}

static bool isValidProjectionType(FeaturePtr theProjection,
                                  GeomEdgePtr theEdge,
                                  GeomVertexPtr theVertex)
{
  if (theVertex && theProjection->getKind() == SketchPlugin_Point::ID())
    return true;
  if (theEdge) {
    if (theEdge->isLine() && theProjection->getKind() == SketchPlugin_Line::ID())
      return true;
    else if (theEdge->isCircle() && theProjection->getKind() == SketchPlugin_Circle::ID())
      return true;
    else if (theEdge->isArc() && theProjection->getKind() == SketchPlugin_Arc::ID())
      return true;
  }
  return false;
}

void SketchPlugin_Projection::computeProjection(const std::string& theID)
{
  AttributeSelectionPtr aExtFeature =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(attribute(EXTERNAL_FEATURE_ID()));

  GeomShapePtr aShape;
  GeomEdgePtr anEdge;
  GeomVertexPtr aVertex;
  if (aExtFeature)
    aShape = aExtFeature->value();
  if (!aShape && aExtFeature->context())
    aShape = aExtFeature->context()->shape();
  if (aShape) {
    if (aShape->isEdge())
      anEdge = GeomEdgePtr(new GeomAPI_Edge(aShape));
    else if (aShape->isVertex())
      aVertex = GeomVertexPtr(new GeomAPI_Vertex(aShape));
  }
  if (!anEdge && !aVertex)
    return;

  AttributeRefAttrPtr aRefAttr = data()->refattr(PROJECTED_FEATURE_ID());
  FeaturePtr aProjection;
  if (aRefAttr && aRefAttr->isInitialized())
    aProjection = ModelAPI_Feature::feature(aRefAttr->object());

  // if the type of feature differs with already selected, remove it and create once again
  bool hasPrevProj = aProjection.get() != 0;
  if (hasPrevProj && !isValidProjectionType(aProjection, anEdge, aVertex)) {
    DocumentPtr aDoc = sketch()->document();

    aRefAttr->setObject(data()->owner()); // to not remove of this remove reference to aProjection
    std::set<FeaturePtr> aFeaturesToBeRemoved;
    aFeaturesToBeRemoved.insert(aProjection);
    ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);
    aProjection = FeaturePtr();
    aRefAttr->setObject(aProjection);
    hasPrevProj = false;
  }

  std::shared_ptr<GeomAPI_Pln> aSketchPlane = sketch()->plane();

  ResultConstructionPtr aResult =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(lastResult());
  if (aResult && aResult->shape() && theID == EXTERNAL_FEATURE_ID()) {
    aResult->setShape(std::shared_ptr<GeomAPI_Edge>());
    if (aProjection)
      aProjection->selection(EXTERNAL_ID())->setValue(lastResult(), lastResult()->shape());
  }

  if (aVertex) {
    std::shared_ptr<GeomAPI_Pnt> aPrjPnt = aSketchPlane->project(aVertex->point());
    std::shared_ptr<GeomAPI_Pnt2d> aPntInSketch = sketch()->to2D(aPrjPnt);

    if (!hasPrevProj)
      aProjection = sketch()->addFeature(SketchPlugin_Point::ID());

    // update coordinates of projection
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Point::COORD_ID()))->setValue(aPntInSketch);
  }
  else if (anEdge->isLine()) {
    std::shared_ptr<GeomAPI_Pnt> aFirst = aSketchPlane->project(anEdge->firstPoint());
    std::shared_ptr<GeomAPI_Pnt> aLast = aSketchPlane->project(anEdge->lastPoint());

    std::shared_ptr<GeomAPI_Pnt2d> aFirstInSketch = sketch()->to2D(aFirst);
    std::shared_ptr<GeomAPI_Pnt2d> aLastInSketch = sketch()->to2D(aLast);
    if (aFirstInSketch->distance(aLastInSketch) < tolerance)
      return; // line is semi-orthogonal to the sketch plane

    if (!hasPrevProj)
      aProjection = sketch()->addFeature(SketchPlugin_Line::ID());

    // update attributes of projection
    std::shared_ptr<GeomDataAPI_Point2D> aStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Line::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Line::END_ID()));
    aStartPnt->setValue(aFirstInSketch);
    aEndPnt->setValue(aLastInSketch);
  }
  else if (anEdge->isCircle()) {
    std::shared_ptr<GeomAPI_Circ> aCircle = anEdge->circle();
    double aRadius = aCircle->radius();

    std::shared_ptr<GeomAPI_Pnt> aCenter = aSketchPlane->project(aCircle->center());
    std::shared_ptr<GeomAPI_Pnt2d> aCenterInSketch = sketch()->to2D(aCenter);

    if (!hasPrevProj)
      aProjection = sketch()->addFeature(SketchPlugin_Circle::ID());

    // update attributes of projection
    std::shared_ptr<GeomDataAPI_Point2D> aCenterPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Circle::CENTER_ID()));
    aCenterPnt->setValue(aCenterInSketch);
    aProjection->real(SketchPlugin_Circle::RADIUS_ID())->setValue(aRadius);
  }
  else if (anEdge->isArc()) {
    std::shared_ptr<GeomAPI_Pnt> aFirst = aSketchPlane->project(anEdge->firstPoint());
    std::shared_ptr<GeomAPI_Pnt> aLast = aSketchPlane->project(anEdge->lastPoint());
    std::shared_ptr<GeomAPI_Pnt2d> aFirstInSketch = sketch()->to2D(aFirst);
    std::shared_ptr<GeomAPI_Pnt2d> aLastInSketch = sketch()->to2D(aLast);

    std::shared_ptr<GeomAPI_Circ> aCircle = anEdge->circle();
    std::shared_ptr<GeomAPI_Pnt> aCenter = aSketchPlane->project(aCircle->center());
    std::shared_ptr<GeomAPI_Pnt2d> aCenterInSketch = sketch()->to2D(aCenter);

    bool isInversed = aCircle->normal()->dot(aSketchPlane->direction()) < 0.;

    if (!hasPrevProj)
      aProjection = sketch()->addFeature(SketchPlugin_Arc::ID());

    bool aWasBlocked = aProjection->data()->blockSendAttributeUpdated(true);

    // update attributes of projection
    std::shared_ptr<GeomDataAPI_Point2D> aCenterPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Arc::CENTER_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aStartPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Arc::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Arc::END_ID()));
    aStartPnt->setValue(aFirstInSketch);
    aEndPnt->setValue(aLastInSketch);
    aCenterPnt->setValue(aCenterInSketch);
    aProjection->boolean(SketchPlugin_Arc::REVERSED_ID())->setValue(isInversed);

    aProjection->data()->blockSendAttributeUpdated(aWasBlocked, false);
  } else
    return;

  aProjection->boolean(COPY_ID())->setValue(true);
  aProjection->execute();
  aRefAttr->setObject(aProjection);

  if (theID == EXTERNAL_FEATURE_ID()) {
    selection(EXTERNAL_ID())->selectValue(aExtFeature);

    if (aResult) {
      aResult->setShape(aProjection->lastResult()->shape());
      setResult(aResult);
      GeomShapePtr anEmptyVal;
      aProjection->selection(EXTERNAL_ID())->setValue(lastResult(), anEmptyVal);
    }
  }
}
