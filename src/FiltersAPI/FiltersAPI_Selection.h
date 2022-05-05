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

#ifndef FILTERSAPI_SELECTION_H_
#define FILTERSAPI_SELECTION_H_

#include "FiltersAPI.h"
#include "FiltersAPI_Feature.h"

#include <ModelHighAPI_Selection.h>


/**\class FiltersAPI_Selection
 * \ingroup CPPHighAPI
 * \brief Interface for the selection of filters
 */
class FiltersAPI_Selection : public ModelHighAPI_Selection
{
public:
  /// Constructor without values
  FILTERSAPI_EXPORT
  explicit FiltersAPI_Selection(const FiltersPtr& theFeature);

  /// Destructor
  FILTERSAPI_EXPORT
  virtual ~FiltersAPI_Selection();

  /// Return filters feature
  FILTERSAPI_EXPORT
  FiltersFeaturePtr feature() const;

  /// Return selected entities
  FILTERSAPI_EXPORT
  std::list<ModelHighAPI_Selection> select(const std::string theShapeType) const;
  FILTERSAPI_EXPORT
  std::list<ModelHighAPI_Selection> select(const GeomAPI_Shape::ShapeType theShapeType) const;
};

/// Create list of filters
FILTERSAPI_EXPORT
FiltersAPI_Selection filters(const std::shared_ptr<ModelAPI_Document>& thePart,
                             const std::list<FilterAPIPtr>& theFilters);

#endif
