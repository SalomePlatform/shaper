// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_WireExplorer_H_
#define GeomAPI_WireExplorer_H_

#include <GeomAPI.h>
#include <GeomAPI_Interface.h>

class GeomAPI_Edge;
class GeomAPI_Shape;
class GeomAPI_Vertex;
class GeomAPI_Wire;

/** \class GeomAPI_WireExplorer
 *  \ingroup DataModel
 *  \brief This class is used to explore edges of wire in the order of connectivity.
 */
class GeomAPI_WireExplorer : public GeomAPI_Interface
{
public:
  /// Default constructor. Creates an empty explorer, becomes usefull after Init.
  GEOMAPI_EXPORT GeomAPI_WireExplorer();

  /** \brief Constructs an iterator on a wire.
   *  \param[in] theWire a wire to explore.
   */
  GEOMAPI_EXPORT GeomAPI_WireExplorer(const std::shared_ptr<GeomAPI_Wire>& theWire);

  /** \brief Resets this explorer.
   *  \param[in] theWire a wire to explore.
   */
  GEOMAPI_EXPORT void init(const std::shared_ptr<GeomAPI_Wire>& theWire);

  /// \return true if there are more shapes in the exploration.
  GEOMAPI_EXPORT bool more() const;

  /// Moves to the next Shape in the exploration or do nothing
  /// if there are no more shapes to explore.
  GEOMAPI_EXPORT void next();

  /// \return the current edge in the exploration or empty pointer
  /// if this explorer has no more shapes to explore.
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Edge> current();

  /// \return the current vertex in the exploration or empty pointer
  /// if this explorer has no more shapes to explore.
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Vertex> currentVertex();

  /// Clears the content of the explorer. It will return False on more().
  GEOMAPI_EXPORT void clear();
};

#endif