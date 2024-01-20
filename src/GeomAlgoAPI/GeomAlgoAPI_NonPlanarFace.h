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

#ifndef GeomAlgoAPI_NonPlanarFace_H_
#define GeomAlgoAPI_NonPlanarFace_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

/** \class GeomAlgoAPI_NonPlanarFace
 *  \ingroup DataAlgo
 *  \brief Creates non planar faces
 */
class GeomAlgoAPI_NonPlanarFace : public GeomAlgoAPI_MakeShape
{
public:

   /// Creates list of faces based on the non-planar wires
   /// \param theEdges list of input non-planar edges.
  GEOMALGOAPI_EXPORT
  GeomAlgoAPI_NonPlanarFace(const ListOfShape& theEdges);

   /// Return list of created faces
  GEOMALGOAPI_EXPORT const ListOfShape& faces() const
  { return myResultFaces; }

private:
  /// \brief Perform operation
  /// \param theEdges list of selected wires.
  GEOMALGOAPI_EXPORT void build(const ListOfShape& theEdges);

  std::list<std::shared_ptr<GeomAPI_Shape> > myResultFaces;
};

#endif
