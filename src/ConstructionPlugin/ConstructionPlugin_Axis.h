// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Axis.h
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#ifndef ConstructionPlugin_Axis_H
#define ConstructionPlugin_Axis_H

#include "ConstructionPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_ICustomPrs.h>


/// Point kind
const std::string CONSTRUCTION_AXIS_KIND("Axis");

/// attribute name for first point
const std::string POINT_ATTR_FIRST = "firstPoint";

/// attribute name for second point
const std::string POINT_ATTR_SECOND = "secondPoint";

/**\class ConstructionPlugin_Axis
 * \ingroup Plugins
 * \brief Feature for creation of the new axis in PartSet.
 */
class ConstructionPlugin_Axis : public ModelAPI_Feature, public GeomAPI_ICustomPrs
{
 public:
  /// Returns the kind of a feature
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = CONSTRUCTION_AXIS_KIND;
    return MY_KIND;
  }

  /// Creates a new part document if needed
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Construction result is allways recomuted on the fly
  CONSTRUCTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  ConstructionPlugin_Axis();

  /// Customize presentation of the feature
  virtual void customisePresentation(AISObjectPtr thePrs);
};


#endif