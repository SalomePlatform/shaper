// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_ShapeIterator_H_
#define GeomAPI_ShapeIterator_H_

#include "GeomAPI.h"
#include "GeomAPI_Shape.h"

/// \class GeomAPI_ShapeIterator
/// \ingroup DataModel
/// \brief Iterates on the underlying shape underlying a given GeomAPI_Shape object,
///        providing access
///        to its component sub-shapes. Each component shape is returned as a GeomAPI_Shape with
///        an orientation, and a compound of the original values and the relative values.
class GeomAPI_ShapeIterator : public GeomAPI_Interface
{
public:
  /// Default constructor. Creates an empty iterator, becomes usefull after Init.
  GEOMAPI_EXPORT GeomAPI_ShapeIterator();

  /// \brief Creates an iterator on theShape sub-shapes.
  /// \param[in] theShape shape to iterate.
  GEOMAPI_EXPORT GeomAPI_ShapeIterator(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \brief Initializes this iterator with theShape.
  /// \param[in] theShape shape to iterate.
  GEOMAPI_EXPORT void init(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return  true if there is another sub-shape in the shape which this iterator is scanning.
  GEOMAPI_EXPORT bool more() const;

  /// Moves on to the next sub-shape in the shape which this iterator is scanning.
  GEOMAPI_EXPORT void next();

  /// \return the current sub-shape in the shape which this iterator is scanning.
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Shape> current();
};

#endif