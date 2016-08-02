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

/// \class ConstructionPlugin_Plane
/// \ingroup Plugins
/// \brief Feature for creation of the new planar surface in PartSet.
class ConstructionPlugin_Plane: public ModelAPI_Feature, public GeomAPI_ICustomPrs
{
public:
  /// \return the kind of a feature.
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = ConstructionPlugin_Plane::ID();
    return MY_KIND;
  }

  /// Default color for a plane.
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string CONSTRUCTION_PLANE_COLOR("150,150,180");
    return CONSTRUCTION_PLANE_COLOR;
  }

  /// Plane kind.
  inline static const std::string& ID()
  {
    static const std::string CONSTRUCTION_PLANE_KIND("Plane");
    return CONSTRUCTION_PLANE_KIND;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("creation_method");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_GENERAL_EQUATION()
  {
    static const std::string MY_CREATION_METHOD_ID("by_general_equation");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_THREE_POINTS()
  {
    static const std::string MY_CREATION_METHOD_ID("by_three_points");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_LINE_AND_POINT()
  {
    static const std::string MY_CREATION_METHOD_ID("by_line_and_point");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_OTHER_PLANE()
  {
    static const std::string MY_CREATION_METHOD_ID("by_other_plane");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_OTHER_PLANE_OPTION()
  {
    static const std::string MY_CREATION_METHOD_ID("by_other_plane_option");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_DISTANCE_FROM_OTHER()
  {
    static const std::string MY_CREATION_METHOD_ID("by_distance_from_other");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_COINCIDENT_TO_POINT()
  {
    static const std::string MY_CREATION_METHOD_ID("by_coincident_to_point");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_ROTATION()
  {
    static const std::string MY_CREATION_METHOD_ID("by_rotation");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_TWO_PARALLEL_PLANES()
  {
    static const std::string MY_CREATION_METHOD_ID("by_two_parallel_planes");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for first point.
  inline static const std::string& POINT1()
  {
    static const std::string ATTR_ID("point1");
    return ATTR_ID;
  }

  /// Attribute name for second point.
  inline static const std::string& POINT2()
  {
    static const std::string ATTR_ID("point2");
    return ATTR_ID;
  }

  /// Attribute name for third point.
  inline static const std::string& POINT3()
  {
    static const std::string ATTR_ID("point3");
    return ATTR_ID;
  }

  /// Attribute name for line.
  inline static const std::string& LINE()
  {
    static const std::string ATTR_ID("line");
    return ATTR_ID;
  }

  /// Attribute name for point.
  inline static const std::string& POINT()
  {
    static const std::string ATTR_ID("point");
    return ATTR_ID;
  }

  /// Attribute name for perpendicular flag.
  inline static const std::string& PERPENDICULAR()
  {
    static const std::string ATTR_ID("perpendicular");
    return ATTR_ID;
  }

  /// Attribute name for selected plane.
  inline static const std::string& PLANE()
  {
    static const std::string ATTR_ID("plane");
    return ATTR_ID;
  }

  /// Attribute name for distance.
  inline static const std::string& DISTANCE()
  {
    static const std::string ATTR_ID("distance");
    return ATTR_ID;
  }

  /// Attribute name for reverse flag.
  inline static const std::string& REVERSE()
  {
    static const std::string ATTR_ID("reverse");
    return ATTR_ID;
  }

  /// Attribute name for coincident point.
  inline static const std::string& COINCIDENT_POINT()
  {
    static const std::string ATTR_ID("coincident_point");
    return ATTR_ID;
  }

  /// Attribute name for axis.
  inline static const std::string& AXIS()
  {
    static const std::string ATTR_ID("axis");
    return ATTR_ID;
  }

  /// Attribute name for angle.
  inline static const std::string& ANGLE()
  {
    static const std::string ATTR_ID("angle");
    return ATTR_ID;
  }

  inline static const std::string& PLANE1()
  {
    static const std::string ATTR_ID("plane1");
    return ATTR_ID;
  }

  inline static const std::string& PLANE2()
  {
    static const std::string ATTR_ID("plane2");
    return ATTR_ID;
  }


  /// Attribute name for a parameter for the general equation of a plane (ax+by+cz+d=0)
  inline static const std::string& A()
  {
    static const std::string ATTR_ID("A");
    return ATTR_ID;
  }
  /// Attribute name for b parameter for the general equation of a plane (ax+by+cz+d=0)
  inline static const std::string& B()
  {
    static const std::string ATTR_ID("B");
    return ATTR_ID;
  }
  /// Attribute name for c parameter for the general equation of a plane (ax+by+cz+d=0)
  inline static const std::string& C()
  {
    static const std::string ATTR_ID("C");
    return ATTR_ID;
  }
  /// Attribute name for d parameter for the general equation of a plane (ax+by+cz+d=0)
  inline static const std::string& D()
  {
    static const std::string ATTR_ID("D");
    return ATTR_ID;
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
  /// Creates a new plane by general equation.
  std::shared_ptr<GeomAPI_Shape> createByGeneralEquation();
  std::shared_ptr<GeomAPI_Shape> createByThreePoints();
  std::shared_ptr<GeomAPI_Shape> createByLineAndPoint();
  std::shared_ptr<GeomAPI_Shape> createByCoincidentPoint();
  std::shared_ptr<GeomAPI_Shape> createByRotation();
  std::shared_ptr<GeomAPI_Shape> createByTwoParallelPlanes();
  /// Creates a new plane by copy of face plane with translation along the normal
  /// to the specified distance.
  std::shared_ptr<GeomAPI_Shape> createByDistanceFromOther();
};

#endif
