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

#ifndef GeomAPI_Angle_H_
#define GeomAPI_Angle_H_

#include <GeomAPI_Interface.h>

class GeomAPI_Edge;
class GeomAPI_Pnt;

/// \class GeomAPI_Angle
/// \ingroup DataModel
/// \brief Build an angle
class GeomAPI_Angle : public GeomAPI_Interface
{
public:
  /// Creation of an angle defined by two edges in the given intersection point.
  GEOMAPI_EXPORT
  GeomAPI_Angle(const std::shared_ptr<GeomAPI_Edge>& theEdge1,
                const std::shared_ptr<GeomAPI_Edge>& theEdge2,
                const std::shared_ptr<GeomAPI_Pnt>&  thePoint);

  /// Creation of an angle defined by 3 points (the second point is an apex of an angle).
  GEOMAPI_EXPORT
  GeomAPI_Angle(const std::shared_ptr<GeomAPI_Pnt>& thePoint1,
                const std::shared_ptr<GeomAPI_Pnt>& thePoint2,
                const std::shared_ptr<GeomAPI_Pnt>& thePoint3);

  /// Returns value of the angle in degrees
  GEOMAPI_EXPORT double angleDegree();
  /// Returns value of the angle in radians
  GEOMAPI_EXPORT double angleRadian();
};

#endif
