// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ConstructionPlugin_Point_H_
#define ConstructionPlugin_Point_H_

#include "ConstructionPlugin.h"

#include <GeomAPI_ICustomPrs.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

#include <math.h>

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

  /// Attribute name for creation method.
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
  inline static const std::string& CREATION_METHOD_BY_INTERSECTION()
  {
    static const std::string MY_CREATION_METHOD_ID("by_intersection");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_GEOMETRICAL_PROPERTY()
  {
    static const std::string MY_CREATION_METHOD_ID("by_geometrical_property");
    return MY_CREATION_METHOD_ID;
  }

  inline static const std::string& POINT3D()
  {
    static const std::string POINT_ATTR("point3d");
    return POINT_ATTR;
  }

  /// Attribute name for selected edge.
  inline static const std::string& EDGE()
  {
    static const std::string ATTR_ID("edge");
    return ATTR_ID;
  }

  /// Attribute name for offset type on selected edge.
  inline static const std::string& OFFSET_TYPE()
  {
    static const std::string ATTR_ID("offset_type");
    return ATTR_ID;
  }

  /// Attribute name for offset type by distance.
  inline static const std::string& OFFSET_TYPE_BY_DISTANCE()
  {
    static const std::string ATTR_ID("offset_type_by_distance");
    return ATTR_ID;
  }

  /// Attribute name for offset type by ratio.
  inline static const std::string& OFFSET_TYPE_BY_RATIO()
  {
    static const std::string ATTR_ID("offset_type_by_ratio");
    return ATTR_ID;
  }

  /// Attribute name for distance.
  inline static const std::string& DISTANCE()
  {
    static const std::string ATTR_ID("distance");
    return ATTR_ID;
  }

  /// Attribute name for percent flag.
  inline static const std::string& RATIO()
  {
    static const std::string ATTR_ID("ratio");
    return ATTR_ID;
  }

  /// Attribute name for reverse flag.
  inline static const std::string& REVERSE()
  {
    static const std::string ATTR_ID("reverse");
    return ATTR_ID;
  }

  /// Attribute name for point to projection.
  inline static const std::string& POINT_TO_PROJECT()
  {
    static const std::string ATTR_ID("point_to_project");
    return ATTR_ID;
  }

  /// Attribute name for projection type.
  inline static const std::string& PROJECTION_TYPE()
  {
    static const std::string ATTR_ID("projection_type");
    return ATTR_ID;
  }

  /// Attribute name for projection type on edge.
  inline static const std::string& PROJECTION_TYPE_ON_EDGE()
  {
    static const std::string ATTR_ID("projection_type_on_edge");
    return ATTR_ID;
  }

  /// Attribute name for projection type on face.
  inline static const std::string& PROJECTION_TYPE_ON_FACE()
  {
    static const std::string ATTR_ID("projection_type_on_face");
    return ATTR_ID;
  }

  /// Attribute name for edge for point projection.
  inline static const std::string& EDGE_FOR_POINT_PROJECTION()
  {
    static const std::string ATTR_ID("edge_for_point_projection");
    return ATTR_ID;
  }

  /// Attribute name for face for point projection.
  inline static const std::string& FACE_FOR_POINT_PROJECTION()
  {
    static const std::string ATTR_ID("face_for_point_projection");
    return ATTR_ID;
  }

  /// Attribute name for intersection type.
  inline static const std::string& INTERSECTION_TYPE()
  {
    static const std::string ATTR_ID("intersection_type");
    return ATTR_ID;
  }

  /// Attribute name for intersection type by lines.
  inline static const std::string& INTERSECTION_TYPE_BY_LINES()
  {
    static const std::string MY_CREATION_METHOD_ID("intersection_type_by_lines");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for intersection type by line and plane.
  inline static const std::string& INTERSECTION_TYPE_BY_LINE_AND_PLANE()
  {
    static const std::string MY_CREATION_METHOD_ID("intersection_type_by_line_and_plane");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for intersection type by planes.
  inline static const std::string& INTERSECTION_TYPE_BY_PLANES()
  {
    static const std::string MY_CREATION_METHOD_ID("intersection_type_by_planes");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for selected first intersection line.
  inline static const std::string& INTERSECTION_LINE_1()
  {
    static const std::string ATTR_ID("intersection_line_1");
    return ATTR_ID;
  }

  /// Attribute name for selected second intersection line.
  inline static const std::string& INTERSECTION_LINE_2()
  {
    static const std::string ATTR_ID("intersection_line_2");
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
  }

  /// Attribute name for use offset for the intersection plane.
  inline static const std::string& USE_OFFSET()
  {
    static const std::string ATTR_ID("use_offset");
    return ATTR_ID;
  }

  /// Attribute name for offset for the intersection plane.
  inline static const std::string& OFFSET()
  {
    static const std::string ATTR_ID("offset");
    return ATTR_ID;
  }

  /// Attribute name for reverse offset for the intersection plane.
  inline static const std::string& REVERSE_OFFSET()
  {
    static const std::string ATTR_ID("reverse_offset");
    return ATTR_ID;
  }

  /// Attribute name for selected intersection plane.
  inline static const std::string& INTERSECTION_PLANE_1()
  {
    static const std::string ATTR_ID("intersection_plane_1");
    return ATTR_ID;
  }

  /// Attribute name for selected intersection plane.
  inline static const std::string& INTERSECTION_PLANE_2()
  {
    static const std::string ATTR_ID("intersection_plane_2");
    return ATTR_ID;
  }

  /// Attribute name for selected intersection plane.
  inline static const std::string& INTERSECTION_PLANE_3()
  {
    static const std::string ATTR_ID("intersection_plane_3");
    return ATTR_ID;
  }

  /// Attribute name for property type.
  inline static const std::string& GEOMETRICAL_PROPERTY_TYPE()
  {
    static const std::string ATTR_ID("geometrical_property_type");
    return ATTR_ID;
  }

  /// Attribute name for property type by center of gravity.
  inline static const std::string& GEOMETRICAL_PROPERTY_TYPE_BY_CENTER_OF_GRAVITY()
  {
    static const std::string PROPERTY_TYPE("geometrical_property_type_by_center_of_gravity");
    return PROPERTY_TYPE;
  }

  /// Attribute name for property type by center of circle.
  inline static const std::string& GEOMETRICAL_PROPERTY_TYPE_BY_CENTER_OF_CIRCLE()
  {
    static const std::string PROPERTY_TYPE("geometrical_property_type_by_center_of_circle");
    return PROPERTY_TYPE;
  }

  /// Attribute name for selected object for center of gravity.
  inline static const std::string& OBJECT_FOR_CENTER_OF_GRAVITY()
  {
    static const std::string ATTR_ID("object_for_center_of_gravity");
    return ATTR_ID;
  }

  /// Attribute name for selected object for center of cricle.
  inline static const std::string& OBJECT_FOR_CENTER_OF_CIRCLE()
  {
    static const std::string ATTR_ID("object_for_center_of_circle");
    return ATTR_ID;
  }

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
  std::shared_ptr<GeomAPI_Vertex> createByDistanceOnEdge();
  std::shared_ptr<GeomAPI_Vertex> createByProjectionOnEdge();
  std::shared_ptr<GeomAPI_Vertex> createByProjectionOnFace();
  std::shared_ptr<GeomAPI_Vertex> createByLinesIntersection();
  std::list<std::shared_ptr<GeomAPI_Vertex> > createByLineAndPlaneIntersection();
  std::shared_ptr<GeomAPI_Vertex> createByPlanesIntersection();
  std::shared_ptr<GeomAPI_Vertex> createByCenterOfGravity();
  std::shared_ptr<GeomAPI_Vertex> createByCenterOfCircle();
};

#endif
