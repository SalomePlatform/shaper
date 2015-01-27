// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Group.h
// Created:     08 Oct 2014
// Author:      Sergey BELASH

#ifndef FEATURESPLUGIN_GROUP_H_
#define FEATURESPLUGIN_GROUP_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>

/**\class FeaturesPlugin_Group
 * \ingroup Plugins
 * \brief Feature for selection of sub-shapes of some shapes.
 *
 * All selected sub-shapes must be of equal type (vertex, edge, face, etc) but may
 * be selected on different objects.
 */
class FeaturesPlugin_Group : public ModelAPI_Feature
{
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_GROUP_ID("Group");
    return MY_GROUP_ID;
  }
  /// attribute name of selected entities list
  inline static const std::string& LIST_ID()
  {
    static const std::string MY_GROUP_LIST_ID("group_list");
    return MY_GROUP_LIST_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Group::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Result of groups is created on the fly and don't stored to the document
  FEATURESPLUGIN_EXPORT virtual bool isPersistentResult() {return true;}

  /// Use plugin manager for features creation
  FeaturesPlugin_Group();

};

#endif
