// Copyright (C) 2014-2025  CEA, EDF
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

#include "CollectionAPI_Group.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <Locale_Convert.h>
#include <Python.h>
#include <PyInterp_Interp.h>

//==================================================================================================
CollectionAPI_Group::CollectionAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
CollectionAPI_Group::CollectionAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theGroupList)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setGroupList(theGroupList);
  }
}

//==================================================================================================
CollectionAPI_Group::~CollectionAPI_Group()
{

}

//==================================================================================================
void CollectionAPI_Group::setGroupList(const std::list<ModelHighAPI_Selection>& theGroupList)
{
  fillAttribute(theGroupList, mygroupList);

  // Checks the content of the selection inside the filters
  // If this content is empty (old scripts using filters), the group is created without filters
  // A warning is sent in the Python console
  if (isFiltersSelectionEmpty()) {
    mygroupList->removeFilters();
    std::string aFeatureName = Locale::Convert::toString(name());
    PyLockWrapper lck;
    std::string aMessage = "WARNING! The filters in " + aFeatureName + " have been removed: " +\
               "the selection argument of one of them was empty.";
    PySys_WriteStdout("%s\n", aMessage.c_str());
  }

  execute();
}

//==================================================================================================
bool CollectionAPI_Group::isFiltersSelectionEmpty() const
{
  FiltersFeaturePtr aFiltersFeature = mygroupList->filters();
  if (aFiltersFeature.get()) {
    for (std::string const& aFilter: aFiltersFeature->filters()) {
      std::list<AttributePtr> aListArgs = aFiltersFeature->filterArgs(aFilter);
      for (AttributePtr const& anAttr: aListArgs) {
        std::shared_ptr<ModelAPI_AttributeSelectionList> aSelList =
          std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(anAttr);
        if (aSelList.get()) {
          if (aSelList->size() == 0) {
            return true; // filter argument is an empty selection list attribute
          }
        }
        std::shared_ptr<ModelAPI_AttributeSelection> aSel =
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(anAttr);
        if (aSel.get()) {
          if (!aSel->isInitialized()) {
            return true; // filter argument is an empty selection attribute
          }
        }
      }
    }
  }
  return false;
}

//==================================================================================================
void CollectionAPI_Group::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrList = aBase->selectionList(CollectionPlugin_Group::LIST_ID());

  theDumper << aBase << " = model.addGroup(" << aDocName << ", ";
  if (anAttrList->isWholeResultAllowed() && !anAttrList->selectionType().empty())
    theDumper<<"\""<<anAttrList->selectionType()<<"\", ";
  theDumper << anAttrList;
  if (anAttrList->isGeometricalSelection())
    theDumper <<", True";
  theDumper << ")" << std::endl;
}

//==================================================================================================
GroupPtr addGroup(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theGroupList,
                  const bool theShareSameTopology)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(CollectionAPI_Group::ID());
  if (theShareSameTopology)
    aFeature->selectionList(CollectionPlugin_Group::LIST_ID())->setGeometricalSelection(true);
  return GroupPtr(new CollectionAPI_Group(aFeature, theGroupList));
}

//==================================================================================================
GroupPtr addGroup(const std::shared_ptr<ModelAPI_Document>& thePart,
  const std::string& theSelectionType,
  const std::list<ModelHighAPI_Selection>& theGroupList,
  const bool theShareSameTopology)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(CollectionAPI_Group::ID());
  aFeature->selectionList(CollectionPlugin_Group::LIST_ID())->setSelectionType(theSelectionType);
  if (theShareSameTopology)
    aFeature->selectionList(CollectionPlugin_Group::LIST_ID())->setGeometricalSelection(true);
  return GroupPtr(new CollectionAPI_Group(aFeature, theGroupList));
}
