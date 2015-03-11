// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Axis.h
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#ifndef ConstructionPlugin_Axis_H
#define ConstructionPlugin_Axis_H

#include "ConstructionPlugin.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <GeomAPI_ICustomPrs.h>

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
    static std::string MY_KIND = ConstructionPlugin_Axis::ID();
    return MY_KIND;
  }

  /// Axis kind
  inline static const std::string& ID()
  {
    static const std::string CONSTRUCTION_AXIS_KIND("Axis");
    return CONSTRUCTION_AXIS_KIND;
  }

  /// attribute name for first point
  inline static const std::string& METHOD()
  {
    static const std::string METHOD_ATTR("creationMethod");
    return METHOD_ATTR;
  }
  /// attribute name for first point
  inline static const std::string& POINT_FIRST()
  {
    static const std::string POINT_ATTR_FIRST("firstPoint");
    return POINT_ATTR_FIRST;
  }
  /// attribute name for second point
  inline static const std::string& POINT_SECOND()
  {
    static const std::string POINT_ATTR_SECOND("secondPoint");
    return POINT_ATTR_SECOND;
  }
  /// attribute name for second point
  inline static const std::string& CYLINDRICAL_FACE()
  {
    static const std::string CYLINDRICAL_FACE_ATTR("cylindricalFace");
    return CYLINDRICAL_FACE_ATTR;
  }

  inline static const double MINIMAL_LENGTH() { return 1.e-5; }

  /// Creates a new part document if needed
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Construction result is allways recomuted on the fly
  CONSTRUCTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  ConstructionPlugin_Axis();

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs);

 protected:
  void createAxisByTwoPoints();
};


#endif
