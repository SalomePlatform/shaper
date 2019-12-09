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

#include <Model_ResultBody.h>

#include <Model_Document.h>
#include <Model_Objects.h>
#include <Model_BodyBuilder.h>
#include <Model_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <Model_Data.h>
#include <Events_Loop.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeExplorer.h>

#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TDataStd_UAttribute.hxx>

// if this attribute exists, the shape is connected topology
Standard_GUID kIsConnectedTopology("e51392e0-3a4d-405d-8e36-bbfe19858ef5");
// if this attribute exists, the connected topology flag must be recomputed
Standard_GUID kUpdateConnectedTopology("01ef7a45-0bec-4266-b0b4-4aa570921818");

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

bool Model_ResultBody::generated(const GeomShapePtr& theNewShape,
  const std::string& theName, const bool theCheckIsInResult)
{
  bool aResult = false;
  if (mySubs.size()) { // consists of subs
    for (std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
         aSubIter != mySubs.cend();
         ++aSubIter)
    {
      const ResultBodyPtr& aSub = *aSubIter;
      if (aSub->generated(theNewShape, theName, theCheckIsInResult))
        aResult = true;
    }
  } else { // do for this directly
    if (myBuilder->generated(theNewShape, theName, theCheckIsInResult))
      aResult = true;
  }
  return aResult;
}

void Model_ResultBody::loadGeneratedShapes(const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgo,
                                           const GeomShapePtr& theOldShape,
                                           const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                           const std::string& theName,
                                           const bool theSaveOldIfNotInTree)
{
  if (mySubs.size()) { // consists of subs
    for (std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
         aSubIter != mySubs.cend();
         ++aSubIter)
    {
      const ResultBodyPtr& aSub = *aSubIter;
      aSub->loadGeneratedShapes(
        theAlgo, theOldShape, theShapeTypeToExplore, theName, theSaveOldIfNotInTree);
    }
  } else { // do for this directly
    myBuilder->loadGeneratedShapes(
      theAlgo, theOldShape, theShapeTypeToExplore, theName, theSaveOldIfNotInTree);
  }
}

void Model_ResultBody::loadModifiedShapes(const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgo,
                                          const GeomShapePtr& theOldShape,
                                          const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                          const std::string& theName)
{
  if (mySubs.size()) { // consists of subs
    // optimization of getting of new shapes for specific sub-result
    if (!theAlgo->isNewShapesCollected(theOldShape, theShapeTypeToExplore))
      theAlgo->collectNewShapes(theOldShape, theShapeTypeToExplore);
    std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
    for(; aSubIter != mySubs.cend(); aSubIter++) {
      (*aSubIter)->loadModifiedShapes(theAlgo, theOldShape, theShapeTypeToExplore, theName);
    }
  } else { // do for this directly
    myBuilder->loadModifiedShapes(theAlgo, theOldShape, theShapeTypeToExplore, theName);
  }
}

void Model_ResultBody::loadFirstLevel(GeomShapePtr theShape, const std::string& theName)
{
  if (mySubs.size()) { // consists of subs
    for (std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
      aSubIter != mySubs.cend();
      ++aSubIter)
    {
      const ResultBodyPtr& aSub = *aSubIter;
      aSub->loadFirstLevel(theShape, theName);
    }
  } else { // do for this directly
    myBuilder->loadFirstLevel(theShape, theName);
  }
}

int Model_ResultBody::numberOfSubs(bool forTree) const
{
  return int(mySubs.size());
}

ResultBodyPtr Model_ResultBody::subResult(const int theIndex, bool forTree) const
{
  if (theIndex >= int(mySubs.size()))
    return ResultBodyPtr();
  return mySubs.at(theIndex);
}

bool Model_ResultBody::isSub(ObjectPtr theResult, int& theIndex) const
{
  std::map<ObjectPtr, int>::const_iterator aFound = mySubsMap.find(theResult);
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
    updateSubs(shape(), false); // to set disabled/enabled
  }
  return aChanged;
}

bool Model_ResultBody::isConcealed()
{
  return myLastConcealed;
}

