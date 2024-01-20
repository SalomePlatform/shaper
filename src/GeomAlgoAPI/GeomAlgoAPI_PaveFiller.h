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

#ifndef GeomAlgoAPI_PaveFiller_H_
#define GeomAlgoAPI_PaveFiller_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_PaveFiller
/// \ingroup DataAlgo
/// \brief Finds the common parts from the list of shapes and
///  breaks it to shapes with shared subshapes.
class GeomAlgoAPI_PaveFiller : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Constructor.
  /// \param[in] theListOfShape list of shape which should be splitted.
  /// \param[in] theIsMakeCompSolids if true gather shapes with shared faces to compsolids.
  /// \param[in] theFuzzy additional tolerance value.
  ///            If the fuzzy value is below the minimum tolerance value (1.e-7), the
  ///            algorithm will use the default internal fuzzy value from OCCT.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_PaveFiller(const ListOfShape& theListOfShape,
                                            const bool theIsMakeCompSolids = false,
                                            const double theFuzzy = 1.e-8);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theListOfShape, const bool theIsMakeCompSolids, const double theFuzzy);
};

#endif
