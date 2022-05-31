// Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

#include <FeaturesPlugin_Defeaturing.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Defeaturing.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Tools.h>

#include <unordered_map>


FeaturesPlugin_Defeaturing::FeaturesPlugin_Defeaturing()
{
}

void FeaturesPlugin_Defeaturing::initAttributes()
{
  data()->addAttribute(OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
}


void FeaturesPlugin_Defeaturing::execute()
{
  typedef std::unordered_map<GeomShapePtr, ListOfShape,
                             GeomAPI_Shape::Hash, GeomAPI_Shape::Equal> SolidFaces;
  SolidFaces aBodiesAndFacesToRemove;

  // getting objects and sort them according to parent solids
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECT_LIST_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); ++anObjectsIndex) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    GeomShapePtr anObject = anObjectAttr->value();
    if (!anObject)
      return;

    ResultPtr aContext = anObjectAttr->context();
    if (!aContext.get())
      return;

    ResultBodyPtr aCtxOwner = ModelAPI_Tools::bodyOwner(aContext, true);
    GeomShapePtr aParent = aCtxOwner ? aCtxOwner->shape() : aContext->shape();
    aBodiesAndFacesToRemove[aParent].push_back(anObject);
  }

  // Perform Defeaturing algorithm
  GeomAlgoAPI_MakeShapeList aMakeShapeList;
  std::shared_ptr<GeomAlgoAPI_Defeaturing> anAlgo;
  int aResultIndex = 0;
  std::string anError;

  std::vector<ModelAPI_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape anOriginalShapesList, aResultShapesList;

  for (SolidFaces::iterator anIt = aBodiesAndFacesToRemove.begin();
       anIt != aBodiesAndFacesToRemove.end(); ++anIt) {
    GeomShapePtr aParent = anIt->first;
    anAlgo.reset(new GeomAlgoAPI_Defeaturing(aParent, anIt->second));
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(anAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    GeomShapePtr aResult = anAlgo->shape();
    ListOfShape aBaseShapes;
    for (GeomAPI_ShapeExplorer anExp(aParent, GeomAPI_Shape::SOLID); anExp.more(); anExp.next())
      aBaseShapes.push_back(anExp.current());

    std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
    ModelAPI_Tools::loadModifiedShapes(aResultBody, aBaseShapes, ListOfShape(),
                                       anAlgo, aResult, "Defeaturing");

    setResult(aResultBody, aResultIndex);
    aResultIndex++;

    ModelAPI_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = aParent;
    aRBA.makeShape = anAlgo;
    aResultBaseAlgoList.push_back(aRBA);
    aResultShapesList.push_back(aResult);
    anOriginalShapesList.insert(anOriginalShapesList.end(), aBaseShapes.begin(), aBaseShapes.end());
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  ModelAPI_Tools::loadDeletedShapes(aResultBaseAlgoList,
      anOriginalShapesList, aResultShapesCompound);

  removeResults(aResultIndex);
}
