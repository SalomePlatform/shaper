// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <Model_ResultCompSolid.h>

#include <Model_Document.h>
#include <Model_Objects.h>
#include <Model_BodyBuilder.h>
#include <Model_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <Events_Loop.h>

#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>


Model_ResultCompSolid::Model_ResultCompSolid()
{
  myBuilder = new Model_BodyBuilder(this);
  myLastConcealed = false;
  updateSubs(shape()); // in case of open, etc.
}

Model_ResultCompSolid::~Model_ResultCompSolid()
{
  updateSubs(std::shared_ptr<GeomAPI_Shape>()); // erase sub-results
}

void Model_ResultCompSolid::store(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                  const bool theIsStoreSameShapes)
{
  ModelAPI_ResultCompSolid::store(theShape, theIsStoreSameShapes);
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

void Model_ResultCompSolid::loadAndOrientModifiedShapes(GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape, const int  theTag,
    const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes,
    const bool theIsStoreSeparate,
    const bool theIsStoreAsGenerated,
    const bool theSplitInSubs)
{
  if (theSplitInSubs && mySubs.size()) { // consists of subs
    std::vector<std::shared_ptr<ModelAPI_ResultBody> >::const_iterator aSubIter = mySubs.cbegin();
    for(; aSubIter != mySubs.cend(); aSubIter++) {
      (*aSubIter)->loadAndOrientModifiedShapes(
        theMS, theShapeIn, theKindOfShape, theTag, theName, theSubShapes, theIsStoreSeparate,
        theIsStoreAsGenerated);
    }
  } else { // do for this directly
    ModelAPI_ResultCompSolid::loadAndOrientModifiedShapes(
    theMS, theShapeIn, theKindOfShape, theTag, theName, theSubShapes, theIsStoreSeparate,
    theIsStoreAsGenerated);
  }
}


int Model_ResultCompSolid::numberOfSubs(bool forTree) const
{
  return int(mySubs.size());
}

std::shared_ptr<ModelAPI_ResultBody> Model_ResultCompSolid::subResult(const int theIndex,
                                                                      bool forTree) const
{
  return mySubs.at(theIndex);
}

bool Model_ResultCompSolid::isSub(ObjectPtr theObject, int& theIndex) const
{
  std::map<ObjectPtr, int>::const_iterator aFound = mySubsMap.find(theObject);
  if (aFound != mySubsMap.end()) {
    theIndex = aFound->second;
    return true;
  }
  return false;
}

void Model_ResultCompSolid::colorConfigInfo(std::string& theSection, std::string& theName,
  std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_body_color";
  theDefault = DEFAULT_COLOR();
}

bool Model_ResultCompSolid::setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
                                        const bool theFlag)
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
  bool aResult = false;;
  if (ModelAPI_ResultCompSolid::isConcealed()) {
    aResult = true;
  } else {
    std::vector<std::shared_ptr<ModelAPI_ResultBody> >::const_iterator aSubIter = mySubs.cbegin();
    for(; aSubIter != mySubs.cend(); aSubIter++) {
      if ((*aSubIter)->ModelAPI_ResultBody::isConcealed()) {
        aResult = true;
        break;
      }
    }
  }
  if (myLastConcealed != aResult) {
    myLastConcealed = aResult;
    //setIsConcealed(aResult); // set for all subs the same result
    std::vector<std::shared_ptr<ModelAPI_ResultBody> >::const_iterator aSubIter = mySubs.cbegin();
    for(; aSubIter != mySubs.cend(); aSubIter++) { // update the visualization status of each sub
      if ((*aSubIter)->ModelAPI_ResultBody::isConcealed() != aResult) {
        if (aResult) { // hidden unit must be redisplayed (hidden)
          ModelAPI_EventCreator::get()->sendDeleted(document(), (*aSubIter)->groupName());
          // redisplay for the viewer (it must be disappeared also)
          static Events_ID EVENT_DISP =
            Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
          ModelAPI_EventCreator::get()->sendUpdated(*aSubIter, EVENT_DISP);
        } else { // was not concealed become concealed => delete event
          static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
          ModelAPI_EventCreator::get()->sendUpdated(*aSubIter, anEvent);
        }
      }
    }
    // update the display state of the subs: explicitly call Model_ResultBody::isConcealed
    for(aSubIter = mySubs.cbegin(); aSubIter != mySubs.cend(); aSubIter++) {
      (*aSubIter)->isConcealed();
    }
  }
  return aResult;
}

