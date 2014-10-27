// File:        Model_AttributeSelectionList.h
// Created:     22 Oct 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeSelectionList.h"
#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"

#include <TDF_ChildIterator.hxx>

using namespace std;

void Model_AttributeSelectionList::append(
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  int aNewTag = mySize->Get() + 1;
  TDF_Label aNewLab = mySize->Label().FindChild(aNewTag);

  boost::shared_ptr<Model_AttributeSelection> aNewAttr = 
    boost::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aNewLab));
  if (owner()) {
    aNewAttr->setObject(owner());
  }
  mySubs.push_back(aNewAttr);
  mySize->Set(aNewTag);
  aNewAttr->setValue(theContext, theSubShape);
  owner()->data()->sendAttributeUpdated(this);
}

int Model_AttributeSelectionList::size()
{
  return mySize->Get();
}

boost::shared_ptr<ModelAPI_AttributeSelection> 
  Model_AttributeSelectionList::value(const int theIndex)
{
  return mySubs[theIndex];
}

void Model_AttributeSelectionList::clear()
{
  if (!mySubs.empty()) {
    mySize->Set(0);
    mySubs.clear();
    TDF_ChildIterator aSubIter(mySize->Label());
    for(; aSubIter.More(); aSubIter.Next()) {
      aSubIter.Value().ForgetAllAttributes(Standard_True);
    }
    owner()->data()->sendAttributeUpdated(this);
  }
}

Model_AttributeSelectionList::Model_AttributeSelectionList(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), mySize) == Standard_True;
  if (!myIsInitialized) {
    mySize = TDataStd_Integer::Set(theLabel, 0);
  } else { // recollect mySubs
    int aNum = mySize->Get();
    TDF_ChildIterator aSubIter(theLabel);
    for(; aSubIter.More(), aNum != 0; aSubIter.Next(), aNum--) {
      TDF_Label aChildLab = aSubIter.Value();
      boost::shared_ptr<Model_AttributeSelection> aNewAttr = 
        boost::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aChildLab));
      if (owner())
        aNewAttr->setObject(owner());
      mySubs.push_back(aNewAttr);
    }
  }
}

void Model_AttributeSelectionList::setObject(const boost::shared_ptr<ModelAPI_Object>& theObject)
{
  ModelAPI_AttributeSelectionList::setObject(theObject);
  std::vector<boost::shared_ptr<Model_AttributeSelection> >::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++) {
    (*aSubIter)->setObject(theObject);
  }
}
