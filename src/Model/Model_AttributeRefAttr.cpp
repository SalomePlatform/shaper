// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "Model_AttributeRefAttr.h"
#include "Model_Application.h"
#include "Model_Data.h"
#include "Model_Objects.h"
#include <ModelAPI_Feature.h>

bool Model_AttributeRefAttr::isObject()
{
  return myID->Get().Length() == 0;
}

void Model_AttributeRefAttr::setAttr(std::shared_ptr<ModelAPI_Attribute> theAttr)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(
      theAttr->owner()->data());
  std::string anID = aData->id(theAttr);
  ObjectPtr anObj = object();
  if (myIsInitialized && anObj == theAttr->owner() && myID->Get().IsEqual(anID.c_str()))
    return;  // nothing is changed
  REMOVE_BACK_REF(anObj);
  myRef->Set(aData->label().Father());
  myID->Set(aData->id(theAttr).c_str());
  ADD_BACK_REF(theAttr->owner());
  owner()->data()->sendAttributeUpdated(this);
}

std::shared_ptr<ModelAPI_Attribute> Model_AttributeRefAttr::attr()
{
  ObjectPtr anObj = object();
  if (anObj && anObj->data()) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(anObj->data());
    return aData->attribute(TCollection_AsciiString(myID->Get()).ToCString());
  }
  // not initialized
  return std::shared_ptr<ModelAPI_Attribute>();
}

void Model_AttributeRefAttr::setObject(ObjectPtr theObject)
{
  // the back reference from the previous object to the attribute should be removed
  ObjectPtr anObject = object();
  if (theObject.get() && (!myIsInitialized || myID->Get().Length() != 0 || object() != theObject)) {
    REMOVE_BACK_REF(anObject);

    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(
        theObject->data());
    myRef->Set(aData->label().Father());
    myID->Set("");  // feature is identified by the empty ID

    // do it before the transaction finish to make just created/removed objects know dependencies
    // and reference from composite feature is removed automatically
    FeaturePtr anOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
    if (anOwnerFeature.get()) {
      aData->addBackReference(anOwnerFeature, id(), false);
    }
    ADD_BACK_REF(theObject);
    owner()->data()->sendAttributeUpdated(this);
  } else if (theObject.get() == NULL) {
    REMOVE_BACK_REF(anObject);
    myRef->Set(myRef->Label()); // reference to itself means that object is null
    myID->Set("");  // feature is identified by the empty ID
    owner()->data()->sendAttributeUpdated(this);
  }
}

ObjectPtr Model_AttributeRefAttr::object()
{
  if (myRef->Get() != myRef->Label()) {  // initialized
    std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
        owner()->document());
    if (aDoc) {
      TDF_Label aRefLab = myRef->Get();
      return aDoc->objects()->object(aRefLab);
    }
  }
  // not initialized
  return ObjectPtr();
}

bool Model_AttributeRefAttr::isInitialized()
{
  if (myRef->Get() == myRef->Label()) { // empty is not initialized: sketch parallelity
    return false;
  }
  return ModelAPI_AttributeRefAttr::isInitialized();
}

Model_AttributeRefAttr::Model_AttributeRefAttr(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeRefAttr::reinit()
{
  myIsInitialized = myLab.FindAttribute(TDataStd_Comment::GetID(), myID) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet
    myID = TDataStd_Comment::Set(myLab, "");
    myRef = TDF_Reference::Set(myLab, myLab);  // not initialized: reference to itself
  } else {
    myLab.FindAttribute(TDF_Reference::GetID(), myRef);
  }
}
