// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef GeomAlgoAPI_Partition_H_
#define GeomAlgoAPI_Partition_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Partition
/// \ingroup DataAlgo
/// \brief Allows to perform of partition operations
class GeomAlgoAPI_Partition : public GeomAlgoAPI_MakeShape
{
public:
  /// Constructor.
  /// \param[in] theObjects list of main objects.
  /// \param[in] theTools list of tools.
  /// \param[in] theFuzzy additional tolerance value.
  ///            If the fuzzy value is below the minimum tolerance value (1.e-7), the
  ///            algorithm will use the default internal fuzzy value from OCCT.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Partition(const ListOfShape& theObjects,
                                           const ListOfShape& theTools,
                                           const double theFuzzy = 1.e-8);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theObjects,
             const ListOfShape& theTools,
             const double theFuzzy);
};

#endif
