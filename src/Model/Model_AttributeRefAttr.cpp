// File:        ModelAPI_AttributeRefAttr.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefAttr.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <Events_Loop.h>

using namespace std;

void Model_AttributeRefAttr::setValue(boost::shared_ptr<ModelAPI_Attribute> theAttr)
{
  if (value() != theAttr) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>(theAttr->feature()->data());
    if (myRef.IsNull()) {
      boost::shared_ptr<Model_Data> aMyData = 
        boost::dynamic_pointer_cast<Model_Data>(feature()->data());
      TDF_Reference::Set(aMyData->label(), aData->label());
    } else {
      myRef->Set(aData->label());
    }
    myID->Set(aData->id(theAttr).c_str());

    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(feature(), anEvent);
    Events_Loop::loop()->send(aMsg);
  }
}

boost::shared_ptr<ModelAPI_Attribute> Model_AttributeRefAttr::value()
{
  if (!myRef.IsNull()) {
    boost::shared_ptr<Model_Document> aDoc = 
      boost::dynamic_pointer_cast<Model_Document>(feature()->document());
    if (aDoc) {
      TDF_Label aRefLab = myRef->Get();
      TDF_Label aFeatureLab = aRefLab.Father();
      boost::shared_ptr<Model_Data> aData = 
        boost::dynamic_pointer_cast<Model_Data>(aDoc->feature(aRefLab)->data());
      return aData->attribute(TCollection_AsciiString(myID->Get()).ToCString());
    }
  }
  // not initialized
  return boost::shared_ptr<ModelAPI_Attribute>();
}

Model_AttributeRefAttr::Model_AttributeRefAttr(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_Comment::GetID(), myID)) {
    // create attribute: not initialized by value yet
    TDataStd_Comment::Set(theLabel, "");
    // reference attribute is not set to the label!
  }
}
