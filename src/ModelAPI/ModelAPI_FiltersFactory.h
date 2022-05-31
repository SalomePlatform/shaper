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

#ifndef ModelAPI_FiltersFactory_H_
#define ModelAPI_FiltersFactory_H_

#include "ModelAPI_Feature.h"
#include "ModelAPI_Filter.h"
#include "ModelAPI_ResultBody.h"

#include <GeomAPI_Shape.h>


/**\class ModelAPI_FiltersFactory
* \ingroup DataModel
* \brief Manages registering of filters
*/
class ModelAPI_FiltersFactory
{
public:
  virtual ~ModelAPI_FiltersFactory() {}

  /// Register an instance of a filter
  /// \param theID unique identifier of the filter, not necessary equal to the name of filter
  /// \param theFilter the filter's instance
  virtual void registerFilter(const std::string& theID, ModelAPI_Filter* theFilter) = 0;

  /// Returns true if all filters of the Filters feature are ok for the Shape (taking into account
  /// the Reversed states).
  /// \param theFiltersFeature feature that contains all information about the filters
  /// \param theResult parent result of the shape to check
  /// \param theShape the checked shape
  virtual bool isValid(FeaturePtr theFiltersFeature,
                       ResultPtr theResult,
                       GeomShapePtr theShape) = 0;

  /// Returns list of all shapes and subshapes in the study, satisfying
  ///         criteria of all filters of \a theFilterFeature.
  /// \param theFiltersFeature feature that contains all information about the filters
  /// \param theShapeType the type of sub-shapes to find
  virtual std::list< std::pair<ResultPtr, GeomShapePtr> > select
    (const FiltersFeaturePtr& theFilterFeature,
     const GeomAPI_Shape::ShapeType theShapeType) = 0;

  /// Returns the filters that support the given shape type
  virtual std::list<FilterPtr> filters(GeomAPI_Shape::ShapeType theType) = 0;

  /// Returns a filter by ID
  virtual FilterPtr filter(std::string theID) = 0;

  /// Returns a filter ID by the filter pointer
  virtual std::string id(FilterPtr theFilter) = 0;

protected:
  /// Get instance from Session
  ModelAPI_FiltersFactory() {}
};

#endif
