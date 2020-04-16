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

#ifndef FILTERSPLUGIN_SELECTION_H_
#define FILTERSPLUGIN_SELECTION_H_

#include "FiltersPlugin.h"
#include <ModelAPI_FiltersFeature.h>

/**\class FiltersPlugin_Selection
* \ingroup DataModel
* \brief An interface for working with filters in the feature. A filters feature must inherit it
*       in order to allow management of filters in the feature data structure.
*/
class FiltersPlugin_Selection : public ModelAPI_FiltersFeature
{
public:
  /// Extrusion kind
  inline static const std::string& ID() {
    static const std::string MY_GROUP_ID("FiltersSelection");
    return MY_GROUP_ID;
  }

  /// Returns the kind of a feature
  FILTERS_EXPORT virtual const std::string& getKind() {
    return ID();
  }

  /// This feature does not displayed in the data tree
  virtual bool isInHistory() {
    return false;
  }

  /// Computes a selection?
  FILTERS_EXPORT virtual void execute() {}

  /// Feature is created in the plugin manager
  FiltersPlugin_Selection() {}

  /// This method initializes all filters on open of document
  FILTERS_EXPORT virtual void initAttributes() override;

  // methods related to the filters management

  /// Adds a filter to the feature. Also initializes arguments of this filter.
  /// Returns the real identifier of the filter.
  FILTERS_EXPORT virtual std::string addFilter(const std::string theFilterID) override;

  /// Removes an existing filter from the feature.
  FILTERS_EXPORT virtual void removeFilter(const std::string theFilterID) override;

  /// Returns the list of existing filters in the feature.
  FILTERS_EXPORT virtual std::list<std::string> filters() const override;

  /// Stores the reversed flag for the filter.
  FILTERS_EXPORT virtual void setReversed
  (const std::string theFilterID, const bool theReversed) override;

  /// Returns the reversed flag value for the filter.
  FILTERS_EXPORT virtual bool isReversed(const std::string theFilterID) override;

  /// Returns the ordered list of attributes related to the filter.
  FILTERS_EXPORT virtual std::list<AttributePtr>
    filterArgs(const std::string theFilterID) const override;

  /// Sets the attribute (not-persistent field) that contains this filters feature.
  /// The filter feature may make synchronization by this method call.
  FILTERS_EXPORT virtual void setAttribute(const AttributePtr& theAttr) override;

  /// Returns the attribute (not-persistent field) that contains this filters feature.
  FILTERS_EXPORT virtual const AttributePtr& baseAttribute() const override;

protected:

  AttributePtr myBase; ///< the attribute related to this filter
};

#endif
