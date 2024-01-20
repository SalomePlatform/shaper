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

#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>

SketchPlugin_Point::SketchPlugin_Point()
    : SketchPlugin_SketchEntity()
{
}

void SketchPlugin_Point::initDerivedClassAttributes()
{
  data()->addAttribute(SketchPlugin_Point::COORD_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_Point::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // compute a point in 3D view
    std::shared_ptr<GeomDataAPI_Point2D> aPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
            data()->attribute(SketchPlugin_Point::COORD_ID()));
    std::shared_ptr<GeomAPI_Pnt> aPoint3D(aSketch->to3D(aPoint->x(), aPoint->y()));
    // make a visible point
    std::shared_ptr<GeomAPI_Shape> aPointShape = GeomAlgoAPI_PointBuilder::vertex(aPoint3D);
    std::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
    aConstr->setShape(aPointShape);
    aConstr->setIsInHistory(false);
    setResult(aConstr);
  }
}

bool SketchPlugin_Point::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_Point::attributeChanged(const std::string& theID) {
  // the second condition for inability to move external point anywhere
  if (theID == EXTERNAL_ID() || isFixed()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
    if (!aSelection) {
      // empty shape in selection shows that the shape is equal to context
      ResultPtr anExtRes = selection(EXTERNAL_ID())->context();
      if (anExtRes)
        aSelection = anExtRes->shape();
    }
    // update arguments due to the selection value
    if (aSelection && !aSelection->isNull() && aSelection->isVertex()) {
      std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aSelection));
      std::shared_ptr<GeomDataAPI_Point2D> aCoordAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(COORD_ID()));
      aCoordAttr->setValue(sketch()->to2D(aVertex->point()));
    }
  }
}
