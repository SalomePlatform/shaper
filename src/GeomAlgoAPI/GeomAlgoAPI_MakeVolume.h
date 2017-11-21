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

#ifndef GeomAlgoAPI_MakeVolume_H_
#define GeomAlgoAPI_MakeVolume_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_MakeVolume
/// \ingroup DataAlgo
/// \brief Perform building solid (compsolid) from a list of faces.
class GeomAlgoAPI_MakeVolume : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Perform making volume.
  /// \param[in] theObjects list of faces.
  /// \return a solid or a compsolid as result of operation.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> make(const ListOfShape& theFaces);

  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeVolume(const ListOfShape& theFaces);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theFaces);
};

#endif
