// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultCompSolid.cpp
// Created:     20 Jul 2015
// Author:      Natalia ERMOLAEVA

#include <Model_ResultCompSolid.h>

#include <Model_Document.h>
#include <Model_Objects.h>
#include <Model_BodyBuilder.h>
#include <Model_Document.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <Events_Loop.h>

#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>


Model_ResultCompSolid::Model_ResultCompSolid()
{
  myBuilder = new Model_BodyBuilder(this);
  setIsConcealed(false);
  myIsDisabled = true; // by default it is not initialized and false to be after created
  updateSubs(shape()); // in case of open, etc.
}

Model_ResultCompSolid::~Model_ResultCompSolid()
{
  updateSubs(std::shared_ptr<GeomAPI_Shape>()); // erase sub-results
}

void Model_ResultCompSolid::initAttributes()
{
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::typeId());
}

void Model_ResultCompSolid::store(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  ModelAPI_ResultCompSolid::store(theShape);
  updateSubs(theShape);
}

void Model_ResultCompSolid::storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
    const std::shared_ptr<GeomAPI_Shape>& theToShape)
{
  ModelAPI_ResultCompSolid::storeGenerated(theFromShape, theToShape);
  updateSubs(theToShape);
}

void Model_ResultCompSolid::storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const int theDecomposeSolidsTag)
{
  ModelAPI_ResultCompSolid::storeModified(theOldShape, theNewShape, theDecomposeSolidsTag);
  updateSubs(theNewShape);
}

int Model_ResultCompSolid::numberOfSubs(bool forTree) const
{
  return mySubs.size();
}

std::shared_ptr<ModelAPI_ResultBody> Model_ResultCompSolid::subResult(const int theIndex,
                                                                      bool forTree) const
{
  return mySubs.at(theIndex);
}

bool Model_ResultCompSolid::isSub(ObjectPtr theObject) const
{
  std::vector<std::shared_ptr<ModelAPI_ResultBody> >::const_iterator aSubIter = mySubs.cbegin();
  for(; aSubIter != mySubs.cend(); aSubIter++)
    if (*aSubIter == theObject)
      return true;
  return false;
}

void Model_ResultCompSolid::colorConfigInfo(std::string& theSection, std::string& theName,
  std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_body_color";
  theDefault = DEFAULT_COLOR();
}

bool Model_ResultCompSolid::setDisabled(std::shared_ptr<ModelAPI_Result> theThis, const bool theFlag)
{
  bool aChanged = ModelAPI_ResultBody::setDisabled(theThis, theFlag);
  if (aChanged) { // state is changed, so modifications are needed
    myBuilder->evolutionToSelection(theFlag);
    updateSubs(shape()); // to set disabled/enabled 
  }
  return aChanged;
}

bool Model_ResultCompSolid::isConcealed()
{
  if (ModelAPI_ResultCompSolid::isConcealed())
    return true;
  std::vector<std::shared_ptr<ModelAPI_ResultBody> >::const_iterator aSubIter = mySubs.cbegin();
  for(; aSubIter != mySubs.cend(); aSubIter++)
    if ((*aSubIter)->isConcealed())
      return true;
  return false;
}

void Model_ResultCompSolid::updateSubs(const std::shared_ptr<GeomAPI_Shape>& theThisShape)
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static Events_ID EVENT_UPD = aLoop->eventByName(EVENT_OBJECT_UPDATED);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  // iterate all sub-solids of compsolid to make sub-results synchronized with them
  TopoDS_Shape aThisShape;
  if (theThisShape.get()) aThisShape = theThisShape->impl<TopoDS_Shape>();
  if (!aThisShape.IsNull() && (aThisShape.ShapeType() == TopAbs_COMPSOLID ||
       aThisShape.ShapeType() == TopAbs_COMPOUND)) {
    bool aWasEmpty = mySubs.empty();
    Model_Objects* anObjects = std::dynamic_pointer_cast<Model_Document>(document())->objects();
    unsigned int aSubIndex = 0;
    TopExp_Explorer aSolids(aThisShape, TopAbs_SOLID);
    for(; aSolids.More(); aSolids.Next(), aSubIndex++) {
      std::shared_ptr<GeomAPI_Shape> aSolidShape(new GeomAPI_Shape);
      aSolidShape->setImpl(new TopoDS_Shape(aSolids.Current()));
      ResultBodyPtr aSub;
      if (mySubs.size() <= aSubIndex) { // it is needed to create a new sub-result
        aSub = anObjects->createBody(this->data(), aSubIndex);
        mySubs.push_back(aSub);
      } else { // just update shape of this result
        aSub = mySubs[aSubIndex];
      }
      if (!aSolidShape->isEqual(aSub->shape())) {
        aSub->store(aSolidShape);
        aECreator->sendUpdated(aSub, EVENT_DISP);
        aECreator->sendUpdated(aSub, EVENT_UPD);
      }
      aSub->setDisabled(aSub, isDisabled());
    }
    // erase left, unused results
    while(mySubs.size() > aSubIndex) {
      ResultBodyPtr anErased = *(mySubs.rbegin());
      anErased->setDisabled(anErased, true);
      mySubs.pop_back();
    }
    if (aWasEmpty) { // erase all subs
      // redisplay this because result with and without subs are displayed differently
      aECreator->sendUpdated(data()->owner(), EVENT_DISP);
    }
  } else if (!mySubs.empty()) { // erase all subs
    while(!mySubs.empty()) {
      ResultBodyPtr anErased = *(mySubs.rbegin());
      anErased->setDisabled(anErased, true);
      mySubs.pop_back();
    }
    // redisplay this because result with and without subs are displayed differently
    aECreator->sendUpdated(data()->owner(), EVENT_DISP);
  }
}
