// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_CompoundBuilder_H_
#define GeomAlgoAPI_CompoundBuilder_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <memory>

#include <list>

/**\class GeomAlgoAPI_CompoundBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_CompoundBuilder
{
 public:
  /// Creates compund of the given shapes
  /// \param theShapes a list of shapes
  static std::shared_ptr<GeomAPI_Shape> compound(
      std::list<std::shared_ptr<GeomAPI_Shape> > theShapes);

  /// Produces the integerr identifier of the shape theSub in theContext (needed for
  /// groups export to old GEOM)
  /// \returns zero if theSub not found in theContext
  static int id(
      std::shared_ptr<GeomAPI_Shape> theContext, std::shared_ptr<GeomAPI_Shape> theSub);
};

#endif
