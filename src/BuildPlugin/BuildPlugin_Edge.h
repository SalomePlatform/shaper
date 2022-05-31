// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef BuildPlugin_Edge_H_
#define BuildPlugin_Edge_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_Edge
/// \ingroup Plugins
/// \brief Feature for creation of edge from sketch edge or existing edges.
class BuildPlugin_Edge: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_Edge();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Edge");
    return MY_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// \return the kind of a feature.
  BUILDPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = BuildPlugin_Edge::ID();
    return MY_KIND;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("creation_method");
    return MY_CREATION_METHOD_ID;
  }

  /// Value for creation method.
  inline static const std::string& CREATION_BY_SEGMENTS()
  {
    static const std::string MY_CREATION_METHOD_ID("by_segments");
    return MY_CREATION_METHOD_ID;
  }

  /// Value for creation method.
  inline static const std::string& CREATION_BY_POINTS()
  {
    static const std::string MY_CREATION_METHOD_ID("by_points");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for the first point.
  inline static const std::string& FIRST_POINT()
  {
    static const std::string MY_FIRST_POINT_ID("first_point");
    return MY_FIRST_POINT_ID;
  }

  /// Attribute name for the second point.
  inline static const std::string& SECOND_POINT()
  {
    static const std::string MY_SECOND_POINT_ID("second_point");
    return MY_SECOND_POINT_ID;
  }

  /// Attribute name of "Compute intersections" checkbox.
  inline static const std::string& INTERSECT_ID()
  {
    static const std::string MY_INTERSECT_ID("intersect");
    return MY_INTERSECT_ID;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();

private:
  /// Build edges by the set of segments
  void edgesBySegments();
  /// Build edge by two points.
  void edgeByPoints();
};

#endif
