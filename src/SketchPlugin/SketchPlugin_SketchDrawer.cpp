// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "SketchPlugin_SketchDrawer.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_ConstraintDistance.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintLength.h"
#include "SketchPlugin_ConstraintRadius.h"
#include "SketchPlugin_ConstraintVertical.h"
#include "SketchPlugin_ConstraintHorizontal.h"
#include "SketchPlugin_ConstraintDistanceVertical.h"
#include "SketchPlugin_ConstraintDistanceHorizontal.h"
#include "SketchPlugin_Tools.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_DataMapOfShapeShape.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_ResultConstruction.h>

#include <cmath>

static const double kTOL = 1.e-6;

SketchPlugin_SketchDrawer::SketchPlugin_SketchDrawer() : ModelAPI_Feature()
{}

void SketchPlugin_SketchDrawer::initAttributes()
{
  data()->addAttribute(BASE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PLANE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(ADD_DIMENSIONS_ID(), ModelAPI_AttributeBoolean::typeId());
}

// sets a point attribute of the feature by 3D point on the sketch
static void setPoint(FeaturePtr theFeature, const std::string& theAttrID,
  std::shared_ptr<SketchPlugin_Sketch> theSketch, GeomPointPtr thePoint,
  std::list<std::pair<GeomPnt2dPtr, std::string> >& aPoints)
{
  GeomPnt2dPtr aPnt2D = theSketch->to2D(thePoint);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    theFeature->attribute(theAttrID))->setValue(aPnt2D);
  aPoints.push_back(std::pair<GeomPnt2dPtr, std::string>(aPnt2D, theAttrID));
}

