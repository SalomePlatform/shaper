// Copyright (C) 2014-2023  CEA, EDF
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

#include "Model_FiltersFactory.h"

#include "ModelAPI_AttributeBoolean.h"
#include "ModelAPI_AttributeSelectionList.h"
#include <Events_InfoMessage.h>

#include "GeomAPI_Edge.h"

#include <unordered_map>
#include <stack>

typedef std::unordered_map<GeomShapePtr, ResultBodyPtr,
                          GeomAPI_Shape::Hash, GeomAPI_Shape::Equal> DataMapOfShapesToResults;

void Model_FiltersFactory::registerFilter(const std::string& theID, ModelAPI_Filter* theFilter)
{
  if (myFilters.find(theID) != myFilters.end()) {
    Events_InfoMessage("Model_FiltersFactory", "Filter %1 is already registered").arg(theID).send();
  }
  else {
    myFilters[theID] = FilterPtr(theFilter);
  }
}

struct FilterArgs {
  FilterPtr myFilter;
  bool myReverse;
  std::string myFilterID;
};

/// Returns the filter ID without the filter index
static std::string pureFilterID(const std::string& theID)
{
  // remove from aPure "_" + number + "_" starting part
  if (theID.size() > 3 && theID[0] == '_') {
    int aNumDigits = 0;
    while(theID[aNumDigits + 1] < '9' && theID[aNumDigits + 1] > '0')
      aNumDigits++;
    if (aNumDigits && theID[aNumDigits + 1] == '_') {
      return theID.substr(aNumDigits + 2);
    }
  }
  return theID;
}

static void fillMapOfShapesToResults(DataMapOfShapesToResults& theMap,
                             const ResultBodyPtr& theBodyResult,
                             const GeomAPI_Shape::ShapeType theShapeType)
{
  std::stack<ResultBodyPtr> stack;
  stack.push(theBodyResult);

  while(!stack.empty())
  {
    ResultBodyPtr aCurrBodyRes = stack.top();
    stack.pop();

    int nbSubs = aCurrBodyRes->numberOfSubs();
    if(nbSubs == 0)
    {
      GeomShapePtr aShape = aCurrBodyRes->shape();
      std::list<GeomShapePtr> aSubShapes = aShape->subShapes(theShapeType, true);
      std::list<GeomShapePtr>::const_iterator aShapesIt;
      for (aShapesIt = aSubShapes.cbegin(); aShapesIt != aSubShapes.cend(); aShapesIt++)
      {
        GeomShapePtr aSubShape = (*aShapesIt);

        // degenerated edge is not valid selection
        if (theShapeType == GeomAPI_Shape::EDGE)
          if (aSubShape->edge()->isDegenerated())
            continue;
        
        if (theMap.find(aSubShape) == theMap.end()) 
        {
          theMap.emplace(aSubShape, aCurrBodyRes);
        }
      }
    }
    else
    {
        for (int aSubIndex = 0; aSubIndex < nbSubs; aSubIndex++) 
        {
          stack.push(aCurrBodyRes->subResult(aSubIndex));
        }
    }
  }

}

