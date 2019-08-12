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

#include "CollectionPlugin_GroupSubstraction.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultGroup.h>

#include <GeomAPI_ShapeIterator.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

typedef std::set<GeomShapePtr, GeomAPI_Shape::ComparatorWithOri> SetOfShape;

CollectionPlugin_GroupSubstraction::CollectionPlugin_GroupSubstraction()
{
}

void CollectionPlugin_GroupSubstraction::initAttributes()
{
  data()->addAttribute(CollectionPlugin_GroupSubstraction::LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(CollectionPlugin_GroupSubstraction::TOOLS_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
}

static void explodeCompound(const GeomShapePtr& theCompound, SetOfShape& theSet)
{
  for (GeomAPI_ShapeIterator anIt(theCompound); anIt.more(); anIt.next())
    theSet.insert(anIt.current());
}

static void subtractLists(const GeomShapePtr& theCompound,
                          const SetOfShape& theExclude,
                                ListOfShape& theResult)
{
  for (GeomAPI_ShapeIterator anIt(theCompound); anIt.more(); anIt.next()) {
    GeomShapePtr aCurrent = anIt.current();
    if (theExclude.find(aCurrent) != theExclude.end())
      continue; // shape has to be excluded
    // check the shape is already in the list
    ListOfShape::iterator anIt2 = theResult.begin();
    for (; anIt2 != theResult.end(); ++anIt2)
      if (aCurrent->isEqual(*anIt2))
        break;
    if (anIt2 == theResult.end())
      theResult.push_back(aCurrent);
  }
}

void CollectionPlugin_GroupSubstraction::execute()
{
  ResultGroupPtr aGroup = document()->createGroup(data());
  // clean the result of the operation
  aGroup->store(GeomShapePtr());

  // collect shapes to be excluded (tools)
  SetOfShape aShapesToExclude;
  AttributeSelectionListPtr aTools = selectionList(TOOLS_ID());
  for (int anIndex = 0; anIndex < aTools->size(); ++anIndex) {
    AttributeSelectionPtr aCurSelection = aTools->value(anIndex);
    ResultGroupPtr aCurGroup =
      std::dynamic_pointer_cast<ModelAPI_ResultGroup>(aCurSelection->context());
    explodeCompound(aCurGroup->shape(), aShapesToExclude);
  }

  // keep only shapes that should not be excluded
  ListOfShape aCut;
  AttributeSelectionListPtr anObjects = selectionList(LIST_ID());
  for (int anIndex = 0; anIndex < anObjects->size(); ++anIndex) {
    AttributeSelectionPtr aCurSelection = anObjects->value(anIndex);
    ResultGroupPtr aCurGroup =
        std::dynamic_pointer_cast<ModelAPI_ResultGroup>(aCurSelection->context());
    subtractLists(aCurGroup->shape(), aShapesToExclude, aCut);
    if (aCut.empty())
      break;
  }

  if (aCut.empty()) {
    setError("Error: Empty result.");
    removeResults(0);
  }
  else {
    GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aCut);
    aGroup->store(aCompound);
    setResult(aGroup);
  }
}
