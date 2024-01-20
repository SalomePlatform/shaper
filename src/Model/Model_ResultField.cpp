// Copyright (C) 2014-2024  CEA, EDF
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

#include <Model_ResultField.h>
#include <Model_Document.h>

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <Locale_Convert.h>

#include <Config_PropManager.h>

Model_ResultField::Model_ResultField(std::shared_ptr<ModelAPI_Data> theOwnerData)
{
  myOwnerData = theOwnerData;
}

Model_ResultField::~Model_ResultField()
{
  while(mySteps.size() > 0) {
    //delete mySteps.back();
    mySteps.pop_back();
  }
}

// LCOV_EXCL_START
void Model_ResultField::colorConfigInfo(std::string& theSection, std::string& theName,
                                       std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_field_color";
  theDefault = DEFAULT_COLOR();
}
// LCOV_EXCL_STOP

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
            aSubs.push_front(aSelection);
          }
        }
      }
      if (!aSubs.empty()) {
        aResult = GeomAlgoAPI_CompoundBuilder::compound(aSubs);
      }
    }
    updateSteps();
  }
  return aResult;
}

void Model_ResultField::updateSteps()
{
  // Update Array of steps
  int aNbSteps = stepsSize();
  if ((int)mySteps.size() != aNbSteps) {
    while((int)mySteps.size() > aNbSteps) {
      //delete mySteps.back();
      mySteps.pop_back();
    }
    while((int)mySteps.size() < aNbSteps) {
      mySteps.push_back(FieldStepPtr(new Model_ResultField::Model_FieldStep(this,
        (int)mySteps.size())));
    }
  }
}

int Model_ResultField::stepsSize() const
{
  if (myOwnerData) {
    AttributeIntArrayPtr aArray = myOwnerData->intArray("stamps");
    if (aArray.get()) {
      return aArray->size();
    }
  }
  return 0;
}

std::string Model_ResultField::textLine(int theLine) const
{
  if (myOwnerData) {
    AttributeIntArrayPtr aArray = myOwnerData->intArray("stamps");
    if (aArray.get()) {
      if (theLine < aArray->size()) {
        std::ostringstream aStream;
        aStream << aArray->value(theLine);
        return aStream.str();
      }
    }
  }
  return "";
}

// used by GUI only
// LCOV_EXCL_START
std::shared_ptr<ModelAPI_ResultField::ModelAPI_FieldStep> Model_ResultField::step(int theId) const
{
  if (theId < (int)mySteps.size()) {
    return mySteps[theId];
  }
  return NULL;
}

std::wstring Model_ResultField::Model_FieldStep::name() {
  std::wostringstream aStream;
  aStream<<myParent->data()->name()<<std::endl;
  aStream<<"Step "<<(myId + 1)<<" "<< Locale::Convert::toWString(myParent->textLine(myId));
  return aStream.str();
}
// LCOV_EXCL_STOP
