// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "FiltersPlugin_BelongsTo.h"

#include <ModelAPI_AttributeSelectionList.h>

bool FiltersPlugin_BelongsTo::isSupported(GeomAPI_Shape::ShapeType /*theType*/) const
{
  return true;
}

bool FiltersPlugin_BelongsTo::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                   const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr aAttr = theArgs.argument("BelongsTo");
  AttributeSelectionListPtr aList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  if (!aList.get())
    return false;
  for (int i = 0; i < aList->size(); i++) {
    AttributeSelectionPtr aCurAttr = aList->value(i);
    GeomShapePtr aGeom = aCurAttr->value();
    if (aGeom && aGeom->isSubShape(theShape))
      return true;
  }
  return false;
}

std::string FiltersPlugin_BelongsTo::xmlRepresentation() const
{
  return xmlFromFile("filter-BelongsTo.xml");
}

void FiltersPlugin_BelongsTo::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("BelongsTo", ModelAPI_AttributeSelectionList::typeId());
}
