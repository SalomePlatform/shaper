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

#include "FiltersPlugin_RelativeToSolid.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>


bool FiltersPlugin_RelativeToSolid::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return true;
}

bool FiltersPlugin_RelativeToSolid::isOk(const GeomShapePtr& theShape,
                                         const ModelAPI_FiltersArgs& theArgs) const
{
  return false;
}

static std::string XMLRepresentation =
"<filter id = \"RelativeToSolid\">"
" <shape_selector id=\"RelativeToSolid__Solid\""
"   label=\"Solid:\""
"   tooltip=\"Select a solid.\""
"   shape_types=\"solids\">"
"   <validator id=\"GeomValidators_ShapeType\" parameters=\"solid\"/>"
" </shape_selector>"
" <switch id=\"RelativeToSolid__Location\" label=\"Algorithm:\">"
"   <case id=\"in\" title=\"In\"/>"
"   <case id=\"out\" title=\"Out\"/>"
"   <case id=\"on\" title=\"On\"/>"
"   <case id=\"noton\" title=\"Not On\"/>"
"   <case id=\"inon\" title=\"In and On\"/>"
"   <case id=\"outon\" title=\"On and Out\"/>"
" </switch>"
"</filter>";


std::string FiltersPlugin_RelativeToSolid::xmlRepresentation() const
{
  return XMLRepresentation;
}

void FiltersPlugin_RelativeToSolid::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("Solid", ModelAPI_AttributeSelection::typeId());
  theArguments.initAttribute("Location", ModelAPI_AttributeString::typeId());
}
