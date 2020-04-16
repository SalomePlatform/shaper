// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_Rotation_H_
#define FeaturesPlugin_Rotation_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>

class GeomAPI_Trsf;

/// \class FeaturesPlugin_Rotation
/// \ingroup Plugins
/// \brief Feature for rotation objects around the axis.
class FeaturesPlugin_Rotation : public ModelAPI_Feature
{
 public:
  /// Rotation kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ROTATION_ID("Rotation");
    return MY_ROTATION_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByAxisAndAngle".
  inline static const std::string& CREATION_METHOD_BY_ANGLE()
  {
    static const std::string MY_CREATION_METHOD_ID("ByAxisAndAngle");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByThreePoints".
  inline static const std::string& CREATION_METHOD_BY_THREE_POINTS()
  {
    static const std::string MY_CREATION_METHOD_ID("ByThreePoints");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of referenced objects.
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("main_objects");
    return MY_OBJECTS_LIST_ID;
  }

  /// Attribute name of an axis.
  inline static const std::string& AXIS_OBJECT_ID()
  {
    static const std::string MY_AXIS_OBJECT_ID("axis_object");
    return MY_AXIS_OBJECT_ID;
  }

  /// Attribute name of angle.
  inline static const std::string& ANGLE_ID()
  {
    static const std::string MY_ANGLE_ID("angle");
    return MY_ANGLE_ID;
  }

  /// Attribute name of a center point.
  inline static const std::string& CENTER_POINT_ID()
  {
    static const std::string MY_CENTER_POINT_ID("center_point");
    return MY_CENTER_POINT_ID;
  }

  /// Attribute name of a center point.
  inline static const std::string& START_POINT_ID()
  {
    static const std::string MY_START_POINT_ID("start_point");
    return MY_START_POINT_ID;
  }

  /// Attribute name of a center point.
  inline static const std::string& END_POINT_ID()
  {
    static const std::string MY_END_POINT_ID("end_point");
    return MY_END_POINT_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Rotation::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Rotation();

private:
  /// Calculate the rotation using an axis and an angle.
  std::shared_ptr<GeomAPI_Trsf> rotationByAxisAndAngle();

  /// Calculate the rotation using a center and two points.
  std::shared_ptr<GeomAPI_Trsf> rotationByThreePoints();

  /// Perform the transformation
  void performRotation(const std::shared_ptr<GeomAPI_Trsf>& theTrsf);
};

#endif
