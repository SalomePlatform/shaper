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

#ifndef ModelAPI_Filter_H_
#define ModelAPI_Filter_H_

#include "ModelAPI.h"
#include "ModelAPI_Attribute.h"
#include "ModelAPI_Feature.h"

#include "ModelAPI_Data.h"

#include <GeomAPI_Shape.h>
#include <map>

/// separator between the filter name and the filter attribute ID
static const std::string kFilterSeparator = "__";

/**\class ModelAPI_FiltersFeature
* \ingroup DataModel
* \brief An interface for working with filters in the feature. A filters feature must inherit it
*       in order to allow management of filters in the feature data structure.
*/
class ModelAPI_FiltersFeature: public ModelAPI_Feature
{
public:
  /// Adds a filter to the feature. Also initializes arguments of this filter.
  virtual void addFilter(const std::string theFilterID) = 0;

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

/**\class ModelAPI_ViewFilter
* \ingroup DataModel
* \brief A general interface class filters definition
*/
class ModelAPI_Filter
{
public:
  /// Returns name of the filter to represent it in GUI
  virtual const std::string& name() const = 0;

  /// Returns true if the given shape type is supported
  virtual bool isSupported(GeomAPI_Shape::ShapeType theType) const = 0;

  /// This method should contain the filter logic. It returns true if the given shape
  /// is accepted by the filter.
  /// \param theShape the given shape
  virtual bool isOk(const GeomShapePtr& theShape, const ModelAPI_FiltersArgs& theArgs) const = 0;

  /// Returns XML string which represents GUI of the filter
  /// By default it returns nothing (no GUI)
  virtual std::string xmlRepresentation() const { return ""; }

  /// Initializes arguments of a filter. If a filter has no arguments, this method may be
  /// not redefined.
  virtual void initAttributes(ModelAPI_FiltersArgs& theArguments) {}

private:
  bool myIsReverse;
};

typedef std::shared_ptr<ModelAPI_Filter> FilterPtr;


/**\class ModelAPI_FiltersFactory
* \ingroup DataModel
* \brief Manages registering of filters
*/
class ModelAPI_FiltersFactory
{
public:
  /// Register an instance of a filter
  /// \param theID unique identifier of the filter, not necessary equal to the name of filter
  /// \param theFilter the filter's instance
  virtual void registerFilter(const std::string& theID, ModelAPI_Filter* theFilter) = 0;

  /// Returns true if all filters of the Filters feature are ok for the Shape (taking into account
  /// the Reversed states).
  /// \param theFiltersFeature feature that contains all information about the filters
  /// \param theShape the checked shape
  virtual bool isValid(FeaturePtr theFiltersFeature, GeomShapePtr theShape) = 0;

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