void SketchPlugin_SketchDrawer::execute()
{
  GeomShapePtr aBase = selection(BASE_ID())->value();
  if (!aBase.get() && selection(BASE_ID())->context().get())
    aBase = selection(BASE_ID())->context()->shape();
  if (!aBase.get()) {
    setError("Error: a base shape can not be obtained");
    return; // invalid case
  }

  ObjectPtr aPlaneContext = selection(PLANE_ID())->contextObject();
  GeomShapePtr aPlaneShape = selection(PLANE_ID())->value();
  if (!aPlaneShape.get() && aPlaneContext.get())
    aPlaneShape = selection(PLANE_ID())->context()->shape();
  if (!aPlaneShape.get() || aPlaneShape->shapeType() != GeomAPI_Shape::FACE) {
    setError("Error: a sketch plane can not be obtained");
    return; // invalid case
  }
  GeomFacePtr aPlaneFace(new GeomAPI_Face(aPlaneShape));
  GeomPlanePtr aPlane = aPlaneFace->getPlane();

  // create and initialize sketch
  DocumentPtr aMyDoc = document();
  std::shared_ptr<SketchPlugin_Sketch> aSketch =
    std::dynamic_pointer_cast<SketchPlugin_Sketch>(aMyDoc->addFeature(SketchPlugin_Sketch::ID()));
  // by selection of plane
  aSketch->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->
    setValue(selection(PLANE_ID())->context(), aPlaneShape);
  // remove reference, but keep the sketch position
  aSketch->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->
    setValue(ResultPtr(), GeomShapePtr());

  bool addDimensions = boolean(ADD_DIMENSIONS_ID())->value();
  // iterate all edges of the base to find all edges that belong to this plane
  GeomAPI_DataMapOfShapeShape alreadyProcessed;
  std::list<AttributePoint2DPtr> aCreatedPoints;// points to check and set coincidence
  for(GeomAPI_ShapeExplorer anEdges(aBase, GeomAPI_Shape::EDGE); anEdges.more(); anEdges.next()) {
    if (!alreadyProcessed.bind(anEdges.current(), anEdges.current()))
      continue; // skip duplicated edges
    GeomEdgePtr anEdge(new GeomAPI_Edge(anEdges.current()));
    if (anEdge->isDegenerated())
      continue; // skip degenerated edges
    GeomPointPtr aStart = anEdge->firstPoint();
    GeomPointPtr anEnd = anEdge->lastPoint();
    if (aPlane->distance(aStart) >= kTOL || aPlane->distance(anEnd) >= kTOL)
      continue; // skip edges not in plane

    FeaturePtr anItem;
    std::list<std::pair<GeomPnt2dPtr, std::string> > aPoints; // created point to attribute ID
    if (anEdge->isLine()) { // line is already in the plane: create by two points
      anItem = aSketch->addFeature(SketchPlugin_Line::ID());
      setPoint(anItem, SketchPlugin_Line::START_ID(), aSketch, aStart, aPoints);
      setPoint(anItem, SketchPlugin_Line::END_ID(), aSketch, anEnd, aPoints);
      anItem->execute(); // for constraints setting on result
      // add a vertical or horizontal constraints
      bool isHorVertConstr = true;
      if (fabs(aPoints.front().first->x() - aPoints.back().first->x()) < kTOL) {
        FeaturePtr aVert = aSketch->addFeature(SketchPlugin_ConstraintVertical::ID());
        aVert->refattr(SketchPlugin_Constraint::ENTITY_A())->setObject(anItem->firstResult());
      } else if (fabs(aPoints.front().first->y() - aPoints.back().first->y()) < kTOL) {
        FeaturePtr aHor = aSketch->addFeature(SketchPlugin_ConstraintHorizontal::ID());
        aHor->refattr(SketchPlugin_Constraint::ENTITY_A())->setObject(anItem->firstResult());
      } else {
        isHorVertConstr = false;
      }
      if (addDimensions) {
        if (isHorVertConstr) { // only length constraint is enough
          FeaturePtr aLen = aSketch->addFeature(SketchPlugin_ConstraintLength::ID());
          aLen->refattr(SketchPlugin_ConstraintLength::ENTITY_A())
            ->setObject(anItem->firstResult());
          aLen->real(SketchPlugin_ConstraintLength::VALUE())->setValue(anEdge->length());
        } else { // set horizontal and vertical distance constraints
          FeaturePtr aVDist = aSketch->addFeature(SketchPlugin_ConstraintDistanceVertical::ID());
          aVDist->refattr(SketchPlugin_Constraint::ENTITY_A())
            ->setAttr(anItem->attribute(SketchPlugin_Line::START_ID()));
          aVDist->refattr(SketchPlugin_Constraint::ENTITY_B())
            ->setAttr(anItem->attribute(SketchPlugin_Line::END_ID()));
          aVDist->real(SketchPlugin_ConstraintDistanceVertical::VALUE())
            ->setValue(aPoints.back().first->y() - aPoints.front().first->y());
          FeaturePtr aHDist = aSketch->addFeature(SketchPlugin_ConstraintDistanceHorizontal::ID());
          aHDist->refattr(SketchPlugin_Constraint::ENTITY_A())
            ->setAttr(anItem->attribute(SketchPlugin_Line::START_ID()));
          aHDist->refattr(SketchPlugin_Constraint::ENTITY_B())
            ->setAttr(anItem->attribute(SketchPlugin_Line::END_ID()));
          aHDist->real(SketchPlugin_ConstraintDistanceVertical::VALUE())
            ->setValue(aPoints.back().first->x() - aPoints.front().first->x());
        }
      }
    } else if (anEdge->isArc()) { // check also center
      GeomPointPtr aCenter = anEdge->circle()->center();
      if (aPlane->distance(aCenter) >= kTOL)
        continue;
      // create arc by 3 points
      anItem = aSketch->addFeature(SketchPlugin_Arc::ID());
      setPoint(anItem, SketchPlugin_Arc::CENTER_ID(), aSketch, aCenter, aPoints);
      setPoint(anItem, SketchPlugin_Arc::START_ID(), aSketch, aStart, aPoints);
      setPoint(anItem, SketchPlugin_Arc::END_ID(), aSketch, anEnd, aPoints);
      anItem->execute(); // for constraints setting on result
      // set radius constraint
      if (addDimensions) {
        FeaturePtr aRad = aSketch->addFeature(SketchPlugin_ConstraintRadius::ID());
        aRad->refattr(SketchPlugin_Constraint::ENTITY_A())->setObject(anItem->lastResult());
      }
    } else if (anEdge->isCircle()) { // check also center and middle (at value 2.)
      GeomPointPtr aCenter = anEdge->circle()->center();
      if (aPlane->distance(aCenter) >= kTOL || aPlane->distance(anEdge->middlePoint()) >= kTOL)
        continue;
      // circle by center and radius
      anItem = aSketch->addFeature(SketchPlugin_Circle::ID());
      setPoint(anItem, SketchPlugin_Circle::CENTER_ID(), aSketch, aCenter, aPoints);
      anItem->real(SketchPlugin_Circle::RADIUS_ID())->setValue(anEdge->circle()->radius());
      anItem->execute(); // for constraints setting on result
      // set radius constraint
      if (addDimensions) {
        FeaturePtr aRad = aSketch->addFeature(SketchPlugin_ConstraintRadius::ID());
        aRad->refattr(SketchPlugin_Constraint::ENTITY_A())->setObject(anItem->lastResult());
      }
    } else {
      continue; // other types of edges are not supported, only lines, circles and arcs
    }
    // check some resulting points are coincident to existing
    std::list<std::pair<GeomPnt2dPtr, std::string> >::iterator aPIter = aPoints.begin();
    for(; aPIter != aPoints.end(); aPIter++) {
      AttributePoint2DPtr aPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anItem->attribute(aPIter->second));
      std::list<AttributePoint2DPtr>::iterator aCoincIter = aCreatedPoints.begin();
      for(; aCoincIter != aCreatedPoints.end(); aCoincIter++) {
        double aDX = (*aCoincIter)->x() - aPIter->first->x();
        if (fabs(aDX) >= kTOL)
          continue;
        double aDY = (*aCoincIter)->y() - aPIter->first->y();
        if (fabs(aDY) >= kTOL)
          continue;
        // create a coincidence constraint
        FeaturePtr aCoinc = aSketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
        aCoinc->refattr(SketchPlugin_Constraint::ENTITY_A())->setAttr(aPointAttr);
        aCoinc->refattr(SketchPlugin_Constraint::ENTITY_B())->setAttr(*aCoincIter);
        break; // only one coincidence per point
      }
      aCreatedPoints.push_back(aPointAttr);
    }
  }
  aMyDoc->setCurrentFeature(aSketch, false);
}