void Model_ResultBody::setIsConcealed(const bool theValue, const bool theForced)
{
  if (ModelAPI_ResultBody::isConcealed() != theValue) {
    ModelAPI_ResultBody::setIsConcealed(theValue, theForced);
    updateConcealment();
  }
}

// recursively check all subs for concealment flag, returns true if everybody have "flag" state,
// in theAll returns results with "flag" state
static bool checkAllSubs(ResultBodyPtr theParent, bool theFlag, std::list<ResultBodyPtr>& theAll)
{
  if (theParent->isConcealed() != theFlag)
    theAll.push_back(theParent);
  bool aResult = theParent->ModelAPI_ResultBody::isConcealed() == theFlag;
  for(int a = 0; a < theParent->numberOfSubs(); a++) {
    bool aSubRes = checkAllSubs(theParent->subResult(a), theFlag, theAll);
    if (theFlag)
      aResult = aResult || aSubRes; // concealed: one makes concealed everyone
    else
      aResult = aResult && aSubRes; // not concealed: all must be not concealed
  }
  return aResult;
}

void Model_ResultBody::updateConcealment()
{
  if (myLastConcealed != ModelAPI_ResultBody::isConcealed()) {
    // check the whole tree of results: if one is concealed, everybody are concealed
    ResultBodyPtr anOwner = std::dynamic_pointer_cast<ModelAPI_ResultBody>(data()->owner());
    if (!anOwner.get())
      return; // "this" is invalid
    ResultBodyPtr aParent = ModelAPI_Tools::bodyOwner(anOwner);
    while(aParent.get()) {
      anOwner = aParent;
      aParent = ModelAPI_Tools::bodyOwner(anOwner);
    }
    // iterate all results and collect all results whose state may be updated
    std::list<ResultBodyPtr> anUpdated;
    bool aNewFlag = !myLastConcealed;
    if (checkAllSubs(anOwner, aNewFlag, anUpdated)) { // state of everyone must be updated
      std::list<ResultBodyPtr>::iterator aRes = anUpdated.begin();
      for(; aRes != anUpdated.end(); aRes++) {
        bool aLastConcealed = (*aRes)->isConcealed();
        if (aNewFlag != aLastConcealed) {
          std::dynamic_pointer_cast<Model_ResultBody>(*aRes)->myLastConcealed = aNewFlag;
          if (aNewFlag) { // become concealed, behaves like removed
            ModelAPI_EventCreator::get()->sendDeleted(document(), groupName());
          } else { // become not-concealed, behaves like created
            static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
            ModelAPI_EventCreator::get()->sendUpdated(*aRes, anEvent);
          }
          static Events_ID EVENT_DISP = // must be redisplayed in any case
            Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
          ModelAPI_EventCreator::get()->sendUpdated(*aRes, EVENT_DISP);
        }
      }
    }
  }
}

