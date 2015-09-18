// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Point.h
// Created:     3 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ConstructionPlugin_Point_H_
#define ConstructionPlugin_Point_H_

#include "ConstructionPlugin.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <GeomAPI_ICustomPrs.h>

/**\class ConstructionPlugin_Point
 * \ingroup Plugins
 * \brief Feature for creation of the new part in PartSet.
 */
class ConstructionPlugin_Point : public ModelAPI_Feature, public GeomAPI_ICustomPrs
{
 public:
  /// Returns the kind of a feature
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind();

  /// Point kind
  inline static const std::string& ID()
  {
    static const std::string CONSTRUCTION_POINT_KIND("Point");
    return CONSTRUCTION_POINT_KIND;
  }

  /// attribute name for X coordinate
  inline static const std::string& X()
  {
    static const std::string POINT_ATTR_X("x");
    return POINT_ATTR_X;
  }
  /// attribute name for Y coordinate
  inline static const std::string& Y()
  {
    static const std::string POINT_ATTR_Y("y");
    return POINT_ATTR_Y;
  }
  /// attribute name for Z coordinate
  inline static const std::string& Z()
  {
    static const std::string POINT_ATTR_Z("z");
    return POINT_ATTR_Z;
  }

  /// Creates a new part document if needed
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Construction result is allways recomuted on the fly
  CONSTRUCTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  ConstructionPlugin_Point();

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs);
};

#endif
