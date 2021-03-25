// Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_Fillet1D_H_
#define GeomAlgoAPI_Fillet1D_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Fillet1D
/// \ingroup DataAlgo
/// \brief Perform fillet on vertices of a wire
class GeomAlgoAPI_Fillet1D : public GeomAlgoAPI_MakeShape
{
  typedef std::map<GeomShapePtr, ListOfShape, GeomAPI_Shape::Comparator> MapModified;

public:
  /// Run fillet operation on a set of vertices with fixed radius.
  /// \param theBaseWire        a changing Wire
  /// \param theFilletVertices  list of edges the fillet is performed on
  /// \param theFilletRadius    radius of the fillet
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Fillet1D(const GeomShapePtr& theBaseWire,
                                          const ListOfShape&  theFilletVertices,
                                          const double        theFilletRadius);

  /// \return the list of shapes generated from the shape \a theShape.
  /// \param[in] theOldShape base shape.
  /// \param[out] theNewShapes shapes generated from \a theShape. Does not cleared!
  GEOMALGOAPI_EXPORT virtual void generated(const GeomShapePtr theOldShape,
                                            ListOfShape& theNewShapes);

  /// \return the list of shapes modified from the shape \a theShape.
  /// \param[in] theOldShape base shape.
  /// \param[out] theNewShapes shapes modified from \a theShape. Does not cleared!
  GEOMALGOAPI_EXPORT virtual void modified(const GeomShapePtr theOldShape,
                                           ListOfShape& theNewShapes);

  /// \return List of failed vertices
  const ListOfShape& failedVertices() const { return myFailedVertices; }

private:
  /// Perform 1d-fillet on wire
  /// \param theBaseWire        a changing wire
  /// \param theFilletVertices  list of vertices of filler
  /// \param theRadius          fillet radius
  void build(const GeomShapePtr& theBaseWire,
             const ListOfShape&  theFilletVertices,
             const double        theRadius);

private:
  MapModified myGenerated;
  MapModified myModified;

  ListOfShape myFailedVertices;
};

#endif
