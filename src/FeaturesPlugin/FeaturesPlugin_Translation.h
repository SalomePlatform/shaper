// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_Translation_H_
#define FeaturesPlugin_Translation_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>

class GeomAPI_Trsf;

/// \class FeaturesPlugin_Translation
/// \ingroup Plugins
/// \brief Feature for translation objects along the axis.
class FeaturesPlugin_Translation : public ModelAPI_Feature
{
 public:
  /// Translation kind.
  inline static const std::string& ID()
  {
    static const std::string MY_TRANSLATION_ID("Translation");
    return MY_TRANSLATION_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByAxisAndDistance".
  inline static const std::string& CREATION_METHOD_BY_DISTANCE()
  {
    static const std::string MY_CREATION_METHOD_ID("ByAxisAndDistance");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByDimensions".
  inline static const std::string& CREATION_METHOD_BY_DIMENSIONS()
  {
    static const std::string MY_CREATION_METHOD_ID("ByDimensions");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByTwoPoints".
  inline static const std::string& CREATION_METHOD_BY_TWO_POINTS()
  {
    static const std::string MY_CREATION_METHOD_ID("ByTwoPoints");
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

  /// Attribute name of distance.
  inline static const std::string& DISTANCE_ID()
  {
    static const std::string MY_DISTANCE_ID("distance");
    return MY_DISTANCE_ID;
  }

  /// Attribute name of dimension in X.
  inline static const std::string& DX_ID()
  {
    static const std::string MY_DX_ID("dx");
    return MY_DX_ID;
  }

  /// Attribute name of dimension in Y.
  inline static const std::string& DY_ID()
  {
    static const std::string MY_DY_ID("dy");
    return MY_DY_ID;
  }

  /// Attribute name of dimension in Z.
  inline static const std::string& DZ_ID()
  {
    static const std::string MY_DZ_ID("dz");
    return MY_DZ_ID;
  }

  /// Attribute name of start point.
  inline static const std::string& START_POINT_ID()
  {
    static const std::string MY_START_POINT_ID("start_point");
    return MY_START_POINT_ID;
  }

  /// Attribute name of end point.
  inline static const std::string& END_POINT_ID()
  {
    static const std::string MY_END_POINT_ID("end_point");
    return MY_END_POINT_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Translation::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Translation();

private:
  /// Calculate the translation using an axis and a distance.
  std::shared_ptr<GeomAPI_Trsf> translationByAxisAndDistance();

  /// Calculate the translation using three dimensions X, Y and Z
  std::shared_ptr<GeomAPI_Trsf> translationByDimensions();

  /// Calculate the translation usind two points
  std::shared_ptr<GeomAPI_Trsf> translationByTwoPoints();

  /// Perform the translation
  void performTranslation(const std::shared_ptr<GeomAPI_Trsf>& theTrsf);
};

#endif
