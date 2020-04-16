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

#ifndef ModelAPI_FiltersFeature_H_
#define ModelAPI_FiltersFeature_H_

#include "ModelAPI_Attribute.h"
#include "ModelAPI_Feature.h"


/**\class ModelAPI_FiltersFeature
* \ingroup DataModel
* \brief An interface for working with filters in the feature. A filters feature must inherit it
*       in order to allow management of filters in the feature data structure.
*/
class ModelAPI_FiltersFeature: public ModelAPI_Feature
{
public:
  /// Adds a filter to the feature. Also initializes arguments of this filter.
  /// Returns the real identifier of the filter.
  virtual std::string addFilter(const std::string theFilterID) = 0;

  /// Removes an existing filter from the feature.
  virtual void removeFilter(const std::string theFilterID) = 0;

  /// Returns the list of existing filters in the feature.
  virtual std::list<std::string> filters() const = 0;

  /// Stores the reversed flag for the filter.
  virtual void setReversed(const std::string theFilterID, const bool theReversed) = 0;

  /// Returns the reversed flag value for the filter.
  virtual bool isReversed(const std::string theFilterID) = 0;

  /// Returns the ordered list of attributes related to the filter.
  virtual std::list<AttributePtr> filterArgs(const std::string theFilterID) const = 0;

  /// Sets the attribute (not-persistent field) that contains this filters feature.
  /// The filter feature may make synchronization by this method call.
  virtual void setAttribute(const AttributePtr& theAttr) = 0;

  /// Returns the attribute (not-persistent field) that contains this filters feature.
  virtual const AttributePtr& baseAttribute() const = 0;
};

typedef std::shared_ptr<ModelAPI_FiltersFeature> FiltersFeaturePtr;

#endif