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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomAlgoAPI_Intersection_H_
#define GeomAlgoAPI_Intersection_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Intersection
/// \ingroup DataAlgo
/// \brief Performs the intersection operations.
class GeomAlgoAPI_Intersection : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Constructor.
  /// \param[in] theObjects list of objects.
  /// \param[in] theTools list of tools.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Intersection(const ListOfShape& theObjects,
                                              const ListOfShape& theTools);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theObjects,
             const ListOfShape& theTools);
};

#endif
