// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSetPlugin_Part.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Part_H_
#define PartSetPlugin_Part_H_

#include "PartSetPlugin.h"
#include <ModelAPI_CompositeFeature.h>

/**\class PartSetPlugin_Part
 * \ingroup Plugins
 * \brief Feature for creation of the new part in PartSet.
 * All sub-features are sub-elements of composite feature.
 */
class PartSetPlugin_Part : public ModelAPI_CompositeFeature
{
 public:
  /// Part kind
  inline static const std::string& ID()
  {
    static const std::string MY_PART_KIND("Part");
    return MY_PART_KIND;
  }
  /// Returns the kind of a feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PartSetPlugin_Part::ID();
    return MY_KIND;
  }

  /// Returns to which group in the document must be added feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getGroup()
  {
    static std::string MY_GROUP = "Parts";
    return MY_GROUP;
  }

  /// Creates a new part document if needed
  PARTSETPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes();

  /// Part must be added only to PartSet
  PARTSETPLUGIN_EXPORT virtual const std::string& documentToAdd();

  // composite feature methods

  /// Adds feature to its document
  virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);

  /// Returns the number of sub-features of the document
  virtual int numberOfSubs() const;

  /// Returns the sub-feature by zero-base index
  virtual std::shared_ptr<ModelAPI_Feature> subFeature(const int theIndex) const;

  /// Returns the sub-feature unique identifier in this composite feature by zero-base index
  virtual int subFeatureId(const int theIndex) const;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  virtual bool isSub(ObjectPtr theObject) const;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after)
  virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Use plugin manager for features creation
  PartSetPlugin_Part();
};

#endif
