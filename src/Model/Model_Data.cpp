// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Data.h>
#include <Model_AttributeDocRef.h>
#include <Model_AttributeInteger.h>
#include <Model_AttributeDouble.h>
#include <Model_AttributeReference.h>
#include <Model_AttributeRefAttr.h>
#include <Model_AttributeRefList.h>
#include <Model_AttributeBoolean.h>
#include <Model_AttributeString.h>
#include <Model_AttributeSelection.h>
#include <Model_AttributeSelectionList.h>
#include <Model_AttributeIntArray.h>
#include <Model_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <GeomData_Point.h>
#include <GeomData_Point2D.h>
#include <GeomData_Dir.h>
#include <Events_Loop.h>
#include <Events_Error.h>

#include <TDataStd_Name.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_RelocationTable.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_UAttribute.hxx>

#include <string>

// myLab contains:
// TDataStd_Name - name of the object
// TDataStd_Integer - state of the object execution

Model_Data::Model_Data() : mySendAttributeUpdated(true)
{
}

void Model_Data::setLabel(TDF_Label theLab)
{
  myLab = theLab;
}

std::string Model_Data::name()
{
  Handle(TDataStd_Name) aName;
  if (myLab.FindAttribute(TDataStd_Name::GetID(), aName))
    return std::string(TCollection_AsciiString(aName->Get()).ToCString());
  return "";  // not defined
}

void Model_Data::setName(const std::string& theName)
{
  bool isModified = false;
  Handle(TDataStd_Name) aName;
  if (!myLab.FindAttribute(TDataStd_Name::GetID(), aName)) {
    TDataStd_Name::Set(myLab, theName.c_str());
    isModified = true;
  } else {
    isModified = !aName->Get().IsEqual(theName.c_str());
    if (isModified)
      aName->Set(theName.c_str());
  }
}

AttributePtr Model_Data::addAttribute(const std::string& theID, const std::string theAttrType)
{
  AttributePtr aResult;
  TDF_Label anAttrLab = myLab.FindChild(myAttrs.size() + 1);
  ModelAPI_Attribute* anAttr = 0;
  if (theAttrType == ModelAPI_AttributeDocRef::typeId()) {
    anAttr = new Model_AttributeDocRef(anAttrLab);
  } else if (theAttrType == Model_AttributeInteger::typeId()) {
    anAttr = new Model_AttributeInteger(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeDouble::typeId()) {
    anAttr = new Model_AttributeDouble(anAttrLab);
  } else if (theAttrType == Model_AttributeBoolean::typeId()) {
    anAttr = new Model_AttributeBoolean(anAttrLab);
  } else if (theAttrType == Model_AttributeString::typeId()) {
    anAttr = new Model_AttributeString(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeReference::typeId()) {
    anAttr = new Model_AttributeReference(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeSelection::typeId()) {
    anAttr = new Model_AttributeSelection(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeSelectionList::typeId()) {
    anAttr = new Model_AttributeSelectionList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefAttr::typeId()) {
    anAttr = new Model_AttributeRefAttr(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefList::typeId()) {
    anAttr = new Model_AttributeRefList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeIntArray::typeId()) {
    anAttr = new Model_AttributeIntArray(anAttrLab);
  } 
  // create also GeomData attributes here because only here the OCAF strucure is known
  else if (theAttrType == GeomData_Point::typeId()) {
    anAttr = new GeomData_Point(anAttrLab);
  } else if (theAttrType == GeomData_Dir::typeId()) {
    anAttr = new GeomData_Dir(anAttrLab);
  } else if (theAttrType == GeomData_Point2D::typeId()) {
    anAttr = new GeomData_Point2D(anAttrLab);
  }
  if (anAttr) {
    aResult = std::shared_ptr<ModelAPI_Attribute>(anAttr);
    myAttrs[theID] = aResult;
    anAttr->setObject(myObject);
    anAttr->setID(theID);
  } else {
    Events_Error::send("Can not create unknown type of attribute " + theAttrType);
  }
  return aResult;
}

// macro for gthe generic returning of the attribute by the ID
#define GET_ATTRIBUTE_BY_ID(ATTR_TYPE, METHOD_NAME) \
  std::shared_ptr<ATTR_TYPE> Model_Data::METHOD_NAME(const std::string& theID) { \
    std::shared_ptr<ATTR_TYPE> aRes; \
    std::map<std::string, AttributePtr >::iterator aFound = \
      myAttrs.find(theID); \
    if (aFound != myAttrs.end()) { \
      aRes = std::dynamic_pointer_cast<ATTR_TYPE>(aFound->second); \
    } \
    return aRes; \
  }
// implement nice getting methods for all ModelAPI attributes
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDocRef, document);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDouble, real);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeInteger, integer);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeBoolean, boolean);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeString, string);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeReference, reference);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeSelection, selection);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeSelectionList, selectionList);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefAttr, refattr);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefList, reflist);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeIntArray, intArray);

