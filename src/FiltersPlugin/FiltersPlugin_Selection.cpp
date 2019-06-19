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

#include "FiltersPlugin_Selection.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_FiltersFactory.h>
#include <ModelAPI_Session.h>

// identifier of the reverse flag of a filter
static const std::string kReverseAttrID("");

void FiltersPlugin_Selection::addFilter(const std::string theFilterID)
{
  ModelAPI_FiltersFactory* aFactory = ModelAPI_Session::get()->filters();
  FilterPtr aFilter = aFactory->filter(theFilterID);
  if (aFilter.get()) {
    std::shared_ptr<ModelAPI_AttributeBoolean> aBool =
      std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(data()->addFloatingAttribute(
        kReverseAttrID, ModelAPI_AttributeBoolean::typeId(), theFilterID));
    aBool->setValue(false); // not reversed by default
    // to add attributes related to the filter
    ModelAPI_FiltersArgs anArgs;
    anArgs.setFeature(std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(data()->owner()));
    anArgs.setFilter(theFilterID);
    aFilter->initAttributes(anArgs);
  }
}

void FiltersPlugin_Selection::removeFilter(const std::string theFilterID)
{
  ModelAPI_FiltersFactory* aFactory = ModelAPI_Session::get()->filters();
  FilterPtr aFilter = aFactory->filter(theFilterID);
  if (aFilter.get()) {
    data()->removeAttributes(theFilterID);
  }
}

std::list<std::string> FiltersPlugin_Selection::filters() const
{
  std::list<std::string> aFilters;
  data()->allGroups(aFilters);
  return aFilters;
}

void FiltersPlugin_Selection::setReversed(const std::string theFilterID, const bool theReversed)
{
  std::string anAttrID = theFilterID + kFilterSeparator + kReverseAttrID;
  std::shared_ptr<ModelAPI_AttributeBoolean> aBool = boolean(anAttrID);
  if (aBool.get())
    aBool->setValue(theReversed);
}

bool FiltersPlugin_Selection::isReversed(const std::string theFilterID)
{
  std::string anAttrID = theFilterID + kFilterSeparator + kReverseAttrID;
  std::shared_ptr<ModelAPI_AttributeBoolean> aBool = boolean(anAttrID);
  if (aBool.get())
    return aBool->value();
  return false;
}

std::list<AttributePtr> FiltersPlugin_Selection::filterArgs(const std::string theFilterID) const
{
  std::list<AttributePtr> aList;
  data()->attributesOfGroup(theFilterID, aList);
  return aList;
}

void FiltersPlugin_Selection::setAttribute(const AttributePtr& theAttr)
{
  myBase = theAttr;
  if (myBase.get()) { // check that the type of sub-elements is supported by all existing filters
    std::shared_ptr<ModelAPI_AttributeSelectionList> aSelList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttr);
    if (aSelList.get()) {
      std::string aStrType = aSelList->selectionType();
      GeomAPI_Shape::ShapeType aType = GeomAPI_Shape::shapeTypeByStr(aStrType);
      std::list<std::string> aFilters;
      data()->allGroups(aFilters);
      ModelAPI_FiltersFactory* aFactory = ModelAPI_Session::get()->filters();
      std::list<std::string>::iterator aFIter = aFilters.begin();
      for(; aFIter != aFilters.end(); aFIter++) {
        FilterPtr aFilter = aFactory->filter(*aFIter);
        if (aFilter.get() && !aFilter->isSupported(aType)) {
          data()->removeAttributes(*aFIter);
        }
      }
    }
  }
}

const AttributePtr& FiltersPlugin_Selection::baseAttribute() const
{
  return myBase;
}

void FiltersPlugin_Selection::initAttributes()
{
  ModelAPI_FiltersFactory* aFactory = ModelAPI_Session::get()->filters();
  std::list<std::string> aFilters;
  data()->allGroups(aFilters);
  for(std::list<std::string>::iterator aFIt = aFilters.begin(); aFIt != aFilters.end(); aFIt++) {
    FilterPtr aFilter = aFactory->filter(*aFIt);
    if (aFilter.get()) {
      std::shared_ptr<ModelAPI_AttributeBoolean> aBool =
        std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(data()->addFloatingAttribute(
          kReverseAttrID, ModelAPI_AttributeBoolean::typeId(), *aFIt));
      aBool->setValue(false); // not reversed by default
      ModelAPI_FiltersArgs anArgs;
      anArgs.setFeature(std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(data()->owner()));
      anArgs.setFilter(*aFIt);
      aFilter->initAttributes(anArgs);
    }
  }
}
