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

#include "Model_Filter.h"

#include "ModelAPI_AttributeBoolean.h"
#include <Events_InfoMessage.h>


void Model_FiltersFactory::registerFilter(const std::string& theID, ModelAPI_Filter* theFilter)
{
  if (myFilters.find(theID) != myFilters.end()) {
    Events_InfoMessage("Model_FiltersFactory", "Filter %1 is already registered").arg(theID).send();
  }
  else {
    myFilters[theID] = FilterPtr(theFilter);
  }
}

bool Model_FiltersFactory::isValid(FeaturePtr theFiltersFeature, GeomShapePtr theShape)
{
  // prepare all filters args
  ModelAPI_FiltersArgs anArgs;
  std::map<FilterPtr, bool> aReverseFlags; /// map of all filters to the reverse values
  static const std::string& anEmptyType("");
  std::list<std::shared_ptr<ModelAPI_Attribute> > allAttrs =
    theFiltersFeature->data()->attributes(anEmptyType);
  std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = allAttrs.begin();
  for(; anAttr != allAttrs.end(); anAttr++) {
    const std::string& anAttrID = (*anAttr)->id();
    if (anAttrID.find(kFilterSeparator) == std::string::npos) { // possible a filter reverse flag
      std::shared_ptr<ModelAPI_AttributeBoolean> aReverse = theFiltersFeature->boolean(anAttrID);
      if (aReverse.get() && myFilters.find(anAttrID) != myFilters.end()) {
        aReverseFlags[myFilters[anAttrID] ] = aReverse->value();
      }
    } else { // an argument of a filter
      anArgs.add(*anAttr);
    }
  }
  // iterate filters and check shape for validity for all of them
  std::map<FilterPtr, bool>::iterator aFilter = aReverseFlags.begin();
  for(; aFilter != aReverseFlags.end(); aFilter++) {
    bool aResult = aFilter->first->isOk(theShape, anArgs);
    if (aFilter->second)
      aResult = !aResult;
    if (!aResult) // one filter is failed => exit immediately
      return false;
  }
  // all filters are passed
  return true;
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
  std::map<std::string, FilterPtr>::iterator aFound = myFilters.find(theID);
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
