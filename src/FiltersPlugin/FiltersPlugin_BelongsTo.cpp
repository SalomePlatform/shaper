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

#include "FiltersPlugin_BelongsTo.h"

bool FiltersPlugin_BelongsTo::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return true;
}

bool FiltersPlugin_BelongsTo::isOk(const GeomShapePtr& theShape,
  const ModelAPI_FiltersArgs& theArgs) const
{
  return true;
}

static std::string XMLRepresentation =
"<filter id = \"Belongs to\""
" <multi_selector id=\"Belongs to\""
"   label = \"Objects:\""
"   tooltip = \"Select objects to limit selection.\""
"   type_choice = \"objects\""
" </multi_selector>"
"</filter>";


std::string FiltersPlugin_BelongsTo::xmlRepresentation() const
{
  return XMLRepresentation;
}