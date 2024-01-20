// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef GeomAPI_Face_H_
#define GeomAPI_Face_H_

#include <GeomAPI_Shape.h>

class GeomAPI_Pln;
class GeomAPI_Sphere;
class GeomAPI_Cylinder;
class GeomAPI_Cone;
class GeomAPI_Torus;

/**\class GeomAPI_Face
* \ingroup DataModel
 * \brief Interface to the face object
 */
class GeomAPI_Face : public GeomAPI_Shape
{
public:
  /// Creation of empty (null) shape
  GEOMAPI_EXPORT
  GeomAPI_Face();

  /// Creation of face by the face-shape
  GEOMAPI_EXPORT
  GeomAPI_Face(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Returns true if the current face is geometrically equal to the given face
  GEOMAPI_EXPORT
  virtual bool isEqual(const std::shared_ptr<GeomAPI_Shape> theFace) const;

  /// Returns \c true if faces have same underlying surface
  GEOMAPI_EXPORT
  virtual bool isSameGeometry(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns true if the face is a cylindrical face
  GEOMAPI_EXPORT
  bool isCylindrical() const;

  /// Returns the base plane of the face (if it is planar) with location in the center of the face
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pln> getPlane() const;

  /// Returns sphere if the face is based on the spherical surface
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Sphere> getSphere() const;

  /// Returns cylinder if the face is based on the cylindrical surface
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Cylinder> getCylinder() const;

  /// Returns cone if the face is based on the conical surface
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Cone> getCone() const;

  /// Returns torus if the face is based on the toroidal surface
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Torus> getTorus() const;

  /// Return inner point in the face
  GEOMAPI_EXPORT virtual std::shared_ptr<GeomAPI_Pnt> middlePoint() const;
};

//! Pointer on attribute object
typedef std::shared_ptr<GeomAPI_Face> GeomFacePtr;

#endif

