// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_Copy.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAPI_ShapeExplorer.h>

#include <sstream>

void FeaturesPlugin_Copy::initAttributes()
{
  data()->addAttribute(OBJECTS(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(NUMBER(), ModelAPI_AttributeInteger::typeId());
}

static GeomShapePtr shapeOfSelection(AttributeSelectionPtr theSel) {
  GeomShapePtr aResult;
  FeaturePtr aSelFeature = theSel->contextFeature();
  if (aSelFeature.get()) {
    if (aSelFeature->results().empty()) // if selected feature has no results, make nothing
      return aResult;
    if (aSelFeature->results().size() == 1) { // for one sub-result don't make compound
      aResult = aSelFeature->firstResult()->shape();
    }
  }
  if (!aResult.get())
    aResult = theSel->value();
  if (!aResult.get()) {
    if (theSel->context().get())
      aResult = theSel->context()->shape();
  }
  return aResult;
}

void FeaturesPlugin_Copy::execute()
{
  int aCopiesNum = integer(NUMBER())->value();
  AttributeSelectionListPtr aList = selectionList(OBJECTS());
  int aResultIndex = 0;
  std::set<std::string> anExistingNames; // to avoid names duplication
  for(int aCopy = 0; aCopy < aCopiesNum; aCopy++) {
    for (int aSelIndex = 0; aSelIndex < aList->size(); aSelIndex++) {
      AttributeSelectionPtr aSel = aList->value(aSelIndex);
      GeomShapePtr aShape = shapeOfSelection(aSel);
      if (!aShape.get())
        continue;
      std::shared_ptr<GeomAlgoAPI_Copy> aCopyBuilder(new GeomAlgoAPI_Copy(aShape, false, false));
      std::string anError;
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCopyBuilder, getKind(), anError)) {
        setError(anError);
        return;
      }
      GeomShapePtr aResult = aCopyBuilder->shape();

      std::string aBaseName = aSel->context() ? aSel->context()->data()->name() :
        aSel->contextFeature()->firstResult()->data()->name();
      std::string aName;
      int anInd = 0;
      do {
        anInd++;
        std::ostringstream aNameStr;
        aNameStr << aBaseName << "_" << (aCopy + anInd);
        aName = aNameStr.str();
      } while (anExistingNames.count(aName));
      anExistingNames.insert(aName);

      std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);
      aResultBody->data()->setName(aName);
      // to make sub-results also names with a similar name temporarily rename the feature
      std::string anOrigName = name();
      data()->setName(aBaseName);
      aResultBody->store(aResult);
      data()->setName(anOrigName);
      aResultBody->loadFirstLevel(aResult, "Copy");
      setResult(aResultBody, aResultIndex++);
    }
  }
  removeResults(aResultIndex);
}

void FeaturesPlugin_Copy::getCopies(
  ObjectPtr theContext, std::shared_ptr<GeomAPI_Shape> theValue,
  std::list<ObjectPtr>& theCopyContext, std::list<std::shared_ptr<GeomAPI_Shape> >& theCopyVals)
{
  ResultPtr aContextRes = std::dynamic_pointer_cast<ModelAPI_Result>(theContext);
  GeomShapePtr aGroupValue = theValue.get() ? theValue : aContextRes->shape();

  AttributeSelectionListPtr aList = selectionList(OBJECTS());
  std::list<ResultPtr>::const_iterator aResIter = results().cbegin();
  while(aResIter != results().cend()) { // do as long as many iterations
    for (int aSelIndex = 0; aSelIndex < aList->size(); aSelIndex++) {
      if (aResIter == results().cend()) // no more results corresponding to the selection
        return;
      AttributeSelectionPtr aSel = aList->value(aSelIndex);
      GeomShapePtr aShape = shapeOfSelection(aSel);
      if (!aShape.get())
        continue;

      if (aShape->isSubShape(aGroupValue, false)) { // group value is subshape of copied => copy
        // search the same result in the copy by the same index of sub-shape in the shape
        GeomAPI_ShapeExplorer anOrigExp(aShape, aGroupValue->shapeType());
        GeomAPI_ShapeExplorer aCopyShape((*aResIter)->shape(), aGroupValue->shapeType());
        for(; anOrigExp.more(); anOrigExp.next(), aCopyShape.next()) {
          if (anOrigExp.current()->isSame(aGroupValue)) {
            // searching for sub-result if it is composite result, but context-not
            ResultPtr aResContext = *aResIter;
            if (aContextRes->shape()->shapeType() > (*aResIter)->shape()->shapeType()) {
              ResultBodyPtr aResBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResContext);
              if (aResBody.get()) {
                std::list<ResultPtr> aSubs;
                ModelAPI_Tools::allSubs(aResBody, aSubs, true);
                std::list<ResultPtr>::iterator aSubIter = aSubs.begin();
                for(; aSubIter != aSubs.end(); aSubIter++) {
                  GeomShapePtr aSubShape = (*aSubIter)->shape();
                  if (aSubShape.get() && aSubShape->isSubShape(aCopyShape.current(), false)) {
                    aResContext = *aSubIter;
                    break;
                  }
                }
              }
            }
            theCopyContext.push_back(aResContext);
            theCopyVals.push_back(aResContext->shape()->isSame(
              aCopyShape.current()) ? GeomShapePtr() : aCopyShape.current());
            break;
          }
        }
      }
      aResIter++;
    }
  }
}