std::shared_ptr<ModelAPI_Attribute> Model_Data::attribute(const std::string& theID)
{
  std::shared_ptr<ModelAPI_Attribute> aResult;
  if (myAttrs.find(theID) == myAttrs.end())  // no such attribute
    return aResult;
  return myAttrs[theID];
}

const std::string& Model_Data::id(const std::shared_ptr<ModelAPI_Attribute>& theAttr)
{
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = 
    myAttrs.begin();
  for (; anAttr != myAttrs.end(); anAttr++) {
    if (anAttr->second == theAttr)
      return anAttr->first;
  }
  // not found
  static std::string anEmpty;
  return anEmpty;
}

bool Model_Data::isEqual(const std::shared_ptr<ModelAPI_Data>& theData)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theData);
  if (aData)
    return myLab.IsEqual(aData->myLab) == Standard_True ;
  return false;
}

bool Model_Data::isValid()
{
  return !myLab.IsNull() && myLab.HasAttribute();
}

std::list<std::shared_ptr<ModelAPI_Attribute> > Model_Data::attributes(const std::string& theType)
{
  std::list<std::shared_ptr<ModelAPI_Attribute> > aResult;
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
    myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    if (theType.empty() || anAttrsIter->second->attributeType() == theType) {
      aResult.push_back(anAttrsIter->second);
    }
  }
  return aResult;
}

std::list<std::string> Model_Data::attributesIDs(const std::string& theType) 
{
  std::list<std::string> aResult;
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
    myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    if (theType.empty() || anAttrsIter->second->attributeType() == theType) {
      aResult.push_back(anAttrsIter->first);
    }
  }
  return aResult;
}

void Model_Data::sendAttributeUpdated(ModelAPI_Attribute* theAttr)
{
  theAttr->setInitialized();
  if (theAttr->isArgument()) {
    static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
    ModelAPI_EventCreator::get()->sendUpdated(myObject, anEvent);
    if (mySendAttributeUpdated && myObject) {
      myObject->attributeChanged(theAttr->id());
    }
  }
}

void Model_Data::blockSendAttributeUpdated(const bool theBlock)
{
  mySendAttributeUpdated = !theBlock;
}

void Model_Data::erase()
{
  if (!myLab.IsNull())
    myLab.ForgetAllAttributes();
}

void Model_Data::execState(const ModelAPI_ExecState theState)
{
  if (theState != ModelAPI_StateNothing)
    TDataStd_Integer::Set(myLab, (int)theState);
}

ModelAPI_ExecState Model_Data::execState()
{
  Handle(TDataStd_Integer) aStateAttr;
  if (myLab.FindAttribute(TDataStd_Integer::GetID(), aStateAttr)) {
    return ModelAPI_ExecState(aStateAttr->Get());
  }
  return ModelAPI_StateMustBeUpdated; // default value
}

void Model_Data::setError(const std::string& theError, bool theSend)
{
  execState(ModelAPI_StateExecFailed);
  if (theSend) {
    Events_Error::send(theError);
  }
  TDataStd_AsciiString::Set(myLab, theError.c_str());
}

std::string Model_Data::error() const
{
  Handle(TDataStd_AsciiString) anErrorAttr;
  if (myLab.FindAttribute(TDataStd_AsciiString::GetID(), anErrorAttr)) {
    return std::string(anErrorAttr->Get().ToCString());
  }
  return std::string();
}

int Model_Data::featureId() const
{
  return myLab.Father().Tag(); // tag of the feature label
}

void Model_Data::eraseBackReferences()
{
  myRefsToMe.clear();
  std::shared_ptr<ModelAPI_Result> aRes = 
    std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
  if (aRes)
    aRes->setIsConcealed(false);
}

void Model_Data::removeBackReference(FeaturePtr theFeature, std::string theAttrID)
{
  AttributePtr anAttribute = theFeature->data()->attribute(theAttrID);
  if (myRefsToMe.find(anAttribute) == myRefsToMe.end())
    return;

  myRefsToMe.erase(anAttribute);
}

