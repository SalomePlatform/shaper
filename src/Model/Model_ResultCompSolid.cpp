// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultCompSolid.cpp
// Created:     20 Jul 2015
// Author:      Natalia ERMOLAEVA

#include <Model_ResultCompSolid.h>

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Object.h>

#include <Model_Document.h>

Model_ResultCompSolid::Model_ResultCompSolid()
{
}

Model_ResultCompSolid::~Model_ResultCompSolid()
{
}

void Model_ResultCompSolid::initAttributes()
{
  data()->addAttribute(Model_ResultCompSolid::BODIES_ID(), ModelAPI_AttributeRefList::typeId());
}

std::shared_ptr<ModelAPI_ResultBody> Model_ResultCompSolid::addResult(const int theIndex)
{
  std::shared_ptr<ModelAPI_ResultBody> aBody = document()->createBody(data(), theIndex);
  if (aBody.get()) {
    data()->reflist(Model_ResultCompSolid::BODIES_ID())->append(aBody);
  }
  return aBody;
}

int Model_ResultCompSolid::numberOfSubs(bool forTree) const
{
  return 0;
}

std::shared_ptr<ModelAPI_ResultBody> Model_ResultCompSolid::subResult(const int theIndex,
                                                                      bool forTree) const
{
  if (forTree) {
    std::shared_ptr<ModelAPI_ResultBody> aBody;
    return aBody;
  }

  ObjectPtr anObj = data()->reflist(Model_ResultCompSolid::BODIES_ID())->object(theIndex);
  return std::dynamic_pointer_cast<ModelAPI_ResultBody>(anObj);
}

/*int Model_ResultCompSolid::subResultId(const int theIndex) const
{
  return subResult(theIndex)->data()->featureId();
}*/

bool Model_ResultCompSolid::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  ResultBodyPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theObject);
  /*if (!aFeature) {
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aRes)
      aFeature = document()->feature(aRes);
  }*/
  if (aResult) {
    return data()->reflist(Model_ResultCompSolid::BODIES_ID())->isInList(aResult);
  }
  return false;
}

void Model_ResultCompSolid::removeResult(std::shared_ptr<ModelAPI_ResultBody> theResult)
{
  if (!data()->isValid()) // sketch is already removed (case on undo of sketch), sync is not needed
    return;

  std::list<ObjectPtr> aSubs = data()->reflist(Model_ResultCompSolid::BODIES_ID())->list();

  std::list<ObjectPtr>::iterator aSubIt = aSubs.begin(), aLastIt = aSubs.end();
  bool isRemoved = false;
  bool aHasEmtpyResult = false;
  for(; aSubIt != aLastIt && !isRemoved; aSubIt++) {
    std::shared_ptr<ModelAPI_ResultBody> aResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aSubIt);
    if (aResult.get() != NULL && aResult == theResult) {
      data()->reflist(Model_ResultCompSolid::BODIES_ID())->remove(aResult);
      isRemoved = true;
    }
    else if (aResult.get() == NULL)
      aHasEmtpyResult = true;
  }
  // if the object is not found in the sketch sub-elements, that means that the object is removed already.
  // Find the first empty element and remove it
  if (!isRemoved && aHasEmtpyResult)
    data()->reflist(Model_ResultCompSolid::BODIES_ID())->remove(ObjectPtr());
}
