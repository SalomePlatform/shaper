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
  // append the color attribute
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::type());
  AttributeIntArrayPtr aColorAttr = aData->intArray(COLOR_ID());
  std::vector<int> aRGB;
  aRGB = Config_PropManager::color("Visualization", "result_group_color", DEFAULT_COLOR());
  aColorAttr->setSize(3);
  aColorAttr->setValue(0, aRGB[0]);
  aColorAttr->setValue(1, aRGB[1]);
  aColorAttr->setValue(2, aRGB[2]);
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