void Model_Data::addBackReference(FeaturePtr theFeature, std::string theAttrID, 
   const bool theApplyConcealment)
{
  // do not add the same attribute twice
  AttributePtr anAttribute = theFeature->data()->attribute(theAttrID);
  if (myRefsToMe.find(anAttribute) != myRefsToMe.end())
    return;

  myRefsToMe.insert(theFeature->data()->attribute(theAttrID));
  if (theApplyConcealment && 
      ModelAPI_Session::get()->validators()->isConcealed(theFeature->getKind(), theAttrID)) {
    std::shared_ptr<ModelAPI_Result> aRes = 
      std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
    if (aRes) {
      aRes->setIsConcealed(true);
    }
  }
}

void Model_Data::referencesToObjects(
  std::list<std::pair<std::string, std::list<ObjectPtr> > >& theRefs)
{
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = myAttrs.begin();
  std::list<ObjectPtr> aReferenced; // not inside of cycle to avoid excess memory menagement
  for(; anAttr != myAttrs.end(); anAttr++) {
    std::string aType = anAttr->second->attributeType();
    if (aType == ModelAPI_AttributeReference::typeId()) { // reference to object
      std::shared_ptr<ModelAPI_AttributeReference> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeReference>(anAttr->second);
      aReferenced.push_back(aRef->value());
    } else if (aType == ModelAPI_AttributeRefAttr::typeId()) { // reference to attribute or object
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(anAttr->second);
      aReferenced.push_back(aRef->isObject() ? aRef->object() : aRef->attr()->owner());
    } else if (aType == ModelAPI_AttributeRefList::typeId()) { // list of references
      aReferenced = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttr->second)->list();
    } else if (aType == ModelAPI_AttributeSelection::typeId()) { // selection attribute
      std::shared_ptr<ModelAPI_AttributeSelection> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeSelection>(anAttr->second);
      aReferenced.push_back(aRef->context());
    } else if (aType == ModelAPI_AttributeSelectionList::typeId()) { // list of selection attributes
      std::shared_ptr<ModelAPI_AttributeSelectionList> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeSelectionList>(anAttr->second);
      for(int a = aRef->size() - 1; a >= 0; a--) {
        aReferenced.push_back(aRef->value(a)->context());
      }
    } else
      continue; // nothing to do, not reference

    if (!aReferenced.empty()) {
      theRefs.push_back(std::pair<std::string, std::list<ObjectPtr> >(anAttr->first, aReferenced));
      aReferenced.clear();
    }
  }
}

/// makes copy of all attributes on the given label and all sub-labels
static void copyAttrs(TDF_Label theSource, TDF_Label theDestination) {
  TDF_AttributeIterator anAttrIter(theSource);
  for(; anAttrIter.More(); anAttrIter.Next()) {
    Handle(TDF_Attribute) aTargetAttr;
    if (!theDestination.FindAttribute(anAttrIter.Value()->ID(), aTargetAttr)) {
      // create a new attribute if not yet exists in the destination
	    aTargetAttr = anAttrIter.Value()->NewEmpty();
      theDestination.AddAttribute(aTargetAttr);
    }
    Handle(TDF_RelocationTable) aRelocTable = new TDF_RelocationTable(); // no relocation, empty map
    anAttrIter.Value()->Paste(aTargetAttr, aRelocTable);
  }
  // copy the sub-labels content
  TDF_ChildIterator aSubLabsIter(theSource);
  for(; aSubLabsIter.More(); aSubLabsIter.Next()) {
    copyAttrs(aSubLabsIter.Value(), theDestination.FindChild(aSubLabsIter.Value().Tag()));
  }
}

void Model_Data::copyTo(std::shared_ptr<ModelAPI_Data> theTarget)
{
  TDF_Label aTargetRoot = std::dynamic_pointer_cast<Model_Data>(theTarget)->label();
  copyAttrs(myLab, aTargetRoot);
  // make initialized the initialized attributes
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator aMyIter = myAttrs.begin();
  for(; aMyIter != myAttrs.end(); aMyIter++) {
    if (aMyIter->second->isInitialized()) {
      theTarget->attribute(aMyIter->first)->setInitialized();
    }
  }
}

const Standard_GUID kIsInHistory("9275e461-4aca-46c7-ad84-1efb569d8144");

bool Model_Data::isInHistory()
{
  return !myLab.IsAttribute(kIsInHistory);
}

void Model_Data::setIsInHistory(const bool theFlag)
{
  if (theFlag) { // is in histiry true: default behavior, so, remove GUID
    myLab.ForgetAttribute(kIsInHistory);
  } else { // not standard behavior is defined by special GUID attribute
    TDataStd_UAttribute::Set(myLab, kIsInHistory);
  }
}
