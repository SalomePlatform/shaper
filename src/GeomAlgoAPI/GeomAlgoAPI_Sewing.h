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

#ifndef GeomAlgoAPI_Sewing_H_
#define GeomAlgoAPI_Sewing_H_

#include "GeomAlgoAPI.h"
#include "GeomAlgoAPI_MakeShape.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Sewing
/// \ingroup DataAlgo
/// \brief Allows creation of connected topology (shells)
///        from a set of separate topological elements (faces).
class GeomAlgoAPI_Sewing : public GeomAlgoAPI_MakeShape
{
public:
  /// Constructor (used by MakeShell).
  /// \param[in] theShapes list of selected shapes.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sewing(const ListOfShape& theShapes);

  /// Constructor with additional arguments (used by Sewing feature)
  /// \param[in] theShapes list of selected shapes.
  /// \param[in] theAllowNonManifold if True, non-manifold results are allowed.
  /// \param[in] theTolerance tolerance value used for the sewing operation.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sewing(const ListOfShape& theShapes, const bool theAllowNonManifold, const double theTolerance);

  /// \return the list of shapes modified from the shape \a theShape.
  /// \param[in] theShape base shape.
  /// \param[out] theHistory modified shapes.
  GEOMALGOAPI_EXPORT virtual void modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           ListOfShape& theHistory);

protected:
  bool  myBuildShell; // whether algorithm is used by MakeShell or by Sewing
  
private:
  /// Builds resulting shape.
  void build(const ListOfShape& theShapes, const bool theAllowNonManifold = false, const double theTolerance = 1.e-6);
};

#endif
