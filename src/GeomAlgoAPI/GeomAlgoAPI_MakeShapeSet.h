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

#ifndef GeomAlgoAPI_MakeShapeSet_H_
#define GeomAlgoAPI_MakeShapeSet_H_

#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShapeList.h>

#include <memory>

/// \class GeomAlgoAPI_MakeShapeSet
/// \ingroup DataAlgo
/// \brief Set of independent topological shapes constructions
class GeomAlgoAPI_MakeShapeSet : public GeomAlgoAPI_MakeShapeList
{
public:
  /// Default constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShapeSet();

  /// \brief Constructor
  /// \param[in] theMakeShapeSet list of algorithms.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShapeSet(const ListOfMakeShape& theMakeShapeSet);

  /// \return the list of shapes generated from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void generated(const GeomShapePtr theShape,
                                            ListOfShape& theHistory);

  /// \return the list of shapes modified from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void modified(const GeomShapePtr theShape,
                                           ListOfShape& theHistory);
};

#endif
