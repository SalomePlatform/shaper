// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef Model_FiltersFactory_H_
#define Model_FiltersFactory_H_

#include "Model.h"

#include <ModelAPI_FiltersFactory.h>

#include <map>


/**\class Model_FiltersFactory
* \ingroup DataModel
* \brief Manages registering of filters
*/
class Model_FiltersFactory : public ModelAPI_FiltersFactory
{
public:
  /// Register an instance of a filter
  /// \param theID unique identifier of the filter, not necessary equal to the name of filter
  /// \param theFilter the filter's instance
  virtual void registerFilter(const std::string& theID, ModelAPI_Filter* theFilter);

  /// Returns true if all filters of the Filters feature are ok for the Shape (taking into account
  /// the Reversed states).
  /// \param theResult parent result of the shape to check
  /// \param theShape the checked shape
  virtual bool isValid(FeaturePtr theFiltersFeature,
                       ResultPtr theResult,
                       GeomShapePtr theShape);

  /// Returns the filters that support the given shape type
  virtual std::list<FilterPtr> filters(GeomAPI_Shape::ShapeType theType);

  /// Returns a filter by ID
  virtual FilterPtr filter(std::string theID);

  /// Returns a filter ID by the filter pointer
  virtual std::string id(FilterPtr theFilter);

protected:
  /// Get instance from Session
  Model_FiltersFactory() {}

private:
  std::map<std::string, FilterPtr> myFilters;  ///< map from ID to registered filters

  friend class Model_Session;
};

#endif