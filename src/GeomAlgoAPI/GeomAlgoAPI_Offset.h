// Copyright (C) 2019-2019  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_Offset_H_
#define GeomAlgoAPI_Offset_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

/// \class GeomAlgoAPI_Offset
/// \ingroup DataAlgo
/// \brief Perform 3D offset for the shape
class GeomAlgoAPI_Offset : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Construct offset.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Offset(const GeomShapePtr& theShape,
                                        const double theOffsetValue);

  /// \return the list of shapes generated from the shape \a theShape.
  /// \param[in] theOldShape base shape.
  /// \param[out] theNewShapes shapes generated from \a theShape. Does not cleared!
  GEOMALGOAPI_EXPORT virtual void generated(const GeomShapePtr theOldShape,
                                            ListOfShape& theNewShapes);

private:
  /// \brief Perform offset operation
  void build(const GeomShapePtr& theShape, const double theOffsetValue);
};

#endif
