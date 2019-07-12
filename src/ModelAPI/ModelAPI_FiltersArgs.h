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

#ifndef ModelAPI_FiltersArgs_H_
#define ModelAPI_FiltersArgs_H_

#include "ModelAPI_FiltersFeature.h"

#include <map>

/// separator between the filter name and the filter attribute ID
static const std::string kFilterSeparator = "__";

/// definition of arguments of filters: id of the argument to attributes
class ModelAPI_FiltersArgs {
  /// a map from the FilterID+AttributeID -> attribute
  std::map<std::string, AttributePtr> myMap;
  std::string myCurrentFilter; ///< ID of the filter that will take attributes now
  FiltersFeaturePtr myFeature; ///< the feature is stored to minimize initAttribute interface
public:
  ModelAPI_FiltersArgs() {}

  /// Sets the current filter ID
  void setFilter(const std::string& theFilterID) {
    myCurrentFilter = theFilterID;
  }

  /// Sets the current feature
  void setFeature(const FiltersFeaturePtr theFeature) {
    myFeature = theFeature;
  }

  /// Appends an argument of a filter
  void add(AttributePtr theAttribute) {
    myMap[theAttribute->id()] = theAttribute;
  }

  /// returns the argument of the current filter by the argument id
  AttributePtr argument(const std::string& theID) const {
    return myMap.find(myCurrentFilter + kFilterSeparator + theID)->second;
  }
  /// adds an attribute of the filter
  std::shared_ptr<ModelAPI_Attribute> initAttribute(
    const std::string& theID, const std::string theAttrType) {
    AttributePtr aR = myFeature->data()->addFloatingAttribute(theID, theAttrType, myCurrentFilter);
    aR->setIsArgument(false); // to avoid parametric update
    return aR;
  }
};

#endif