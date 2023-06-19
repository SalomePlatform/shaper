// Copyright (C) 2018-2023  CEA, EDF
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

#ifndef FeaturesPlugin_BoundingBox_H_
#define FeaturesPlugin_BoundingBox_H_

#include <FeaturesPlugin_BoundingBoxBase.h>

/// \class FeaturesPlugin_BoundingBox
/// \ingroup Plugins
/// \brief Persistence feature to create the bounding box.

class FeaturesPlugin_BoundingBox : public FeaturesPlugin_BoundingBoxBase
{
public:
  /// Bounding box kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("BoundingBox");
    return MY_ID;
  }

  /// Attribute name for object selected.
  inline static const std::string& OBJECT_ID()
  {
    static const std::string MY_OBJECT_ID("main_object");
    return MY_OBJECT_ID;
  }

  /// Attribute name for x coodinate.
  inline static const std::string& X_MIN_COORD_ID()
  {
    static const std::string MY_X_MIN_COORD_ID("xmincoordinate");
    return MY_X_MIN_COORD_ID;
  }

  /// Attribute name for y coodinate.
  inline static const std::string& Y_MIN_COORD_ID()
  {
    static const std::string MY_Y_MIN_COORD_ID("ymincoordinate");
    return MY_Y_MIN_COORD_ID;
  }

  /// Attribute name for z coodinate.
  inline static const std::string& Z_MIN_COORD_ID()
  {
    static const std::string MY_Z_MIN_COORD_ID("zmincoordinate");
    return MY_Z_MIN_COORD_ID;
  }

  /// Attribute name for x max coodinate.
  inline static const std::string& X_MAX_COORD_ID()
  {
    static const std::string MY_X_MAX_COORD_ID("xmaxcoordinate");
    return MY_X_MAX_COORD_ID;
  }

  /// Attribute name for y max coodinate.
  inline static const std::string& Y_MAX_COORD_ID()
  {
    static const std::string MY_Y_MAX_COORD_ID("ymaxcoordinate");
    return MY_Y_MAX_COORD_ID;
  }

  /// Attribute name for z max coodinate.
  inline static const std::string& Z_MAX_COORD_ID()
  {
    static const std::string MY_Z_MAX_COORD_ID("zmaxcoordinate");
    return MY_Z_MAX_COORD_ID;
  }

  /// Attribute name for values of result.
  inline static const std::string& RESULT_VALUES_ID()
  {
    static const std::string MY_RESULT_VALUES_ID("result_values");
    return MY_RESULT_VALUES_ID;
  }

  /// Attribute name for indicate to compute the bounding box.
  inline static const std::string& COMPUTE_ID()
  {
    static const std::string MY_COMPUTE_ID("compute");
    return MY_COMPUTE_ID;
  }

  /// \return the kind of a feature.
  virtual const std::string& getKind()
  {
    return ID();
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Return Attribut values of result.
  FEATURESPLUGIN_EXPORT virtual AttributePtr attributResultValues();

  /// Use plugin manager for features creation
  FeaturesPlugin_BoundingBox();

private:
  /// Update values displayed.
  bool updateValues();

};

#endif
