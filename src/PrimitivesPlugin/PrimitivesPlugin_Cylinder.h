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

#ifndef PRIMITIVESPLUGIN_CYLINDER_H_
#define PRIMITIVESPLUGIN_CYLINDER_H_

#include <PrimitivesPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Cylinder.h>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/**\class PrimitivesPlugin_Cylinder
 * \ingroup Plugins
 * \brief Feature for creation of a cylinder.
 *
 * Creates a cylinder from a radius, a height, an angle, a center point defaulting to
 * the origin and an axis defaulting to OZ
 */
class PrimitivesPlugin_Cylinder : public ModelAPI_Feature
{
 public:
  /// Cylinder kind
  inline static const std::string& ID()
  {
    static const std::string MY_CYLINDER_ID("Cylinder");
    return MY_CYLINDER_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD_CYLINDER()
  {
    static const std::string MY_CREATION_METHOD_ID("Cylinder");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD_CYLINDER_PORTION()
  {
    static const std::string MY_CREATION_METHOD_ID("CylinderPortion");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of the base point
  inline static const std::string& BASE_POINT_ID()
  {
    static const std::string MY_BASE_POINT_ID("base_point");
    return MY_BASE_POINT_ID;
  }

  /// Attribute name of the axis
  inline static const std::string& AXIS_ID()
  {
    static const std::string MY_AXIS_ID("axis");
    return MY_AXIS_ID;
  }

  /// Attribute name of the radius
  inline static const std::string& RADIUS_ID()
  {
    static const std::string MY_RADIUS_ID("radius");
    return MY_RADIUS_ID;
  }

  /// Attribute name of the height
  inline static const std::string& HEIGHT_ID()
  {
    static const std::string MY_HEIGHT_ID("height");
    return MY_HEIGHT_ID;
  }

  /// Attribute name of the angle
  inline static const std::string& ANGLE_ID()
  {
    static const std::string MY_ANGLE_ID("angle");
    return MY_ANGLE_ID;
  }

  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Cylinder::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PrimitivesPlugin_Cylinder();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Cylinder> theCylinderAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultCylinder);

  ///Perform the creation of a cylinder
  void createCylinder(bool withAngle);

};

#endif // PRIMITIVESPLUGIN_CYLINDER_H_
