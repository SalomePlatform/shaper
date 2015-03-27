// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeReference.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeReference.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Session.h>

#include <TDataStd_Comment.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDF_Tool.hxx>

using namespace std;

void Model_AttributeReference::setValue(ObjectPtr theObject)
{
  if(!theObject)
    return;
  if (!myIsInitialized || value() != theObject) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(
        theObject->data());
    TDF_Label anObjLab = aData->label().Father(); // object label

    if (owner()->document() == theObject->document()) { // same document, use reference attribute

      std::shared_ptr<Model_Document> aDoc =
        std::dynamic_pointer_cast<Model_Document>(owner()->document());
      myRef->Set(anObjLab);  // references to the object label
       // remove external link attributes (if any)
      myRef->Label().ForgetAttribute(TDataStd_Comment::GetID());
      myRef->Label().ForgetAttribute(TDataStd_AsciiString::GetID());
    } else { // different document: store the document name (comment) and entry (string): external
      // if these attributes exist, the link is external: keep reference to access the label
      TDataStd_Comment::Set(myRef->Label(), theObject->document()->id().c_str());
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(anObjLab, anEntry);
      TDataStd_AsciiString::Set(myRef->Label(), anEntry);
    }
    // do it before the transaction finish to make just created/removed objects know dependencies
    // and reference from composite feature is removed automatically
    FeaturePtr anOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
    if (anOwnerFeature.get()) {
      aData->addBackReference(anOwnerFeature, id(), false);
    }

    owner()->data()->sendAttributeUpdated(this);
  }
}

ObjectPtr Model_AttributeReference::value()
{
  if (myIsInitialized) {
    Handle(TDataStd_Comment) aDocID;
    if (myRef->Label().FindAttribute(TDataStd_Comment::GetID(), aDocID)) { // external ref
      DocumentPtr aRefDoc =
        ModelAPI_Session::get()->document(TCollection_AsciiString(aDocID->Get()).ToCString());
      if (aRefDoc) {
        Handle(TDataStd_AsciiString) anEntry;
        if (myRef->Label().FindAttribute(TDataStd_AsciiString::GetID(), anEntry)) {
          std::shared_ptr<Model_Document> aDR = std::dynamic_pointer_cast<Model_Document>(aRefDoc);
          TDF_Label aRefLab;
          TDF_Tool::Label(aDR->featuresLabel().Data(), anEntry->Get().ToCString(), aRefLab);
          if (!aRefLab.IsNull()) {
            return aDR->object(aRefLab);
          }
        }
      }
    } else { // internal ref
      std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
          owner()->document());
      if (aDoc) {
        TDF_Label aRefLab = myRef->Get();
        if (!aRefLab.IsNull()) {  // it may happen with old document, issue #285
          return aDoc->object(aRefLab);
        }
      }
    }
  }
  // not initialized
  return FeaturePtr();
}

Model_AttributeReference::Model_AttributeReference(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDF_Reference::GetID(), myRef) == Standard_True;
  if (!myIsInitialized) {
    myRef = TDF_Reference::Set(theLabel, theLabel);  // not initialized references to itself
  } else {
    if (owner()) {
      std::shared_ptr<Model_Document> aDoc =
        std::dynamic_pointer_cast<Model_Document>(owner()->document());
    }
  }
}

void Model_AttributeReference::setObject(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  if (owner() != theObject) {
    ModelAPI_AttributeReference::setObject(theObject);
    std::shared_ptr<Model_Document> aDoc =
      std::dynamic_pointer_cast<Model_Document>(owner()->document());
  }
}

Model_AttributeReference::~Model_AttributeReference()
{
  std::shared_ptr<Model_Document> aDoc =
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  TDF_Label aLab = myRef->Get();
}
