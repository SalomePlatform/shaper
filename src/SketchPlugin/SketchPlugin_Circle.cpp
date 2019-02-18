// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
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


SketchPlugin_Circle::SketchPlugin_Circle()
: SketchPlugin_SketchEntity()
{
}

void SketchPlugin_Circle::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
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
  if(!aCenterAttr->isInitialized() || !aRadiusAttr->isInitialized()) {
    return;
  }

  double aRadius = aRadiusAttr->value();
  if(aRadius < tolerance) {
    return;
  }

  // Make a visible point.
  SketchPlugin_Sketch::createPoint2DResult(this, sketch(), CENTER_ID(), 0);

  // Make a visible circle.
  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

  std::shared_ptr<GeomAPI_Shape> aCircleShape =
      GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aRadius);

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
    }
  }
}
