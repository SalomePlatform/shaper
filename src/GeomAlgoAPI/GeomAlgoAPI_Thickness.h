// Copyright (C) 2019-2026  CEA, EDF
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

#ifndef GeomAlgoAPI_Thickness_H_
#define GeomAlgoAPI_Thickness_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

/// \class GeomAlgoAPI_Thickness
/// \ingroup DataAlgo
/// \brief Perform Thickness or Hollowed Solid algorithm for the shape
class GeomAlgoAPI_Thickness : public GeomAlgoAPI_MakeShape
{
  typedef std::map<GeomShapePtr, ListOfShape, GeomAPI_Shape::Comparator> MapModified;

public:
  /// \brief Perform thickening algorithm
  /// \param[in] theShape base shape (Face or Shell)
  /// \param[in] theThickness thickness of the resulting solid
  /// \param[in] isInside if true, the thickness is applied towards inside
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Thickness
    (const GeomShapePtr& theShape,
     const double        theThickness,
     const bool          isInside);

  /// \brief Perform hollowed solid algorithm
  /// \param[in] theShape base shape (Solid)
  /// \param[in] theFaces the list of faces to be removed from the result
  /// \param[in] theThickness thickness of the resulting solid
  /// \param[in] isInside if true, the thickness is applied towards inside
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Thickness
    (const GeomShapePtr& theShape,
     const ListOfShape&  theFaces,
     const double        theThickness,
     const bool          isInside);

  /// \return the list of shapes generated from the shape \a theShape.
  /// \param[in] theOldShape base shape.
  /// \param[out] theNewShapes shapes generated from \a theShape. Does not cleared!
  GEOMALGOAPI_EXPORT virtual void generated(const GeomShapePtr theOldShape,
                                            ListOfShape& theNewShapes);

private:
  /// \brief Perform thickening algorithm
  void buildThickening(const GeomShapePtr& theShape,
                       const double        theThickness,
                       const bool          isInside);

  /// \brief Perform hollowed solid algorithm
  void buildHollowedSolid(const GeomShapePtr& theShape,
                          const ListOfShape&  theFaces,
                          const double        theThickness,
                          const bool          isInside);

private:
  MapModified myGenerated;
};

#endif
