// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSetPlugin_Duplicate.h
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Duplicate_H_
#define PartSetPlugin_Duplicate_H_

#include "PartSetPlugin.h"
#include <ModelAPI_Feature.h>

/**\class PartSetPlugin_Duplicate
 * \ingroup Plugins
 * \brief Duplicates the active part (not root). Creates a new "part" feature.
 */
class PartSetPlugin_Duplicate : public ModelAPI_Feature
{
 public:
 
  /// Duplicate kind
  inline static const std::string& ID()
  {
    static const std::string MY_REMOVE_KIND("Duplicate");
    return MY_REMOVE_KIND;
  }
  /// Returns the kind of a feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PartSetPlugin_Duplicate::ID();
    return MY_KIND;
  }

  /// Returns to which group in the document must be added feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getGroup()
  {
    static std::string MY_GROUP = "Parts";
    return MY_GROUP;
  }

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes()
  {
  }

  /// Not normal feature that stored in the tree
  PARTSETPLUGIN_EXPORT virtual bool isAction()
  {
    return true;
  }

  /// Performs the "duplicate"
  PARTSETPLUGIN_EXPORT virtual void execute();

  /// Use plugin manager for features creation
  PartSetPlugin_Duplicate()
  {
  }

};

#endif
