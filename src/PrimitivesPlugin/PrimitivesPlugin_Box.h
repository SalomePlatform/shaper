// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef PrimitivesPlugin_Box_H_
#define PrimitivesPlugin_Box_H_

#include <PrimitivesPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Box.h>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/**\class PrimitivesPlugin_Box
 * \ingroup Plugins
 * \brief Feature for creation of a box primitive using various methods.
 *
 * Box creates a cuboid - Parallelepiped with 6 rectangular faces. It can be built via three
 * methods : using two points that define a diagonal, a point that define a center and 3 lengths
 * that define the half-lengths on X, Y and Z-axes, or using 3 lengths that define the 
 * rectangular dimensions.
 */
class PrimitivesPlugin_Box : public ModelAPI_Feature
{
 public:
  /// Box kind
  inline static const std::string& ID()
  {
    static const std::string MY_BOX_ID("Box");
    return MY_BOX_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD_BY_DIMENSIONS()
  {
    static const std::string MY_CREATION_METHOD_ID("BoxByDimensions");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD_BY_TWO_POINTS()
  {
    static const std::string MY_CREATION_METHOD_ID("BoxByTwoPoints");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD_BY_ONE_POINT_AND_DIMS()
  {
    static const std::string MY_CREATION_METHOD_ID("BoxByOnePointAndDims");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of first point
  inline static const std::string& POINT_FIRST_ID()
  {
    static const std::string MY_POINT_FIRST_ID("FirstPoint");
    return MY_POINT_FIRST_ID;
  }

  /// Attribute name of second point
  inline static const std::string& POINT_SECOND_ID()
  {
    static const std::string MY_POINT_SECOND_ID("SecondPoint");
    return MY_POINT_SECOND_ID;
  }

  /// Attribute first coordinate
  inline static const std::string& DX_ID()
  {
    static const std::string MY_DX_ID("dx");
    return MY_DX_ID;
  }

  /// Attribute second coordinate
  inline static const std::string& DY_ID()
  {
    static const std::string MY_DY_ID("dy");
    return MY_DY_ID;
  }

  /// Attribute third coordinate
  inline static const std::string& DZ_ID()
  {
    static const std::string MY_DZ_ID("dz");
    return MY_DZ_ID;
  }

  /// Attribute name of the first coordinate of the center
  inline static const std::string& OX_ID()
  {
    static const std::string MY_OX_ID("ox");
    return MY_OX_ID;
  }

  /// Attribute name of the second coordinate of the center
  inline static const std::string& OY_ID()
  {
    static const std::string MY_OY_ID("oy");
    return MY_OY_ID;
  }

  /// Attribute name of the third coordinate of the center
  inline static const std::string& OZ_ID()
  {
    static const std::string MY_OZ_ID("oz");
    return MY_OZ_ID;
  }

  /// Attribute name of the half-length on X axis
  inline static const std::string& HALF_DX_ID()
  {
    static const std::string MY_HALF_DX_ID("half_dx");
    return MY_HALF_DX_ID;
  }

  /// Attribute name of the half-length on Y axis
  inline static const std::string& HALF_DY_ID()
  {
    static const std::string MY_HALF_DY_ID("half_dy");
    return MY_HALF_DY_ID;
  }

  /// Attribute name of the half-length on Z axis
  inline static const std::string& HALF_DZ_ID()
  {
    static const std::string MY_HALF_DZ_ID("half_dz");
    return MY_HALF_DZ_ID;
  }

  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Box::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PrimitivesPlugin_Box();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Box> theBoxAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBox);

  ///Perform the creation of the box using two points defining a diagonal
  void createBoxByTwoPoints();

  ///Perform the creation of the box using three cordinates
  void createBoxByDimensions();

  ///Perform the creation of the box using a center and three half-lenths
  void createBoxByOnePointAndDims();

};


#endif
