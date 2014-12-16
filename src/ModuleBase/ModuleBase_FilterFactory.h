// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterFactory.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_FilterFactory_H
#define ModeleBase_SelectionFilterFactory_H

#include "ModuleBase.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_Filter.h"

#include <SelectMgr_ListOfFilter.hxx>

#include <map>
#include <set>

/**
 * Allows to get a selection filter by the feature identifier and 
 * the attribute identifier (if attribute is validated).
 * All accessible filters must be registered by the ID string first.
 * The instance of this factory can be get in the Workshop interface.
 * Keeps the filter objects alive and just returns one of it by request.
 * All the needed information is provided to the filter as an argument,
 * this allows to work with them independently from the feature specific object.
 */
class ModuleBase_FilterFactory : public QObject
{
 private:
  std::map<std::string, ModuleBase_Filter*> myIDs;  ///< map from ID to registered filter
  /// filters IDs to list of arguments
  typedef std::map<std::string, std::list<std::string> > AttrFilters;
  /// filters IDs by feature ID
  std::map<std::string, AttrFilters> myFeatures;
  /// filters IDs and arguments by feature and attribute IDs
  std::map<std::string, std::map<std::string, AttrFilters> > myAttrs;

 public:
  /// Registers the instance of the filter by the ID
  MODULEBASE_EXPORT virtual void registerFilter(const std::string& theID,
                                                ModuleBase_Filter* theFilter);

  /// Assigns filter to the attribute of the feature
  MODULEBASE_EXPORT virtual void assignFilter(const std::string& theID,
                                              const std::string& theFeatureID,
                                              const std::string& theAttrID,
                                              const std::list<std::string>& theArguments);

  /// Provides a filter for the attribute, returns NULL if no filter
  MODULEBASE_EXPORT void filters(const std::string& theFeatureID,
                                 const std::string& theAttrID,
                                 SelectMgr_ListOfFilter& theFilters) const;

  /// Returns registered filter by its Id
  MODULEBASE_EXPORT virtual const ModuleBase_Filter* filter(const std::string& theID) const;

protected:
  ModuleBase_FilterFactory();

  ~ModuleBase_FilterFactory() {}

  friend class ModuleBase_IWorkshop;

};

#endif //ModuleBase_FilterFactory
