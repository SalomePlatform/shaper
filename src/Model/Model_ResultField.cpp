// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultField.cpp
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultField.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <Model_Document.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <Config_PropManager.h>

Model_ResultField::Model_ResultField(std::shared_ptr<ModelAPI_Data> theOwnerData)
{
  myOwnerData = theOwnerData;
}

void Model_ResultField::colorConfigInfo(std::string& theSection, std::string& theName,
                                       std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_field_color";
  theDefault = DEFAULT_COLOR();
}

std::shared_ptr<GeomAPI_Shape> Model_ResultField::shape()
{
  std::shared_ptr<GeomAPI_Shape> aResult;
  if (myOwnerData) {
    AttributeSelectionListPtr aList = myOwnerData->selectionList("selected");
    if (aList) {
      std::list<std::shared_ptr<GeomAPI_Shape> > aSubs;
      if (aList->selectionType() == "part") { // all result bodies of the part
        std::shared_ptr<Model_Document> aDoc =
          std::dynamic_pointer_cast<Model_Document>(document());
        if (!aDoc.get())
          return aResult;
        FeaturePtr aThisFeature = document()->feature(
          std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner()));
        int aResults = document()->size(ModelAPI_ResultBody::group());
        for(int a = 0; a < aResults; a++) {
          ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(
            document()->object(ModelAPI_ResultBody::group(), a));
          if (!aBody.get())
            continue;
          // check that only results that were created before this field are used
          FeaturePtr aResultFeature = document()->feature(aBody);
          if (!aResultFeature.get())
            continue;
          if (aDoc->isLater(aResultFeature, aThisFeature))
            continue;
          GeomShapePtr aShape = aBody->shape();
          if (!aShape.get() || aShape->isNull())
            continue;
          aSubs.push_back(aShape);
        }
      } else {
        for(int a = aList->size() - 1; a >= 0; a--) {
          std::shared_ptr<GeomAPI_Shape> aSelection = aList->value(a)->value();
          if (aSelection && !aSelection->isNull()) {
            aSubs.push_back(aSelection);
          }
        }
      }
      if (!aSubs.empty()) {
        aResult = GeomAlgoAPI_CompoundBuilder::compound(aSubs);
      }
    }
  }
  return aResult;
}
