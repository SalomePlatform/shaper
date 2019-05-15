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

#include "FeaturesPlugin_Union.h"

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

static const int THE_UNION_VERSION_1 = 20190506;

//=================================================================================================
FeaturesPlugin_Union::FeaturesPlugin_Union()
{
}

//=================================================================================================
void FeaturesPlugin_Union::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  initVersion(THE_UNION_VERSION_1, selectionList(BASE_OBJECTS_ID()));
}

//=================================================================================================
void FeaturesPlugin_Union::execute()
{
  ObjectHierarchy anObjects;
  ListOfShape anEmptyList;

  // Getting objects.
  if (!processAttribute(BASE_OBJECTS_ID(), anObjects, anEmptyList))
    return;

  if(anObjects.Objects().size() < 2) {
    setError("Error: Not enough objects for operation. Should be at least 2.");
    return;
  }

  std::string anError;
  int aResultIndex = 0;
  std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;

  int aUnionVersion = version();
  GeomShapePtr aResultCompound = GeomAlgoAPI_CompoundBuilder::compound(ListOfShape());

  // Fuse objects.
  bool isOk = true;
  for (ObjectHierarchy::Iterator anObjectsIt = anObjects.Begin();
       anObjectsIt != anObjects.End() && isOk;
       ++anObjectsIt) {
    GeomShapePtr anObject = *anObjectsIt;
    GeomShapePtr aParent = anObjects.Parent(anObject, false);

    if (aParent && aParent->shapeType() <= GeomAPI_Shape::COMPSOLID) {
      // get parent once again to mark it and the subs as processed
      aParent = anObjects.Parent(anObject);
      // compsolid handling
      isOk = processCompsolid(GeomAlgoAPI_Tools::BOOL_FUSE,
                              anObjects, aParent, anEmptyList, anEmptyList,
                              aResultIndex, aResultBaseAlgoList, aResultShapesList,
                              aResultCompound);
    } else {
      // process object as is
      isOk = processObject(GeomAlgoAPI_Tools::BOOL_FUSE,
                           anObject, anEmptyList, anEmptyList,
                           aResultIndex, aResultBaseAlgoList, aResultShapesList,
                           aResultCompound);
    }
  }

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  for (std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>::iterator
       aRBAIt = aResultBaseAlgoList.begin();
       aRBAIt != aResultBaseAlgoList.end(); ++aRBAIt) {
    aMakeShapeList->appendAlgo(aRBAIt->makeShape);
  }

  GeomShapePtr aShape;
  GeomAPI_ShapeIterator aCIt(aResultCompound);
  if (aUnionVersion < THE_UNION_VERSION_1) {
    // if the compound consists of a single sub-shape, take it,
    // otherwise, take the full compound
    aShape = aCIt.current();
    aCIt.next();
    if (aCIt.more())
      aShape = aResultCompound;
  }
  else {
    // merge hierarchies of compounds containing objects and tools
    aShape = keepUnusedSubsOfCompound(aCIt.current(), anObjects, ObjectHierarchy(), aMakeShapeList);
    for (aCIt.next(); aCIt.more(); aCIt.next()) {
      std::shared_ptr<GeomAlgoAPI_ShapeBuilder> aBuilder(new GeomAlgoAPI_ShapeBuilder);
      aBuilder->add(aShape, aCIt.current());
      aMakeShapeList->appendAlgo(aBuilder);
    }
  }

  // Store result and naming.
  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  ListOfShape anObjectsList = anObjects.Objects();
  aResultBody->storeModified(anObjectsList.front(), aShape);

  for(ListOfShape::const_iterator anIter = anObjectsList.begin(); anIter != anObjectsList.end(); ++anIter) {
    aResultBody->loadModifiedShapes(aMakeShapeList, *anIter, GeomAPI_Shape::EDGE);
    aResultBody->loadModifiedShapes(aMakeShapeList, *anIter, GeomAPI_Shape::FACE);
    //aResultBody->loadDeletedShapes(&aMakeShapeList, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
  }

  setResult(aResultBody);
}
