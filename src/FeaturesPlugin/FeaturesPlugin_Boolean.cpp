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

#include "FeaturesPlugin_Boolean.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <algorithm>
#include <map>

//=================================================================================================
FeaturesPlugin_Boolean::FeaturesPlugin_Boolean(const OperationType theOperationType)
: myOperationType(theOperationType)
{
}

//=================================================================================================
void FeaturesPlugin_Boolean::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Boolean::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(FeaturesPlugin_Boolean::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(FeaturesPlugin_Boolean::FUZZY_PARAM_ID(), ModelAPI_AttributeDouble::typeId());
  // Initialize the fuzzy parameter with a value below Precision::Confusion() to indicate,
  // that the internal algorithms should use their default fuzzy value, if none was specified
  // by the user.
  real(FUZZY_PARAM_ID())->setValue(1.e-8);

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), OBJECT_LIST_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TOOL_LIST_ID());
}

//=================================================================================================
FeaturesPlugin_Boolean::OperationType FeaturesPlugin_Boolean::operationType()
{
  return myOperationType;
}

//=================================================================================================
void FeaturesPlugin_Boolean::loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                          const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                          const ListOfShape& theTools,
                                          const std::shared_ptr<GeomAPI_Shape> theResultShape,
                                          const GeomMakeShapePtr& theMakeShape)
{
  //load result
  if(theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape, false);
    return;
  }

  theResultBody->storeModified(theBaseShape, theResultShape);

  theResultBody->loadModifiedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::EDGE);
  theResultBody->loadModifiedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::FACE);

  theResultBody->loadDeletedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::FACE);

  for (ListOfShape::const_iterator anIter = theTools.begin();
       anIter != theTools.end();
       ++anIter)
  {
    GeomAPI_Shape::ShapeType aShapeType =
      (*anIter)->shapeType() <= GeomAPI_Shape::FACE ? GeomAPI_Shape::FACE
                                                    : GeomAPI_Shape::EDGE;
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, aShapeType);

    theResultBody->loadDeletedShapes(theMakeShape, *anIter, GeomAPI_Shape::FACE);
  }
}

//=================================================================================================
void FeaturesPlugin_Boolean::storeResult(
    const ListOfShape& theObjects,
    const ListOfShape& theTools,
    const GeomShapePtr theResultShape,
    int& theResultIndex,
    std::shared_ptr<GeomAlgoAPI_MakeShapeList> theMakeShapeList,
    std::vector<ModelAPI_Tools::ResultBaseAlgo>& theResultBaseAlgoList)
{
  if (!theResultShape)
    return;

  std::shared_ptr<ModelAPI_ResultBody> aResultBody =
      document()->createBody(data(), theResultIndex);

  ModelAPI_Tools::loadModifiedShapes(aResultBody,
                                     theObjects,
                                     theTools,
                                     theMakeShapeList,
                                     theResultShape);
  setResult(aResultBody, theResultIndex++);

  // merge algorithms
  ModelAPI_Tools::ResultBaseAlgo aRBA;
  aRBA.resultBody = aResultBody;
  aRBA.baseShape = theObjects.front();
  for (std::vector<ModelAPI_Tools::ResultBaseAlgo>::iterator
       aRBAIt = theResultBaseAlgoList.begin();
       aRBAIt != theResultBaseAlgoList.end(); ++aRBAIt) {
    theMakeShapeList->appendAlgo(aRBAIt->makeShape);
  }
  aRBA.makeShape = theMakeShapeList;
  theResultBaseAlgoList.clear();
  theResultBaseAlgoList.push_back(aRBA);
}
