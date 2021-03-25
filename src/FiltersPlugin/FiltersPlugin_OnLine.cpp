// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "FiltersPlugin_OnLine.h"

#include <ModelAPI_AttributeSelectionList.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Vertex.h>


static void convertToLines(const AttributeSelectionListPtr& theSelected,
                           std::list<GeomLinePtr>& theLines)
{
  for (int i = 0; i < theSelected->size(); i++) {
    AttributeSelectionPtr aSel = theSelected->value(i);
    GeomShapePtr aCurShape = aSel->value();
    if (!aCurShape)
      aCurShape = aSel->context()->shape();
    if (aCurShape && aCurShape->isEdge()) {
      GeomLinePtr aLine = aCurShape->edge()->line();
      if (aLine)
        theLines.push_back(aLine);
    }
  }
}


bool FiltersPlugin_OnLine::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::EDGE || theType == GeomAPI_Shape::VERTEX;
}

bool FiltersPlugin_OnLine::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr aAttr = theArgs.argument("OnLine");
  AttributeSelectionListPtr aList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  if (!aList.get())
    return false;
  // convert selected shapes to lines
  std::list<GeomLinePtr> aLines;
  convertToLines(aList, aLines);

  if (theShape->isVertex()) {
    GeomPointPtr aPnt = theShape->vertex()->point();
    for (std::list<GeomLinePtr>::iterator anIt = aLines.begin(); anIt != aLines.end(); ++anIt) {
      if ((*anIt)->contains(aPnt))
        return true;
    }
  }
  else if (theShape->isEdge()) {
    GeomEdgePtr aEdge(new GeomAPI_Edge(theShape));
    if (aEdge->isLine()) {
      GeomPointPtr aPnt1 = aEdge->firstPoint();
      GeomPointPtr aPnt2 = aEdge->lastPoint();
      for (std::list<GeomLinePtr>::iterator anIt = aLines.begin(); anIt != aLines.end(); ++anIt) {
        if ((*anIt)->contains(aPnt1) && (*anIt)->contains(aPnt2))
          return true;
      }
    }
  }
  return false;
}

std::string FiltersPlugin_OnLine::xmlRepresentation() const
{
  return xmlFromFile("filter-OnLine.xml");
}

void FiltersPlugin_OnLine::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("OnLine", ModelAPI_AttributeSelectionList::typeId());
}
