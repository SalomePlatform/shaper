// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_SortListOfShapes_H_
#define GeomAlgoAPI_SortListOfShapes_H_

#include "GeomAlgoAPI.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_SortListOfShapes
/// \ingroup DataAlgo
/// \brief Performs sorting of shapes according to geometric properties.
class GeomAlgoAPI_SortListOfShapes
{
 public:
   /// \brief Sorts list of shapes in accordance with their geometric properties.
   /// \param[in/out] theShapes list of shapes.
   GEOMALGOAPI_EXPORT static void sort(ListOfShape& theShapes);
};

#endif
