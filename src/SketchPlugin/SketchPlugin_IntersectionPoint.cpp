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

#include "SketchPlugin_IntersectionPoint.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomDataAPI_Point2D.h>

SketchPlugin_IntersectionPoint::SketchPlugin_IntersectionPoint()
    : SketchPlugin_Point()
{
}

void SketchPlugin_IntersectionPoint::initDerivedClassAttributes()
{
  data()->addAttribute(EXTERNAL_LINE_ID(), ModelAPI_AttributeSelection::typeId());

  SketchPlugin_Point::initDerivedClassAttributes();
}

void SketchPlugin_IntersectionPoint::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    computePoint();
    SketchPlugin_Point::execute();

    // set this feature as external
    data()->selection(EXTERNAL_ID())->setValue(lastResult(), lastResult()->shape());
  }
}

void SketchPlugin_IntersectionPoint::move(double theDeltaX, double theDeltaY)
{
}

void SketchPlugin_IntersectionPoint::attributeChanged(const std::string& theID)
{
  if (theID == EXTERNAL_LINE_ID()) {
    // compute intersection between line and sketch plane
    computePoint();
  }
}

void SketchPlugin_IntersectionPoint::computePoint()
{
  AttributeSelectionPtr aLineAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(attribute(EXTERNAL_LINE_ID()));

  std::shared_ptr<GeomAPI_Edge> anEdge;
  if(aLineAttr && aLineAttr->value() && aLineAttr->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aLineAttr->value()));
  } else if(aLineAttr->context() && aLineAttr->context()->shape() &&
            aLineAttr->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aLineAttr->context()->shape()));
  }
  if(!anEdge.get())
    return;

  std::shared_ptr<GeomAPI_Lin> aLine = anEdge->line();
  std::shared_ptr<GeomAPI_Pln> aSketchPlane = sketch()->plane();

  std::shared_ptr<GeomAPI_Pnt> anIntersection = aSketchPlane->intersect(aLine);
  if (!anIntersection)
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aCoordAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(COORD_ID()));
  aCoordAttr->setValue(sketch()->to2D(anIntersection));
}
