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

#ifndef GeomAlgoAPI_ThroughAll_H_
#define GeomAlgoAPI_ThroughAll_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_Prism.h>
//#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_ThroughAll
/// \ingroup DataAlgo
/// \brief Cuts a prism by all given objects, throw away end pieces
class GeomAlgoAPI_ThroughAll : public GeomAlgoAPI_Boolean
{
public:

  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_ThroughAll (std::shared_ptr<GeomAlgoAPI_Prism> thePrismAlgo,
                                             const ListOfShape& theObjects);

private:
  /// Builds resulting shape.
  void removeEnds (std::shared_ptr<GeomAlgoAPI_Prism> thePrismAlgo);
};

#endif
