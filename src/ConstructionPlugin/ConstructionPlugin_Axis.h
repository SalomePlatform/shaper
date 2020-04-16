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

#ifndef ConstructionPlugin_Axis_H
#define ConstructionPlugin_Axis_H

#include "ConstructionPlugin.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <GeomAPI_ICustomPrs.h>

/**\class ConstructionPlugin_Axis
 * \ingroup Plugins
 * \brief Feature for creation of the new axis in PartSet.
 */
class ConstructionPlugin_Axis : public ModelAPI_Feature, public GeomAPI_ICustomPrs
{
 public:
  /// Returns the kind of a feature
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = ConstructionPlugin_Axis::ID();
    return MY_KIND;
  }

  /// Axis kind
  inline static const std::string& ID()
  {
    static const std::string CONSTRUCTION_AXIS_KIND("Axis");
    return CONSTRUCTION_AXIS_KIND;
  }

  /// attribute name for first point
  inline static const std::string& METHOD()
  {
    static const std::string METHOD_ATTR("CreationMethod");
    return METHOD_ATTR;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_TWO_POINTS()
  {
    static const std::string METHOD_ATTR("AxisByPointsCase");
    return METHOD_ATTR;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_CYLINDRICAL_FACE()
  {
    static const std::string METHOD_ATTR("AxisByCylindricalFaceCase");
    return METHOD_ATTR;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_POINT_AND_DIRECTION()
  {
    static const std::string METHOD_ATTR("AxisByPointAndDirection");
    return METHOD_ATTR;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_DIMENSIONS()
  {
    static const std::string METHOD_ATTR("AxisByDimensionsCase");
    return METHOD_ATTR;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_LINE()
  {
    static const std::string METHOD_ATTR("by_line");
    return METHOD_ATTR;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_PLANE_AND_POINT()
  {
    static const std::string METHOD_ATTR("by_plane_and_point");
    return METHOD_ATTR;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_TWO_PLANES()
  {
    static const std::string METHOD_ATTR("by_two_planes");
    return METHOD_ATTR;
  }

  /// attribute name for first point
  inline static const std::string& POINT_FIRST()
  {
    static const std::string POINT_ATTR_FIRST("FirstPoint");
    return POINT_ATTR_FIRST;
  }
  /// attribute name for second point
  inline static const std::string& POINT_SECOND()
  {
    static const std::string POINT_ATTR_SECOND("SecondPoint");
    return POINT_ATTR_SECOND;
  }
  /// attribute name for second point
  inline static const std::string& CYLINDRICAL_FACE()
  {
    static const std::string CYLINDRICAL_FACE_ATTR("CylindricalFace");
    return CYLINDRICAL_FACE_ATTR;
  }
  /// attribute name for the X dimension
  inline static const std::string& DX()
  {
    static const std::string DX_ATTR("DX");
    return DX_ATTR;
  }
  /// attribute name for the Y dimension
  inline static const std::string& DY()
  {
    static const std::string DY_ATTR("DY");
    return DY_ATTR;
  }
  /// attribute name for the Z dimension
  inline static const std::string& DZ()
  {
    static const std::string DZ_ATTR("DZ");
    return DZ_ATTR;
  }

  /// attribute name for X direction
  inline static const std::string& X_DIRECTION()
  {
    static const std::string ATTR_X_DIRECTION("X_Direction");
    return ATTR_X_DIRECTION;
  }

  /// attribute name for Y direction
  inline static const std::string& Y_DIRECTION()
  {
    static const std::string ATTR_Y_DIRECTION("Y_Direction");
    return ATTR_Y_DIRECTION;
  }

  /// attribute name for Y direction
  inline static const std::string& Z_DIRECTION()
  {
    static const std::string ATTR_Z_DIRECTION("Z_Direction");
    return ATTR_Z_DIRECTION;
  }

  /// Attribute name for line.
  inline static const std::string& LINE()
  {
    static const std::string ATTR_ID("line");
    return ATTR_ID;
  }

  /// Attribute name for plane.
  inline static const std::string& PLANE()
  {
    static const std::string ATTR_ID("plane");
    return ATTR_ID;
  }

  /// Attribute name for point.
  inline static const std::string& POINT()
  {
    static const std::string ATTR_ID("point");
    return ATTR_ID;
  }

  /// Attribute name for plane 1.
  inline static const std::string& PLANE1()
  {
    static const std::string ATTR_ID("plane1");
    return ATTR_ID;
  }

  /// Attribute name for use offset for plane 1.
  inline static const std::string& USE_OFFSET1()
  {
    static const std::string ATTR_ID("use_offset1");
    return ATTR_ID;
  }

  /// Attribute name for offset for plane 1.
  inline static const std::string& OFFSET1()
  {
    static const std::string ATTR_ID("offset1");
    return ATTR_ID;
  }

  /// Attribute name for reverse offset for plane 1.
  inline static const std::string& REVERSE_OFFSET1()
  {
    static const std::string ATTR_ID("reverse_offset1");
    return ATTR_ID;
  }

  /// Attribute name for plane 2.
  inline static const std::string& PLANE2()
  {
    static const std::string ATTR_ID("plane2");
    return ATTR_ID;
  }

  /// Attribute name for use offset for plane 2.
  inline static const std::string& USE_OFFSET2()
  {
    static const std::string ATTR_ID("use_offset2");
    return ATTR_ID;
  }

  /// Attribute name for offset for plane 2.
  inline static const std::string& OFFSET2()
  {
    static const std::string ATTR_ID("offset2");
    return ATTR_ID;
  }

  /// Attribute name for reverse offset for plane 2.
  inline static const std::string& REVERSE_OFFSET2()
  {
    static const std::string ATTR_ID("reverse_offset2");
    return ATTR_ID;
  }

  /// Returns a minimal length for axis
  inline static const double MINIMAL_LENGTH() { return 1.e-5; }

  /// Creates a new part document if needed
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  ConstructionPlugin_Axis();

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs);

 protected:
  /// Creates a new axis by two defined points
  void createAxisByTwoPoints();
  /// Creates a new axis using three dimensions
  void createAxisByDimensions();
  /// Creates a new axis as copy of cylindrical face axis
  void createAxisByCylindricalFace();
  /// Creates a new axis by point and direction
  void createAxisByPointAndDirection();
  void createAxisByLine();
  void createAxisByPlaneAndPoint();
  void createAxisByTwoPlanes();
};


#endif
