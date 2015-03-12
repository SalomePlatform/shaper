// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Plane.h
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#ifndef ConstructionPlugin_Plane_H
#define ConstructionPlugin_Plane_H

#include "ConstructionPlugin.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <GeomAPI_ICustomPrs.h>



/**\class ConstructionPlugin_Plane
 * \ingroup Plugins
 * \brief Feature for creation of the new planar surface in PartSet.
 */
class ConstructionPlugin_Plane : public ModelAPI_Feature, public GeomAPI_ICustomPrs
{
 public:
  /// Returns the kind of a feature
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = ConstructionPlugin_Plane::ID();
    return MY_KIND;
  }

  /// default color for a plane
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string CONSTRUCTION_PLANE_COLOR("#32FF32");
    return CONSTRUCTION_PLANE_COLOR;
  }

  /// Plane kind
  inline static const std::string& ID()
  {
    static const std::string CONSTRUCTION_PLANE_KIND("Plane");
    return CONSTRUCTION_PLANE_KIND;
  }

  /// attribute name for first point
  inline static const std::string& METHOD()
  {
    static const std::string METHOD_ATTR("CreationMethod");
    return METHOD_ATTR;
  }

  /// attribute name for base face
  inline static const std::string& FACE()
  {
    static const std::string FACE_ATTR("planeFace");
    return FACE_ATTR;
  }
  /// attribute name for distance
  inline static const std::string& DISTANCE()
  {
    static const std::string DISTANCE_ATTR("distance");
    return DISTANCE_ATTR;
  }

  /// the a parameter for the general equation of a plane (ax+by+cz+d=0)
  inline static const std::string& A()
  {
    static const std::string PARAM_A_ATTR("A");
    return PARAM_A_ATTR;
  }
  /// the b parameter for the general equation of a plane (ax+by+cz+d=0)
  inline static const std::string& B()
  {
    static const std::string PARAM_B_ATTR("B");
    return PARAM_B_ATTR;
  }
  /// the c parameter for the general equation of a plane (ax+by+cz+d=0)
  inline static const std::string& C()
  {
    static const std::string PARAM_C_ATTR("C");
    return PARAM_C_ATTR;
  }
  /// the d parameter for the general equation of a plane (ax+by+cz+d=0)
  inline static const std::string& D()
  {
    static const std::string PARAM_D_ATTR("D");
    return PARAM_D_ATTR;
  }

  /// Creates a new part document if needed
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Construction result is always recomputed on the fly
  CONSTRUCTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  ConstructionPlugin_Plane();

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs);

 protected:
  std::shared_ptr<GeomAPI_Shape> createPlaneByFaceAndDistance();
  std::shared_ptr<GeomAPI_Shape> createPlaneByGeneralEquation();
};

#endif