bool Model_FiltersFactory::isValid(FeaturePtr theFiltersFeature,
                                   ResultPtr theResult,
                                   GeomShapePtr theShape)
{
  // check that the shape type corresponds to the attribute list type
  AttributePtr aBase =
    std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(theFiltersFeature)->baseAttribute();
  if (aBase.get()) {
    std::shared_ptr<ModelAPI_AttributeSelectionList> aList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aBase);
    std::string aStrType = aList->selectionType();
    GeomAPI_Shape::ShapeType aType = GeomAPI_Shape::shapeTypeByStr(aStrType);
    if (theShape->shapeType() != aType)
      return false;
  }
  // prepare all filters args
  ModelAPI_FiltersArgs anArgs;
  std::list<FilterArgs> aFilters; /// all filters and the reverse values

  std::list<std::string> aGroups;
  theFiltersFeature->data()->allGroups(aGroups);
  for(std::list<std::string>::iterator aGIter = aGroups.begin(); aGIter != aGroups.end(); aGIter++)
  {
    std::string aPureID = pureFilterID(*aGIter);
    if (myFilters.find(aPureID) == myFilters.end())
      continue;
    std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs;
    theFiltersFeature->data()->attributesOfGroup(*aGIter, anAttrs);
    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrIter = anAttrs.begin();
    for(; anAttrIter != anAttrs.end(); anAttrIter++) {
      std::string anArgID = (*anAttrIter)->id().substr((*aGIter).length() + 2);
      if (anArgID.empty()) { // reverse flag
        std::shared_ptr<ModelAPI_AttributeBoolean> aReverse =
          std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(*anAttrIter);
        FilterArgs aFArgs = { myFilters[aPureID] , aReverse->value() , *aGIter };
        aFilters.push_back(aFArgs);

      } else {
        anArgs.add(*anAttrIter);
      }
    }
  }

  // iterate filters and check shape for validity for all of them
  std::list<FilterArgs>::iterator aFilter = aFilters.begin();
  for(; aFilter != aFilters.end(); aFilter++) {
    anArgs.setFilter(aFilter->myFilterID);
    bool aResult = aFilter->myFilter->isOk(theShape, theResult, anArgs);

    if (aFilter->myReverse)
      aResult = !aResult;
    if (!aResult) // one filter is failed => exit immediately
      return false;
  }
  // all filters are passed
  return true;
}

std::list< std::pair<ResultPtr, GeomShapePtr> > Model_FiltersFactory::select
(const FiltersFeaturePtr& theFilterFeature,
 const GeomAPI_Shape::ShapeType theShapeType)
{
  std::list< std::pair<ResultPtr, GeomShapePtr> > aResList;

  DocumentPtr aDoc = theFilterFeature->document();
  int aNb = aDoc->size(ModelAPI_ResultBody::group());
  ObjectPtr aObj;
  ResultBodyPtr aBody;
  for (int i = 0; i < aNb; i++) 
  {
    aObj = aDoc->object(ModelAPI_ResultBody::group(), i);
    aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aObj);
    DataMapOfShapesToResults aShapeToResMap;
    fillMapOfShapesToResults(aShapeToResMap, aBody, theShapeType);

    GeomShapePtr aShape = aBody->shape();
    std::list<GeomShapePtr> aSubShapes = aShape->subShapes(theShapeType, true);
    std::list<GeomShapePtr>::const_iterator aShapesIt;
    for (aShapesIt = aSubShapes.cbegin(); aShapesIt != aSubShapes.cend(); aShapesIt++)
    {
      GeomShapePtr aSubShape = (*aShapesIt);
      // degenerated edge is not valid selection
      if (theShapeType == GeomAPI_Shape::EDGE)
        if (aSubShape->edge()->isDegenerated())
          continue;


      if (aShapeToResMap.find(aSubShape) != aShapeToResMap.end()) 
      {
        ResultBodyPtr aResBody = aShapeToResMap[aSubShape];
        if(this->isValid(theFilterFeature, aResBody, aSubShape))
        {
           std::pair<ResultPtr, GeomShapePtr> aPair(aResBody, aSubShape);
          aResList.push_back(aPair);
        }
      }
    }
  }
  return aResList;
}

/// Returns list of filters for the given shape type
/// \param theType a shape type
std::list<FilterPtr> Model_FiltersFactory::filters(GeomAPI_Shape::ShapeType theType)
{
  std::list<FilterPtr> aResult;
  std::map<std::string, FilterPtr>::const_iterator anIt;
  std::list<int> aTypes;
  std::list<int>::const_iterator aTIt;
  for (anIt = myFilters.cbegin(); anIt != myFilters.cend(); anIt++) {
    if (anIt->second->isSupported(theType))
      aResult.push_back(anIt->second);
  }
  return aResult;
}

FilterPtr Model_FiltersFactory::filter(std::string theID)
{
  std::string aPureID = pureFilterID(theID);
  std::map<std::string, FilterPtr>::iterator aFound = myFilters.find(aPureID);
  return aFound == myFilters.end() ? FilterPtr() : aFound->second;
}

std::string Model_FiltersFactory::id(FilterPtr theFilter)
{
  std::map<std::string, FilterPtr>::iterator anIter = myFilters.begin();
  for(; anIter != myFilters.end(); anIter++) {
    if (anIter->second == theFilter)
      return anIter->first;
  }
  return ""; // unknown case
}
