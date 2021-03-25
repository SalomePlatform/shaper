// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_VersionedChFi.h"
#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_MakeShapeList.h>

#include <GeomAPI_ShapeExplorer.h>


static const std::string CHAMFERFILLET_VERSION_1("v9.5");


// Obtain all sub-shapes from the shape and append them to the list
static void collectSubs(const GeomShapePtr& theShape,
                              ListOfShape& theSubs,
                        const GeomAPI_Shape::ShapeType theShapeType)
{
  GeomAPI_ShapeExplorer anExp(theShape, theShapeType);
  for (; anExp.more(); anExp.next()) {
    GeomShapePtr aShape = anExp.current();
    // Store all shapes with FORWARD orientation to avoid duplication of shared edges/vertices
    aShape->setOrientation(GeomAPI_Shape::FORWARD);
    theSubs.push_back(aShape);
  }
}


void FeaturesPlugin_VersionedChFi::initVersion(const AttributePtr& theObjectsAttr)
{
  if (!theObjectsAttr->isInitialized()) {
    // new feature, not read from file
    data()->setVersion(CHAMFERFILLET_VERSION_1);
  }
}

void FeaturesPlugin_VersionedChFi::execute()
{
  GeomAPI_ShapeHierarchy anObjectHierarchy;
  if (!processAttribute(objectsAttribute(), anObjectHierarchy))
    return;

  // Perform the operation
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);
  int aResultIndex = 0;
  std::string anError;

  ListOfShape anEdges;
  ListOfShape anOriginalSolids;
  for (GeomAPI_ShapeHierarchy::iterator anIt = anObjectHierarchy.begin();
       anIt != anObjectHierarchy.end(); ++anIt) {
    GeomShapePtr aSolid = anObjectHierarchy.parent(*anIt);
    const ListOfShape& aSubs = anObjectHierarchy.objects(aSolid);

    std::shared_ptr<GeomAlgoAPI_MakeShape> anAlgo = performOperation(aSolid, aSubs);
    if (!anAlgo)
      return; // something gone wrong

    anObjectHierarchy.markModified(aSolid, anAlgo->shape());
    aMakeShapeList->appendAlgo(anAlgo);

    anOriginalSolids.push_back(aSolid);
    anEdges.insert(anEdges.end(), aSubs.begin(), aSubs.end());
  }

  // Build results of the operaion.
  const std::string& aPrefix = modifiedShapePrefix();
  ListOfShape aTopLevel;
  anObjectHierarchy.topLevelObjects(aTopLevel);
  for (ListOfShape::iterator anIt = aTopLevel.begin(); anIt != aTopLevel.end(); ++anIt) {
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, anOriginalSolids, ListOfShape(),
                                             aMakeShapeList, *anIt, aPrefix);
    setResult(aResultBody, aResultIndex++);

    for (ListOfShape::iterator aEIt = anEdges.begin(); aEIt != anEdges.end(); ++aEIt) {
      GeomShapePtr aBase = *aEIt;
      // Store new faces generated from edges and vertices
      aResultBody->loadGeneratedShapes(aMakeShapeList, aBase, GeomAPI_Shape::EDGE, aPrefix, true);
    }

    FeaturesPlugin_Tools::loadDeletedShapes(aResultBody, GeomShapePtr(), anOriginalSolids,
                                            aMakeShapeList, *anIt);
  }

  removeResults(aResultIndex);
}


bool FeaturesPlugin_VersionedChFi::processAttribute(const AttributePtr& theAttribute,
                                                    GeomAPI_ShapeHierarchy& theObjects)
{
  bool isStoreFullHierarchy = data()->version() == CHAMFERFILLET_VERSION_1;

  AttributeSelectionListPtr anObjectsSelList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); ++anObjectsIndex) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    GeomShapePtr anObject = anObjectAttr->value();
    if (!anObject)
      return false;

    ResultPtr aContext = anObjectAttr->context();
    GeomShapePtr aParent;
    if (aContext.get()) {
      ResultBodyPtr aCtxOwner = ModelAPI_Tools::bodyOwner(aContext);
      if (aCtxOwner && aCtxOwner->shape()->shapeType() == GeomAPI_Shape::COMPSOLID)
        aContext = aCtxOwner;
      aParent = aContext->shape();
      if (!aParent)
        return false;

      // store full shape hierarchy for the corresponding version only
      theObjects.addObject(anObject);
      theObjects.addParent(anObject, aParent);
      if (isStoreFullHierarchy)
        ModelAPI_Tools::fillShapeHierarchy(aParent, aContext, theObjects);
    } else { // get it from a feature
      FeaturePtr aFeature = anObjectAttr->contextFeature();
      if (aFeature.get()) {
        aParent = aFeature->firstResult()->shape();
        if (!aParent)
          return false;

        ListOfShape anEdges;
        collectSubs(aParent, anEdges, GeomAPI_Shape::EDGE);
        for (ListOfShape::iterator anIt = anEdges.begin(); anIt != anEdges.end(); ++anIt) {
          theObjects.addObject(*anIt);
          theObjects.addParent(*anIt, aParent);
        }
      }
    }
  }
  return true;
}
