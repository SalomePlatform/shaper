// File:        ModelAPI_AttributeRefList.cxx
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefList.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <Events_Loop.h>
#include <TDF_ListIteratorOfLabelList.hxx>

using namespace std;

void Model_AttributeRefList::append(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  boost::shared_ptr<Model_Data> aData = 
    boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
  myRef->Append(aData->label());

  static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
  Model_FeatureUpdatedMessage aMsg(owner(), anEvent);
  Events_Loop::loop()->send(aMsg);
}

void Model_AttributeRefList::remove(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  boost::shared_ptr<Model_Data> aData = 
    boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
  myRef->Remove(aData->label());

}

int Model_AttributeRefList::size()
{
  return myRef->Extent();
}

list<boost::shared_ptr<ModelAPI_Feature> > Model_AttributeRefList::list()
{
  std::list< boost::shared_ptr<ModelAPI_Feature> > aResult;
  boost::shared_ptr<Model_Document> aDoc = 
    boost::dynamic_pointer_cast<Model_Document>(owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    for(TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
      aResult.push_back(aDoc->feature(aLIter.Value()));
    }
  }
  return aResult;
}

Model_AttributeRefList::Model_AttributeRefList(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_ReferenceList::GetID(), myRef)) {
    myRef = TDataStd_ReferenceList::Set(theLabel);
  }
}
