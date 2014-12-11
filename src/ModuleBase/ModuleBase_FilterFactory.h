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
 * Keeps the validator objects alive and just returns one of it by request.
 * All the needed information is provided to the validator as an argument,
 * this allows to work with them independently from the feature specific object.
 */
class ModuleBase_FilterFactory : public QObject
{
 public:

   //ModuleBase_FilterFactory();
   //virtual ~ModuleBase_FilterFactory() {}


 private:
  std::map<std::string, Handle_ModuleBase_Filter> myIDs;  ///< map from ID to registered validator
  /// validators IDs to list of arguments
  typedef std::map<std::string, std::list<std::string> > AttrValidators;
  /// validators IDs by feature ID
  std::map<std::string, AttrValidators> myFeatures;
  /// validators IDs and arguments by feature and attribute IDs
  std::map<std::string, std::map<std::string, AttrValidators> > myAttrs;
  /// Stores the registered attributes that leads to the concealment of referenced objects in 
  /// data tree. Map from feature kind to set of attribute IDs.
  std::map<std::string, std::set<std::string> > myConcealed;

 public:
  /// Registers the instance of the validator by the ID
  MODULEBASE_EXPORT virtual void registerFilter(const std::string& theID,
                                                Handle_ModuleBase_Filter theValidator);

  /// Assigns validator to the attribute of the feature
  MODULEBASE_EXPORT virtual void assignFilter(const std::string& theID,
                                            const std::string& theFeatureID,
                                            const std::string& theAttrID);

  /// Provides a validator for the attribute, returns NULL if no validator
  MODULEBASE_EXPORT const SelectMgr_ListOfFilter& filters(const std::string& theFeatureID,
                                       const std::string& theAttrID) const;

  /// Returns registered validator by its Id
  MODULEBASE_EXPORT virtual const Handle_ModuleBase_Filter validator(const std::string& theID) const;

  /// Returns true if feature and all its attributes are valid.
  MODULEBASE_EXPORT virtual bool validate(const std::shared_ptr<ModelAPI_Feature>& theFeature) const;

  /// register that this attribute in feature is not obligatory for the feature execution
  /// so, it is not needed for the standard validation mechanism
  virtual void registerNotObligatory(std::string theFeature, std::string theAttribute);

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);

  /// register that this attribute conceals in the object browser
  /// all referenced features after execution
  virtual void registerConcealment(std::string theFeature, std::string theAttribute);

  /// Returns true that it was registered that attribute conceals the referenced result
  virtual bool isConcealed(std::string theFeature, std::string theAttribute);

protected:
  void addDefaultValidators(std::list<Handle_ModuleBase_Filter>& theValidators) const;
  /// Get instance from workshop

  ModuleBase_FilterFactory();

  ~ModuleBase_FilterFactory() {}

  friend class ModuleBase_IWorkshop;

};

#endif //ModuleBase_FilterFactory