void Model_ResultBody::updateSubs(const std::shared_ptr<GeomAPI_Shape>& theThisShape,
                                  const bool theShapeChanged)
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static Events_ID EVENT_UPD = aLoop->eventByName(EVENT_OBJECT_UPDATED);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  // erase flag that topology is connected: the shape is new
  if (theShapeChanged && data().get()) {
    TDF_Label aDataLab = std::dynamic_pointer_cast<Model_Data>(data())->label();
    if (!aDataLab.IsNull()) {
      TDataStd_UAttribute::Set(aDataLab, kUpdateConnectedTopology);
      isConnectedTopology(); // to store this flag in transaction, #2630
    }
  }
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
        if (isConcealed()) { // for issue #2579 note7
          aSub->ModelAPI_ResultBody::setIsConcealed(true);
          std::dynamic_pointer_cast<Model_ResultBody>(aSub)->updateConcealment();
        }
      } else { // just update shape of this result
        aSub = mySubs[aSubIndex];
      }
      GeomShapePtr anOldSubShape = aSub->shape();
      if (!aShape->isEqual(anOldSubShape)) {
        if (myAlgo.get()) {
          std::list<GeomShapePtr> anOldForSub;
          computeOldForSub(aShape, myOlds, anOldForSub);
          myIsGenerated ? aSub->storeGenerated(anOldForSub, aShape, myAlgo) :
            aSub->storeModified(anOldForSub, aShape, myAlgo);
        } else {
          aSub->store(aShape, false);
        }
        aECreator->sendUpdated(aSub, EVENT_DISP);
        aECreator->sendUpdated(aSub, EVENT_UPD);
      }
      aSub->setDisabled(aSub, isDisabled());
    }
    // erase left, unused results
    while(mySubs.size() > aSubIndex) {
      ResultBodyPtr anErased = *(mySubs.rbegin());
      if (anErased->ModelAPI_ResultBody::isConcealed()) {
        anErased->ModelAPI_ResultBody::setIsConcealed(false);
        std::dynamic_pointer_cast<Model_ResultBody>(anErased)->updateConcealment();
      }
      anErased->setDisabled(anErased, true);
      mySubsMap.erase(anErased);
      mySubs.pop_back();
    }
    if (aWasEmpty) { // erase all subs
      // redisplay this because result with and without subs are displayed differently
      aECreator->sendUpdated(data()->owner(), EVENT_DISP);
    }
    cleanCash();
  } else if (!mySubs.empty()) { // erase all subs
    while(!mySubs.empty()) {
      ResultBodyPtr anErased = *(mySubs.rbegin());
      if (anErased->ModelAPI_ResultBody::isConcealed()) {
        anErased->ModelAPI_ResultBody::setIsConcealed(false);
        std::dynamic_pointer_cast<Model_ResultBody>(anErased)->updateConcealment();
      }
      anErased->setDisabled(anErased, true); // even if it is invalid (to erase subs on abort/undo)
      mySubs.pop_back();
    }
    mySubsMap.clear();
    // redisplay this because result with and without subs are displayed differently
    aECreator->sendUpdated(data()->owner(), EVENT_DISP);
  }
}

void Model_ResultBody::updateSubs(
  const GeomShapePtr& theThisShape, const std::list<GeomShapePtr>& theOlds,
  const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape, const bool isGenerated)
{
  myAlgo = theMakeShape;
  myOlds = theOlds;
  myIsGenerated = isGenerated;
  // to avoid changing of "isDisabled" flag in the "updateSubs" cycle
  isDisabled();

  updateSubs(theThisShape, true);
  myAlgo.reset();
  myOlds.clear();
  myHistoryCash.Clear();
}


bool Model_ResultBody::isConnectedTopology()
{
  TDF_Label aDataLab = std::dynamic_pointer_cast<Model_Data>(data())->label();
  if (!aDataLab.IsNull()) {
    if (aDataLab.IsAttribute(kUpdateConnectedTopology)) { // recompute state
      aDataLab.ForgetAttribute(kUpdateConnectedTopology);
      GeomShapePtr aShape = shape();
      if (aShape.get() && aShape->isConnectedTopology()) {
        TDataStd_UAttribute::Set(aDataLab, kIsConnectedTopology);
      } else {
        aDataLab.ForgetAttribute(kIsConnectedTopology);
      }
    }
    return aDataLab.IsAttribute(kIsConnectedTopology);
  }
  return false; // invalid case
}

void Model_ResultBody::cleanCash()
{
  myBuilder->cleanCash();
  for (std::vector<ResultBodyPtr>::const_iterator aSubIter = mySubs.cbegin();
    aSubIter != mySubs.cend(); ++aSubIter)
  {
    const ResultBodyPtr& aSub = *aSubIter;
    aSub->cleanCash();
  }
}

