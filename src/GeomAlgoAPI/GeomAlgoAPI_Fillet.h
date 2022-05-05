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

#ifndef GeomAlgoAPI_Fillet_H_
#define GeomAlgoAPI_Fillet_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Fillet
/// \ingroup DataAlgo
/// \brief Perform fillet
class GeomAlgoAPI_Fillet : public GeomAlgoAPI_MakeShape
{
public:
  /// Run fillet operation with fixed radius.
  /// \param theBaseSolid    a changing solid
  /// \param theFilletEdges  list of edges the fillet is performed on
  /// \param theFilletRadius radius of the fillet
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Fillet(const GeomShapePtr& theBaseSolid,
                                        const ListOfShape&  theFilletEdges,
                                        const double        theFilletRadius);

  /// Run fillet operation with variable radius.
  /// \param theBaseSolid    a changing solid
  /// \param theFilletEdges  list of edges the fillet is performed on
  /// \param theStartRadius  start radius of the fillet
  /// \param theEndRadius    end radius of the fillet
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Fillet(const GeomShapePtr& theBaseSolid,
                                        const ListOfShape&  theFilletEdges,
                                        const double        theStartRadius,
                                        const double        theEndRadius);

private:
  /// Perform fillet operation.
  /// If theEndRadius is less than 0., the fixed radius fillet will be built.
  /// \param theBaseSolid    a changing solid
  /// \param theFilletEdges  list of edges the fillet is performed on
  /// \param theStartRadius  start radius of the fillet
  /// \param theEndRadius    end radius of the fillet
  void build(const GeomShapePtr& theBaseSolid,
             const ListOfShape&  theFilletEdges,
             const double        theStartRadius,
             const double        theEndRadius = -1.0);
};

#endif
