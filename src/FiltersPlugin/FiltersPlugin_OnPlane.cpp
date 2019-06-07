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

#include "FiltersPlugin_OnPlane.h"

#include <ModelAPI_AttributeSelectionList.h>

bool FiltersPlugin_OnPlane::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  switch (theType) {
  case GeomAPI_Shape::SHELL:
  case GeomAPI_Shape::FACE:
  case GeomAPI_Shape::WIRE:
  case GeomAPI_Shape::EDGE:
  case GeomAPI_Shape::VERTEX:
    return true;
  }
  return false;
}

bool FiltersPlugin_OnPlane::isOk(const GeomShapePtr& theShape,
  const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr aAttr = theArgs.argument("OnPlane");
  AttributeSelectionListPtr aList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  if (!aList.get())
    return false;
  // ToDo
  return false;
}

static std::string XMLRepresentation =
"<filter id = \"OnPlane\">"
" <multi_selector id=\"OnPlane__OnPlane\""
"   label=\"Planes:\""
"   tooltip=\"Select planes or planar faces.\""
"   type_choice=\"faces\">"
"   <validator id=\"GeomValidators_ShapeType\" parameters=\"plane\"/>"
" </multi_selector>"
"</filter>";


std::string FiltersPlugin_OnPlane::xmlRepresentation() const
{
  return XMLRepresentation;
}

void FiltersPlugin_OnPlane::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("OnPlane", ModelAPI_AttributeSelectionList::typeId());
}
