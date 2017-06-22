// Copyright (C) 2017  CEA/DEN, EDF R&D
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

// File:        SketchPlugin_Ellipse.cpp
// Created:     26 April 2017
// Author:      Artem ZHIDKOV

#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_Sketch.h>

#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Ellipse.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

static const double tolerance = 1e-7;


SketchPlugin_Ellipse::SketchPlugin_Ellipse()
: SketchPlugin_SketchEntity()
{
}

void SketchPlugin_Ellipse::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(FOCUS_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(MAJOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(MINOR_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_Ellipse::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return;
  }

  // Compute a ellipse in 3D view.
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aFocusAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(FOCUS_ID()));
  AttributeDoublePtr aMajorRadiusAttr = real(MAJOR_RADIUS_ID());
  AttributeDoublePtr aMinorRadiusAttr = real(MINOR_RADIUS_ID());
  if (!aCenterAttr->isInitialized() ||
      !aFocusAttr->isInitialized() ||
      !aMajorRadiusAttr->isInitialized() ||
      !aMinorRadiusAttr->isInitialized()) {
    return;
  }

  double aMajorRadius = aMajorRadiusAttr->value();
  double aMinorRadius = aMinorRadiusAttr->value();
  if(aMajorRadius < tolerance || aMinorRadius < tolerance) {
    return;
  }

  // Make a visible point.
  SketchPlugin_Sketch::createPoint2DResult(this, aSketch, CENTER_ID(), 0);

  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aSketch->attribute(SketchPlugin_Sketch::NORM_ID()));

  // Make a visible ellipse.
  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
  std::shared_ptr<GeomAPI_Pnt> aFocus(aSketch->to3D(aFocusAttr->x(), aFocusAttr->y()));
  std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  std::shared_ptr<GeomAPI_Dir> aMajorAxis(new GeomAPI_Dir(aFocus->x() - aCenter->x(),
      aFocus->y() - aCenter->y(), aFocus->z() - aCenter->z()));

  std::shared_ptr<GeomAPI_Shape> anEllipseShape =
      GeomAlgoAPI_EdgeBuilder::ellipse(aCenter, aNormal, aMajorAxis, aMajorRadius, aMinorRadius);

  std::shared_ptr<ModelAPI_ResultConstruction> aResult = document()->createConstruction(data(), 1);
  aResult->setShape(anEllipseShape);
  aResult->setIsInHistory(false);
  setResult(aResult, 1);
}

void SketchPlugin_Ellipse::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if(!aData->isValid()) {
    return;
  }

  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(CENTER_ID()));
  if(aPoint->isInitialized()) {
    aPoint->move(theDeltaX, theDeltaY);
  }
}

bool SketchPlugin_Ellipse::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_Ellipse::attributeChanged(const std::string& theID) {
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
      std::shared_ptr<GeomAPI_Ellipse> anEllipse = anEdge->ellipse();

      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_ID()));
      aCenterAttr->setValue(sketch()->to2D(anEllipse->center()));

      std::shared_ptr<GeomDataAPI_Point2D> aFocusAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(FOCUS_ID()));
      aFocusAttr->setValue(sketch()->to2D(anEllipse->firstFocus()));

      real(MAJOR_RADIUS_ID())->setValue(anEllipse->majorRadius());
      real(MINOR_RADIUS_ID())->setValue(anEllipse->minorRadius());
    }
  }
}
