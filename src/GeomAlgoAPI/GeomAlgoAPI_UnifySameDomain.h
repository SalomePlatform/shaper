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

#ifndef GeomAlgoAPI_UnifySameDomain_H_
#define GeomAlgoAPI_UnifySameDomain_H_

#include "GeomAlgoAPI.h"
#include "GeomAlgoAPI_MakeShape.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_UnifySameDomain
/// \ingroup DataAlgo
/// \brief This tool tries to unify faces and edges of the shape which lies on the same geometry.
class GeomAlgoAPI_UnifySameDomain: public GeomAlgoAPI_MakeShape
{
public:
  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_UnifySameDomain(const ListOfShape& theShapes);

  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_UnifySameDomain(const GeomShapePtr& theShape);

  /// \return the list of shapes modified from the shape \a theShape.
  /// \param[in] theShape base shape.
  /// \param[out] theHistory modified shapes.
  GEOMALGOAPI_EXPORT virtual void modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           ListOfShape& theHistory);

private:
  /// Builds resulting shape from list of shapes.
  void build(const ListOfShape& theShapes);

  /// Builds resulting shape from the shape.
  void build(const GeomShapePtr& theShape, const bool theIsToSimplifyShell);
};

#endif
