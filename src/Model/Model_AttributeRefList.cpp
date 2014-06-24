// File:        ModelAPI_AttributeRefList.cxx
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefList.h"
#include "Model_Application.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <TDF_ListIteratorOfLabelList.hxx>

using namespace std;

void Model_AttributeRefList::append(FeaturePtr theFeature)
{
  boost::shared_ptr<Model_Data> aData = 
    boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
  myRef->Append(aData->label());

  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeRefList::remove(FeaturePtr theFeature)
{
  boost::shared_ptr<Model_Data> aData = 
    boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
  myRef->Remove(aData->label());

  owner()->data()->sendAttributeUpdated(this);
}

int Model_AttributeRefList::size()
{
  return myRef->Extent();
}

list<FeaturePtr > Model_AttributeRefList::list()
{
  std::list< FeaturePtr > aResult;
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
  myIsInitialized = theLabel.FindAttribute(TDataStd_ReferenceList::GetID(), myRef) == Standard_True;
  if (!myIsInitialized) {
    myRef = TDataStd_ReferenceList::Set(theLabel);
  }
}
