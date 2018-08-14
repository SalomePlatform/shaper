// Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomAPI_Box_H_
#define GeomAPI_Box_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Ax3;
class GeomAPI_Pnt;

/**\class GeomAPI_Box
 * \ingroup DataModel
 * \brief Box in 3D
 */
class GeomAPI_Box : public GeomAPI_Interface
{
public:
  /// Default constructor
  GEOMAPI_EXPORT GeomAPI_Box();

  /// Creation of torus defined by center point, direction, major and minor radii
  GEOMAPI_EXPORT GeomAPI_Box(const std::shared_ptr<GeomAPI_Ax3>& theCorner,
                             const double theWidth,
                             const double theDepth,
                             const double theHeight);

  /// Return axes of the box
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Ax3> axes() const;

  /// Return base corner of the box
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> corner() const;

  /// Return width of the box
  GEOMAPI_EXPORT double width() const;

  /// Return depth of the box
  GEOMAPI_EXPORT double depth() const;

  /// Return height of the box
  GEOMAPI_EXPORT double height() const;

  /// Return \c true is the box is aligned in main axes
  GEOMAPI_EXPORT bool isAxesAligned() const;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Box> GeomBoxPtr;

#endif
