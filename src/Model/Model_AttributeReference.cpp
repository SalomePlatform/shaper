// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeReference.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeReference.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include "Model_Objects.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Session.h>

#include <TDataStd_Comment.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDF_Tool.hxx>

using namespace std;

void Model_AttributeReference::setValue(ObjectPtr theObject)
{
  // now allow to deselect in this attribute: extrusion from/to
  //if(!theObject)
  //  return;
  ObjectPtr aValue = value();
  if (!myIsInitialized || aValue != theObject) {
    REMOVE_BACK_REF(aValue);

    TDF_Label anObjLab;
    if (theObject.get() && theObject->data()->isValid()) {
      std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(
        theObject->data());
      anObjLab = aData->label().Father(); // object label
    }
    // same document, use reference attribute
    if (anObjLab.IsNull() || owner()->document() == theObject->document()) {

      if (anObjLab.IsNull()) {
        myRef->Set(myRef->Label());
      } else {
        myRef->Set(anObjLab);  // references to the object label
      }
       // remove external link attributes (if any)
      myRef->Label().ForgetAttribute(TDataStd_Comment::GetID());
      myRef->Label().ForgetAttribute(TDataStd_AsciiString::GetID());
    } else { // different document: store the document name (comment) and entry (string): external
      // if these attributes exist, the link is external: keep reference to access the label
      std::ostringstream anIdString; // string with document Id
      anIdString<<theObject->document()->id();
      TDataStd_Comment::Set(myRef->Label(), anIdString.str().c_str());
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(anObjLab, anEntry);
      TDataStd_AsciiString::Set(myRef->Label(), anEntry);
    }
    // do it before the transaction finish to make just created/removed objects know dependencies
    // and reference from composite feature is removed automatically
    ADD_BACK_REF(theObject);

    owner()->data()->sendAttributeUpdated(this);
  }
}

ObjectPtr Model_AttributeReference::value()
{
  if (isInitialized()) {
    Handle(TDataStd_Comment) aDocID;
    if (myRef->Label().FindAttribute(TDataStd_Comment::GetID(), aDocID)) { // external ref
      int anID = atoi(TCollection_AsciiString(aDocID->Get()).ToCString());
      DocumentPtr aRefDoc = Model_Application::getApplication()->document(anID);
      if (aRefDoc.get()) {
        Handle(TDataStd_AsciiString) anEntry;
        if (myRef->Label().FindAttribute(TDataStd_AsciiString::GetID(), anEntry)) {
          std::shared_ptr<Model_Document> aDR = std::dynamic_pointer_cast<Model_Document>(aRefDoc);
          TDF_Label aRefLab;
          TDF_Tool::Label(aDR->objects()->featuresLabel().Data(), anEntry->Get().ToCString(), aRefLab);
          if (!aRefLab.IsNull()) {
            return aDR->objects()->object(aRefLab);
          }
        }
      }
    } else { // internal ref
      std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
          owner()->document());
      if (aDoc) {
        TDF_Label aRefLab = myRef->Get();
        if (!aRefLab.IsNull()) {  // it may happen with old document, issue #285
          return aDoc->objects()->object(aRefLab);
        }
      }
    }
  }
  // not initialized
  return FeaturePtr();
}

bool Model_AttributeReference::isInitialized()
{
  if (myRef->Label() == myRef->Get() && !myRef->Label().IsAttribute(TDataStd_Comment::GetID())) {
    // empty reference is not initialized
    return false;
  }
  return ModelAPI_AttributeReference::isInitialized();
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
{}
