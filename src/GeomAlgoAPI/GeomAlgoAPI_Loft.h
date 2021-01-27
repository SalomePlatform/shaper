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

#ifndef GeomAlgoAPI_Loft_H_
#define GeomAlgoAPI_Loft_H_

#include "GeomAlgoAPI.h"

#include "GeomAlgoAPI_MakeShape.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Loft
/// \ingroup DataAlgo
/// \brief Allows to create loft of two objects with same type.
/// It produces the following results from objects:
/// 2 edges -> Face
/// 2 Wire -> Shell
/// 2 Face -> Solid
class GeomAlgoAPI_Loft : public GeomAlgoAPI_MakeShape
{
  public:
    /// \brief Creates loft for two given shape with same type.
    /// \param[in] theFirstShape  the first shape.
    /// \param[in] theSecondShape the second shape.
    GEOMALGOAPI_EXPORT GeomAlgoAPI_Loft(const GeomShapePtr theFirstShape,
                                        const GeomShapePtr theSecondShape);

  private:
    void build(const GeomShapePtr theFirstShape,
              const GeomShapePtr theSecondShape);

};

#endif
