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

#include "BuildPlugin_Edge.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAlgoAPI_Copy.h>

//=================================================================================================
BuildPlugin_Edge::BuildPlugin_Edge()
{
}

//=================================================================================================
void BuildPlugin_Edge::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Edge::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(!aSelectionList.get()) {
    setError("Error: Could not get selection list.");
    return;
  }
  if(aSelectionList->size() == 0) {
    setError("Error: Empty selection list.");
    return;
  }

  // Collect base shapes.
  ListOfShape aListOfShapes;
  int aResultIndex = 0;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      ResultPtr aContext = aSelection->context();
      if(!aContext.get()) {
        setError("Error: Attribute has empty context.");
        return;
      }

      aShape = aContext->shape();
    }
    if(!aShape.get()) {
      setError("Error: Empty shape selected.");
      return;
    }

    if(aShape->shapeType() != GeomAPI_Shape::EDGE) {
      setError("Error: Selected shape has wrong type. Only edges acceptable.");
      return;
    }

    // Copy shape.
    GeomMakeShapePtr aCopyAlgo(new GeomAlgoAPI_Copy(aShape));

    // Check that algo is done.
    if(!aCopyAlgo->isDone()) {
      setError("Error: " + getKind() + " algorithm failed.");
      return;
    }

    // Check if shape is not null.
    if(!aCopyAlgo->shape().get() || aCopyAlgo->shape()->isNull()) {
      setError("Error: Resulting shape is null.");
      return;
    }

    // Check that resulting shape is valid.
    if(!aCopyAlgo->isValid()) {
      setError("Error: Resulting shape is not valid.");
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->storeModified(aShape, aCopyAlgo->shape());
    aResultBody->loadModifiedShapes(aCopyAlgo, aShape, GeomAPI_Shape::VERTEX);

    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  removeResults(aResultIndex);
}
