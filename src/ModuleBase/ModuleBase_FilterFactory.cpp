// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterFactory.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_FilterFactory.h"
#include <ModuleBase_Filter.h>

//#include <Model_FeatureValidator.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
//#include <ModelAPI_AttributeValidator.h>
#include <Events_Error.h>


void ModuleBase_FilterFactory::registerFilter(const std::string& theID,
                                              ModuleBase_Filter* theFilter)
{
  if (myIDs.find(theID) != myIDs.end()) {
    Events_Error::send(std::string("Filter ") + theID + " is already registered");
  } else {
    myIDs[theID] = theFilter;
  }
}

void ModuleBase_FilterFactory::assignFilter(const std::string& theID,
                                            const std::string& theFeatureID,
                                            const std::string& theAttrID,
                                            const std::list<std::string>& theArguments)
{
  // create feature-structures if not exist
  std::map<std::string, std::map<std::string, AttrFilters> >::iterator aFeature = myAttrs.find(
    theFeatureID);
  if (aFeature == myAttrs.end()) {
    myAttrs[theFeatureID] = std::map<std::string, AttrFilters>();
    aFeature = myAttrs.find(theFeatureID);
  }
  // add attr-structure if not exist, or generate error if already exist
  std::map<std::string, AttrFilters>::iterator anAttr = aFeature->second.find(theAttrID);
  if (anAttr == aFeature->second.end()) {
    aFeature->second[theAttrID] = AttrFilters();
  }
  aFeature->second[theAttrID][theID] = theArguments;
}

void ModuleBase_FilterFactory::filters(const std::string& theFeatureID,
                                       const std::string& theAttrID,
                                       SelectMgr_ListOfFilter& theFilters/*,
                                        std::list<ModuleBase_Filter*>& theFilters/*,
                                        std::list<std::list<std::string> >& theArguments*/) const
{
  SelectMgr_ListOfFilter aFilters;

  std::map<std::string, std::map<std::string, AttrFilters> >::const_iterator aFeature = 
    myAttrs.find(theFeatureID);
  if (aFeature != myAttrs.cend()) {
    std::map<std::string, AttrFilters>::const_iterator anAttr = aFeature->second.find(theAttrID);
    if (anAttr != aFeature->second.end()) {
      AttrFilters::const_iterator aValIter = anAttr->second.cbegin();
      for (; aValIter != anAttr->second.cend(); aValIter++) {
        std::map<std::string, ModuleBase_Filter*>::const_iterator aFound = myIDs.find(
          aValIter->first);
        if (aFound == myIDs.end()) {
          Events_Error::send(std::string("Filter ") + aValIter->first + " was not registered");
        } else {
          ModuleBase_Filter* aFilter = aFound->second;
          aFilter->setArguments(aValIter->second);
          //if (aFilter.IsNull())
          //  continue;

          theFilters.Append(aFilter->getFilter());//aFound->second);
          //theArguments.push_back(aValIter->second);
        }
      }
    }
  }
}

ModuleBase_FilterFactory::ModuleBase_FilterFactory()
{
  //const static std::string kDefaultId = "Model_FeatureFilter";
  //registerFilter(kDefaultId, new Model_FeatureFilter);
}

const ModuleBase_Filter* ModuleBase_FilterFactory::filter(const std::string& theID) const
{
  std::map<std::string, ModuleBase_Filter*>::const_iterator aIt = myIDs.find(theID);
  if (aIt != myIDs.end()) {
    return aIt->second;
  }
  return NULL;
}
