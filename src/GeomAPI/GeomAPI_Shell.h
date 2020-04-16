// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_Shell_H_
#define GeomAPI_Shell_H_

#include <GeomAPI_Shape.h>

class GeomAPI_Sphere;
class GeomAPI_Cylinder;
class GeomAPI_Cone;
class GeomAPI_Torus;
class GeomAPI_Box;

/// \class GeomAPI_Shell
/// \ingroup DataModel
/// \brief Interface to the shell object
class GeomAPI_Shell: public GeomAPI_Shape
{
public:
  /// Makes an undefined shell.
  GEOMAPI_EXPORT GeomAPI_Shell();

  /// Creation of shell by the shell-shape
  GEOMAPI_EXPORT GeomAPI_Shell(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Returns sphere if the shell consists only of faces
  /// which are based on the same spherical surface
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Sphere> getSphere() const;

  /// Returns cylinder if the shell consists only of faces
  /// which are based on the same cylindrical surface
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Cylinder> getCylinder() const;

  /// Returns cone if the shell consists only of faces
  /// which are based on the same conical surface
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Cone> getCone() const;

  /// Returns torus if the shell consists only of faces
  /// which are based on the same toroidal surface
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Torus> getTorus() const;

  /// Returns box if the shell consists of 6 rectangular faces composing a box
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Box> getParallelepiped() const;

  /// Return middle point on the shell
  GEOMAPI_EXPORT virtual std::shared_ptr<GeomAPI_Pnt> middlePoint() const;
};

typedef std::shared_ptr<GeomAPI_Shell> GeomShellPtr;

#endif