// adds to the theSubSubs map all sub-shapes of theSub if it is compound of compsolid
static void collectSubs(
  const GeomShapePtr theSub, TopTools_MapOfShape& theSubSubs, const bool theOneLevelMore)
{
  if (theSub->isNull())
    return;
  if (theSubSubs.Add(theSub->impl<TopoDS_Shape>()))  {
    bool aIsComp = theSub->isCompound() || theSub->isCompSolid();
    if (aIsComp) {
      for(GeomAPI_ShapeIterator anIter(theSub); anIter.more(); anIter.next())
        collectSubs(anIter.current(), theSubSubs, theOneLevelMore);
    } else if (theOneLevelMore) {
      GeomAPI_Shape::ShapeType aSubType = GeomAPI_Shape::ShapeType(int(theSub->shapeType()) + 1);
      if (aSubType == GeomAPI_Shape::SHAPE)
        return;
      if (aSubType == GeomAPI_Shape::SHELL)
        aSubType = GeomAPI_Shape::FACE;
      if (aSubType == GeomAPI_Shape::WIRE)
        aSubType = GeomAPI_Shape::EDGE;

      for(GeomAPI_ShapeExplorer anExp(theSub, aSubType); anExp.more(); anExp.next()) {
        collectSubs(anExp.current(), theSubSubs, false);
      }
    }
  }
}

void Model_ResultBody::computeOldForSub(const GeomShapePtr& theSub,
  const std::list<GeomShapePtr>& theAllOlds, std::list<GeomShapePtr>& theOldForSub)
{
  // the old can be also used for sub-shape of theSub; collect all subs of compound or compsolid
  TopTools_MapOfShape aSubSubs;
  collectSubs(theSub, aSubSubs, false);

  std::list<GeomShapePtr>::const_iterator aRootOlds = theAllOlds.cbegin();
  for (; aRootOlds != theAllOlds.cend(); aRootOlds++) {
    // use sub-shapes of olds too if they are compounds or compsolids
    TopTools_MapOfShape anOldSubs;
    // iterate one level more (for intersection of solids this is face)
    collectSubs(*aRootOlds, anOldSubs, true);
    for (TopTools_MapOfShape::Iterator anOldIter(anOldSubs); anOldIter.More(); anOldIter.Next()) {
      TopoDS_Shape anOldShape = anOldIter.Value();
      if (anOldShape.ShapeType() == TopAbs_COMPOUND || anOldShape.ShapeType() == TopAbs_SHELL ||
          anOldShape.ShapeType() == TopAbs_WIRE)
        continue; // container old-shapes are not supported by the history, may cause crash
      GeomShapePtr anOldSub(new GeomAPI_Shape);
      anOldSub->setImpl<TopoDS_Shape>(new TopoDS_Shape(anOldShape));

      ListOfShape aNews;
      if (myHistoryCash.IsBound(anOldShape)) {
        const TopTools_ListOfShape& aList = myHistoryCash.Find(anOldShape);
        for(TopTools_ListIteratorOfListOfShape anIter(aList); anIter.More(); anIter.Next()) {
          GeomShapePtr aShape(new GeomAPI_Shape);
          aShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(anIter.Value()));
          aNews.push_back(aShape);
        }
      } else {
        myIsGenerated ? myAlgo->generated(anOldSub, aNews) : myAlgo->modified(anOldSub, aNews);
        // MakeShape may return alone old shape if there is no history information for this input
        if (aNews.size() == 1 && aNews.front()->isEqual(anOldSub))
          aNews.clear();
        // store result in the history
        TopTools_ListOfShape aList;
        for (ListOfShape::iterator aNewIter = aNews.begin(); aNewIter != aNews.end(); aNewIter++) {
          aList.Append((*aNewIter)->impl<TopoDS_Shape>());
        }
        myHistoryCash.Bind(anOldShape, aList);
      }

      for (ListOfShape::iterator aNewIter = aNews.begin(); aNewIter != aNews.end(); aNewIter++) {
        if (aSubSubs.Contains((*aNewIter)->impl<TopoDS_Shape>())) {
          // check list already contains this sub
          std::list<GeomShapePtr>::iterator aResIter = theOldForSub.begin();
          for(; aResIter != theOldForSub.end(); aResIter++)
            if ((*aResIter)->isSame(anOldSub))
              break;
          if (aResIter == theOldForSub.end())
            theOldForSub.push_back(anOldSub); // found old used for new theSubShape creation
          break;
        }
      }
    }
  }
}
