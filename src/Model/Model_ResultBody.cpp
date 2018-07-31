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

#include <Model_ResultBody.h>

#include <Model_Document.h>
#include <Model_Objects.h>
#include <Model_BodyBuilder.h>
#include <Model_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <Events_Loop.h>

#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>


Model_ResultBody::Model_ResultBody() : ModelAPI_ResultBody()
{
  myBuilder = new Model_BodyBuilder(this);
  myLastConcealed = false;
  updateSubs(shape()); // in case of open, etc.
}

Model_ResultBody::~Model_ResultBody()
{
  updateSubs(std::shared_ptr<GeomAPI_Shape>()); // erase sub-results
  delete myBuilder;
}

void Model_ResultBody::loadAndOrientModifiedShapes(GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape, const int  theTag,
    const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes,
    const bool theIsStoreSeparate,
    const bool theIsStoreAsGenerated,
    const bool theSplitInSubs)
{
  if (theSplitInSubs && mySubs.size()) { // consists of subs
    std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
    for(; aSubIter != mySubs.cend(); aSubIter++) {
      // check that sub-shape was also created as modification of ShapeIn
      /* to find when it is needed later to enable: to store modification of sub-bodies not only as primitives
      GeomShapePtr aSubGeomShape = (*aSubIter)->shape();
      if (!theIsStoreAsGenerated && aSubGeomShape.get() && !aSubGeomShape->isNull()) {
        TopoDS_Shape aSubShape = aSubGeomShape->impl<TopoDS_Shape>();
        TopoDS_Shape aWholeIn = theShapeIn->impl<TopoDS_Shape>();
        for(TopExp_Explorer anExp(aWholeIn, aSubShape.ShapeType()); anExp.More(); anExp.Next()) {
          ListOfShape aHistory;
          std::shared_ptr<GeomAPI_Shape> aSubIn(new GeomAPI_Shape());
          aSubIn->setImpl((new TopoDS_Shape(anExp.Current())));
          theMS->modified(aSubIn, aHistory);
          std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = aHistory.begin();
          for (; anIt != aHistory.end(); anIt++) {
            if ((*anIt)->isSame(aSubGeomShape)) {
              (*aSubIter)->storeModified(aSubIn, aSubGeomShape, -2); // -2 is to avoid clearing
            }
          }
        }
      }*/
      (*aSubIter)->loadAndOrientModifiedShapes(
        theMS, theShapeIn, theKindOfShape, theTag, theName, theSubShapes, theIsStoreSeparate,
        theIsStoreAsGenerated);
    }
  } else { // do for this directly
    myBuilder->loadAndOrientGeneratedShapes(
      theMS, theShapeIn, theKindOfShape, theTag, theName, theSubShapes);
  }
}

int Model_ResultBody::numberOfSubs(bool forTree) const
{
  return int(mySubs.size());
}

ResultBodyPtr Model_ResultBody::subResult(const int theIndex, bool forTree) const
{
  return mySubs.at(theIndex);
}

bool Model_ResultBody::isSub(ObjectPtr theObject, int& theIndex) const
{
  std::map<ObjectPtr, int>::const_iterator aFound = mySubsMap.find(theObject);
  if (aFound != mySubsMap.end()) {
    theIndex = aFound->second;
    return true;
  }
  return false;
}

void Model_ResultBody::colorConfigInfo(std::string& theSection, std::string& theName,
  std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_body_color";
  theDefault = DEFAULT_COLOR();
}

bool Model_ResultBody::setDisabled(std::shared_ptr<ModelAPI_Result> theThis, const bool theFlag)
{
  bool aChanged = ModelAPI_ResultBody::setDisabled(theThis, theFlag);
  if (aChanged) { // state is changed, so modifications are needed
    myBuilder->evolutionToSelection(theFlag);
    updateSubs(shape()); // to set disabled/enabled
  }
  return aChanged;
}

bool Model_ResultBody::isConcealed()
{
  return myLastConcealed;
}

void Model_ResultBody::setIsConcealed(const bool theValue)
{
  if (ModelAPI_ResultBody::isConcealed() != theValue) {
    ModelAPI_ResultBody::setIsConcealed(theValue);
    updateConcealment();
  }
}

void Model_ResultBody::updateConcealment()
{
  if (myLastConcealed != ModelAPI_ResultBody::isConcealed()) {
    ResultPtr anOwner = std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner());
    std::shared_ptr<Model_ResultBody> aParent = std::dynamic_pointer_cast<Model_ResultBody>(
      ModelAPI_Tools::bodyOwner(anOwner));

    myLastConcealed = ModelAPI_ResultBody::isConcealed(); // set new value and check parent
    if (myLastConcealed) { // this becomes concealed, so, update all: parent and children
      if (aParent.get())
        aParent->updateConcealment();
      static Events_ID EVENT_DISP =
        Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
      ModelAPI_EventCreator::get()->sendDeleted(document(), groupName());
      ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), EVENT_DISP);
      std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
      for (; aSubIter != mySubs.cend(); aSubIter++) {
        std::dynamic_pointer_cast<Model_ResultBody>(*aSubIter)->updateConcealment();
      }
    } else {
      // ask parent: if it is still concealed, nothing is changed
      if (aParent.get()) {
        aParent->updateConcealment();
        if (aParent->isConcealed()) {
          myLastConcealed = true;
          return;
        }
      }
      // iterate children: if they are concealed, nothing is changed
      bool aChildConcealed = false;
      std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
      for (; aSubIter != mySubs.cend(); aSubIter++) {
        std::dynamic_pointer_cast<Model_ResultBody>(*aSubIter)->updateConcealment();
        if ((*aSubIter)->isConcealed()) {
          aChildConcealed = true;
          break;
        }
      }
      if (aChildConcealed) { // some child is concealed, so, update back
        myLastConcealed = true;
        if (aParent.get())
          aParent->updateConcealment();
        std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
        for (; aSubIter != mySubs.cend(); aSubIter++) {
          std::dynamic_pointer_cast<Model_ResultBody>(*aSubIter)->updateConcealment();
        }
      } else { // so, it becomes unconcealed
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
        ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), anEvent);
        static Events_ID EVENT_DISP =
          Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
        ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), EVENT_DISP);
      }
    }
  }
}

void Model_ResultBody::updateSubs(const std::shared_ptr<GeomAPI_Shape>& theThisShape)
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
    }
    // erase left, unused results
    while(mySubs.size() > aSubIndex) {
      ResultBodyPtr anErased = *(mySubs.rbegin());
      if (anErased->ModelAPI_ResultBody::isConcealed())
        std::dynamic_pointer_cast<Model_ResultBody>(anErased)->updateConcealment();
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
      if (anErased->ModelAPI_ResultBody::isConcealed())
        std::dynamic_pointer_cast<Model_ResultBody>(anErased)->updateConcealment();
      anErased->setDisabled(anErased, true); // even if it is invalid (to erase subs on abort/undo)
      mySubs.pop_back();
    }
    mySubsMap.clear();
    // redisplay this because result with and without subs are displayed differently
    aECreator->sendUpdated(data()->owner(), EVENT_DISP);
  }
}

bool Model_ResultBody::isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (myBuilder->isLatestEqual(theShape))
    return true;
  // also check that it is asked for sub-elements
  std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
  for(; aSubIter != mySubs.cend(); aSubIter++) {
    if (aSubIter->get() && (*aSubIter)->isLatestEqual(theShape)) {
      return true;
    }
  }
  return false;
}
