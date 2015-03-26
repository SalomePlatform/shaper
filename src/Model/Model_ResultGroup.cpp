// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultGroup.cpp
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultGroup.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeIntArray.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <Config_PropManager.h>

Model_ResultGroup::Model_ResultGroup(std::shared_ptr<ModelAPI_Data> theOwnerData)
{
  setIsConcealed(false);
  myOwnerData = theOwnerData;
}

void Model_ResultGroup::initAttributes()
{
  // append the color attribute. It is empty, the attribute will be filled by a request
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::typeId());
}

void Model_ResultGroup::colorConfigInfo(std::string& theSection, std::string& theName,
                                       std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_group_color";
  theDefault = DEFAULT_COLOR();
}

std::shared_ptr<GeomAPI_Shape> Model_ResultGroup::shape()
{
  std::shared_ptr<GeomAPI_Shape> aResult;
  if (myOwnerData) {
    AttributeSelectionListPtr aList = myOwnerData->selectionList("group_list");
    if (aList) {
      std::list<std::shared_ptr<GeomAPI_Shape> > aSubs;
      for(int a = aList->size() - 1; a >= 0; a--) {
        std::shared_ptr<GeomAPI_Shape> aSelection = aList->value(a)->value();
        if (aSelection && !aSelection->isNull()) {
          aSubs.push_back(aSelection);
        }
      }
      if (!aSubs.empty()) {
        aResult = GeomAlgoAPI_CompoundBuilder::compound(aSubs);
      }
    }
  }
  return aResult;
}
