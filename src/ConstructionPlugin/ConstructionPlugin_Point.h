// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ConstructionPlugin_Point_H_
#define ConstructionPlugin_Point_H_

#include "ConstructionPlugin.h"

#include <GeomAPI_ICustomPrs.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

class GeomAPI_Vertex;

/// \class ConstructionPlugin_Point
/// \ingroup Plugins
/// \brief Feature for creation of the new part in PartSet.
class ConstructionPlugin_Point: public ModelAPI_Feature, public GeomAPI_ICustomPrs
{
public:
  /// Returns the kind of a feature.
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind();

  /// Point kind.
  inline static const std::string& ID()
  {
    static const std::string CONSTRUCTION_POINT_KIND("Point");
    return CONSTRUCTION_POINT_KIND;
  }

  /*/// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("creation_method");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_XYZ()
  {
    static const std::string MY_CREATION_METHOD_ID("by_xyz");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_DISTANCE_ON_EDGE()
  {
    static const std::string MY_CREATION_METHOD_ID("by_distance_on_edge");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_PROJECTION()
  {
    static const std::string MY_CREATION_METHOD_ID("by_projection");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_LINES_INTERSECTION()
  {
    static const std::string MY_CREATION_METHOD_ID("by_lines_intersection");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_LINE_AND_PLANE_INTERSECTION()
  {
    static const std::string MY_CREATION_METHOD_ID("by_line_and_plane_intersection");
    return MY_CREATION_METHOD_ID;
  }*/

  /// Attribute name for X coordinate.
  inline static const std::string& X()
  {
    static const std::string POINT_ATTR_X("x");
    return POINT_ATTR_X;
  }

  /// Attribute name for Y coordinate.
  inline static const std::string& Y()
  {
    static const std::string POINT_ATTR_Y("y");
    return POINT_ATTR_Y;
  }

  /// Attribute name for Z coordinate.
  inline static const std::string& Z()
  {
    static const std::string POINT_ATTR_Z("z");
    return POINT_ATTR_Z;
  }

  /*/// Attribute name for selected edge.
  inline static const std::string& EDGE()
  {
    static const std::string ATTR_ID("edge");
    return ATTR_ID;
  }

  /// Attribute name for distance.
  inline static const std::string& DISTANCE_VALUE()
  {
    static const std::string ATTR_ID("value");
    return ATTR_ID;
  }

  /// Attribute name for percent flag.
  inline static const std::string& DISTANCE_PERCENT()
  {
    static const std::string ATTR_ID("percent");
    return ATTR_ID;
  }

  /// Attribute name for reverse flag.
  inline static const std::string& REVERSE()
  {
    static const std::string ATTR_ID("reverse");
    return ATTR_ID;
  }

  /// Attribute name for point.
  inline static const std::string& POINT()
  {
    static const std::string ATTR_ID("point");
    return ATTR_ID;
  }

  /// Attribute name for plane.
  inline static const std::string& PLANE()
  {
    static const std::string ATTR_ID("plane");
    return ATTR_ID;
  }

  /// Attribute name for selected first line.
  inline static const std::string& FIRST_LINE()
  {
    static const std::string ATTR_ID("first_line");
    return ATTR_ID;
  }

  /// Attribute name for selected second line.
  inline static const std::string& SECOND_LINE()
  {
    static const std::string ATTR_ID("second_line");
    return ATTR_ID;
  }

  /// Attribute name for selected intersection line.
  inline static const std::string& INTERSECTION_LINE()
  {
    static const std::string ATTR_ID("intersection_line");
    return ATTR_ID;
  }

  /// Attribute name for selected intersection plane.
  inline static const std::string& INTERSECTION_PLANE()
  {
    static const std::string ATTR_ID("intersection_plane");
    return ATTR_ID;
  }*/

  /// Creates a new part document if needed.
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Construction result is allways recomuted on the fly.
  CONSTRUCTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  ConstructionPlugin_Point();

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs);

private:
  std::shared_ptr<GeomAPI_Vertex> createByXYZ();
  /*std::shared_ptr<GeomAPI_Vertex> createByDistanceOnEdge();
  std::shared_ptr<GeomAPI_Vertex> createByProjection();
  std::shared_ptr<GeomAPI_Vertex> createByLinesIntersection();
  std::shared_ptr<GeomAPI_Vertex> createByLineAndPlaneIntersection();*/

};

#endif