void Model_ResultCompSolid::setIsConcealed(const bool theValue)
{
  if (theValue != ModelAPI_ResultCompSolid::isConcealed()) {
    std::vector<std::shared_ptr<ModelAPI_ResultBody> >::const_iterator aSubIter = mySubs.cbegin();
    for(; aSubIter != mySubs.cend(); aSubIter++) {
      if ((*aSubIter)->ModelAPI_ResultBody::isConcealed() != theValue) {
        if (theValue) { // hidden unit must be redisplayed (hidden)
          ModelAPI_EventCreator::get()->sendDeleted(document(), (*aSubIter)->groupName());
          // redisplay for the viewer (it must be disappeared also)
          static Events_ID EVENT_DISP =
            Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
          ModelAPI_EventCreator::get()->sendUpdated(*aSubIter, EVENT_DISP);
        } else { // was not concealed become concealed => delete event
          static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
          ModelAPI_EventCreator::get()->sendUpdated(*aSubIter, anEvent);
        }
      }
    }
    ModelAPI_ResultCompSolid::setIsConcealed(theValue);
    // to set correct myLastConcealed
    isConcealed();
  }
  //myLastConcealed = theValue;
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
    TopoDS_Iterator aShapesIter(aThisShape);
    for(; aShapesIter.More(); aShapesIter.Next(), aSubIndex++) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
      aShape->setImpl(new TopoDS_Shape(aShapesIter.Value()));
      ResultBodyPtr aSub;
      if (mySubs.size() <= aSubIndex) { // it is needed to create a new sub-result
        aSub = anObjects->createBody(this->data(), aSubIndex);
        mySubs.push_back(aSub);
        mySubsMap[aSub] = int(mySubs.size() - 1);
      } else { // just update shape of this result
        aSub = mySubs[aSubIndex];
      }
      if (!aShape->isEqual(aSub->shape())) {
        aSub->store(aShape, false);
        aECreator->sendUpdated(aSub, EVENT_DISP);
        aECreator->sendUpdated(aSub, EVENT_UPD);
      }
      aSub->setDisabled(aSub, isDisabled());
      aSub->setIsConcealed(myLastConcealed);
    }
    // erase left, unused results
    while(mySubs.size() > aSubIndex) {
      ResultBodyPtr anErased = *(mySubs.rbegin());
      anErased->setDisabled(anErased, true);
      mySubsMap.erase(anErased);
      mySubs.pop_back();
    }
    if (aWasEmpty) { // erase all subs
      // redisplay this because result with and without subs are displayed differently
      aECreator->sendUpdated(data()->owner(), EVENT_DISP);
    }
  } else if (!mySubs.empty()) { // erase all subs
    while(!mySubs.empty()) {
      ResultBodyPtr anErased = *(mySubs.rbegin());
      anErased->setDisabled(anErased, true); // even if it is invalid (to erase subs on abort/undo)
      mySubs.pop_back();
    }
    mySubsMap.clear();
    // redisplay this because result with and without subs are displayed differently
    aECreator->sendUpdated(data()->owner(), EVENT_DISP);
  }
}

bool Model_ResultCompSolid::isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (myBuilder->isLatestEqual(theShape))
    return true;
  // also check that it is asked for sub-elements
  std::vector<std::shared_ptr<ModelAPI_ResultBody> >::const_iterator aSubIter = mySubs.cbegin();
  for(; aSubIter != mySubs.cend(); aSubIter++) {
    if (aSubIter->get() && (*aSubIter)->isLatestEqual(theShape)) {
      return true;
    }
  }
  return false;
}
