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

#include "FiltersPlugin_OnGeometry.h"

#include <ModelAPI_AttributeSelectionList.h>

bool FiltersPlugin_OnGeometry::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::EDGE || theType == GeomAPI_Shape::FACE;
}

bool FiltersPlugin_OnGeometry::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                    const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr aAttr = theArgs.argument("OnGeometry");
  AttributeSelectionListPtr aList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  if (!aList.get())
    return false;
  for (int i = 0; i < aList->size(); i++) {
    AttributeSelectionPtr aAttr = aList->value(i);
    GeomShapePtr aGeom = aAttr->value();
    if (!aGeom)
      aGeom = aAttr->context()->shape();
    if (aGeom && aGeom->isSameGeometry(theShape))
      return true;
  }
  return false;
}

std::string FiltersPlugin_OnGeometry::xmlRepresentation() const
{
  return xmlFromFile("filter-OnGeometry.xml");
}

void FiltersPlugin_OnGeometry::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("OnGeometry", ModelAPI_AttributeSelectionList::typeId());
}
