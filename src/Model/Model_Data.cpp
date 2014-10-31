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
#include <TDataStd_UAttribute.hxx>

#include <string>

Model_Data::Model_Data()
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

void Model_Data::addAttribute(const std::string& theID, const std::string theAttrType)
{
  TDF_Label anAttrLab = myLab.FindChild(myAttrs.size() + 1);
  ModelAPI_Attribute* anAttr = 0;
  if (theAttrType == ModelAPI_AttributeDocRef::type()) {
    anAttr = new Model_AttributeDocRef(anAttrLab);
  } else if (theAttrType == Model_AttributeInteger::type()) {
    anAttr = new Model_AttributeInteger(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeDouble::type()) {
    anAttr = new Model_AttributeDouble(anAttrLab);
  } else if (theAttrType == Model_AttributeBoolean::type()) {
    anAttr = new Model_AttributeBoolean(anAttrLab);
  } else if (theAttrType == Model_AttributeString::type()) {
    anAttr = new Model_AttributeString(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeReference::type()) {
    anAttr = new Model_AttributeReference(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeSelection::type()) {
    anAttr = new Model_AttributeSelection(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeSelectionList::type()) {
    anAttr = new Model_AttributeSelectionList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefAttr::type()) {
    anAttr = new Model_AttributeRefAttr(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefList::type()) {
    anAttr = new Model_AttributeRefList(anAttrLab);
  } else if (theAttrType == GeomData_Point::type()) {
    anAttr = new GeomData_Point(anAttrLab);
  } else if (theAttrType == GeomData_Dir::type()) {
    anAttr = new GeomData_Dir(anAttrLab);
  } else if (theAttrType == GeomData_Point2D::type()) {
    anAttr = new GeomData_Point2D(anAttrLab);
  }
  if (anAttr) {
    myAttrs[theID] = boost::shared_ptr<ModelAPI_Attribute>(anAttr);
    anAttr->setObject(myObject);
  } else {
    Events_Error::send("Can not create unknown type of attribute " + theAttrType);
  }
}

boost::shared_ptr<ModelAPI_AttributeDocRef> Model_Data::document(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeDocRef>();
  }
  boost::shared_ptr<ModelAPI_AttributeDocRef> aRes = boost::dynamic_pointer_cast<
      ModelAPI_AttributeDocRef>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeDouble> Model_Data::real(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeDouble>();
  }
  boost::shared_ptr<ModelAPI_AttributeDouble> aRes = boost::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeInteger> Model_Data::integer(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeInteger>();
  }
  boost::shared_ptr<ModelAPI_AttributeInteger> aRes = boost::dynamic_pointer_cast<
      ModelAPI_AttributeInteger>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeBoolean> Model_Data::boolean(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeBoolean>();
  }
  boost::shared_ptr<ModelAPI_AttributeBoolean> aRes = boost::dynamic_pointer_cast<
      ModelAPI_AttributeBoolean>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeString> Model_Data::string(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeString>();
  }
  boost::shared_ptr<ModelAPI_AttributeString> aRes =
      boost::dynamic_pointer_cast<ModelAPI_AttributeString>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;

}

boost::shared_ptr<ModelAPI_AttributeReference> Model_Data::reference(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeReference>();
  }
  boost::shared_ptr<ModelAPI_AttributeReference> aRes = boost::dynamic_pointer_cast<
      ModelAPI_AttributeReference>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeSelection> Model_Data::selection(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeSelection>();
  }
  boost::shared_ptr<ModelAPI_AttributeSelection> aRes = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeSelection>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeSelectionList> 
  Model_Data::selectionList(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeSelectionList>();
  }
  boost::shared_ptr<ModelAPI_AttributeSelectionList> aRes = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeRefAttr> Model_Data::refattr(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeRefAttr>();
  }
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRes = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeRefList> Model_Data::reflist(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = 
    myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeRefList>();
  }
  boost::shared_ptr<ModelAPI_AttributeRefList> aRes = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_Attribute> Model_Data::attribute(const std::string& theID)
{
  boost::shared_ptr<ModelAPI_Attribute> aResult;
  if (myAttrs.find(theID) == myAttrs.end())  // no such attribute
    return aResult;
  return myAttrs[theID];
}

const std::string& Model_Data::id(const boost::shared_ptr<ModelAPI_Attribute>& theAttr)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = 
    myAttrs.begin();
  for (; anAttr != myAttrs.end(); anAttr++) {
    if (anAttr->second == theAttr)
      return anAttr->first;
  }
  // not found
  static std::string anEmpty;
  return anEmpty;
}

bool Model_Data::isEqual(const boost::shared_ptr<ModelAPI_Data>& theData)
{
  boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(theData);
  if (aData)
    return myLab.IsEqual(aData->myLab) == Standard_True ;
  return false;
}

bool Model_Data::isValid()
{
  return !myLab.IsNull() && myLab.HasAttribute();
}

std::list<boost::shared_ptr<ModelAPI_Attribute> > Model_Data::attributes(const std::string& theType)
{
  std::list<boost::shared_ptr<ModelAPI_Attribute> > aResult;
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
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
    if (myObject) {
      myObject->attributeChanged();
    }
  }
}

void Model_Data::erase()
{
  if (!myLab.IsNull())
    myLab.ForgetAllAttributes();
}

/// identifeir of the "must be updated" flag in the data tree
Standard_GUID kMustBeUpdatedGUID("baede74c-31a6-4416-9c4d-e48ce65f2005");

void Model_Data::mustBeUpdated(const bool theFlag)
{
  if (theFlag)
    TDataStd_UAttribute::Set(myLab, kMustBeUpdatedGUID);
  else
    myLab.ForgetAttribute(kMustBeUpdatedGUID);
}

bool Model_Data::mustBeUpdated()
{
  return myLab.IsAttribute(kMustBeUpdatedGUID) == Standard_True;
}

int Model_Data::featureId() const
{
  return myLab.Father().Tag(); // tag of the feature label
}

void Model_Data::eraseBackReferences()
{
  myRefsToMe.clear();
  boost::shared_ptr<ModelAPI_Result> aRes = 
    boost::dynamic_pointer_cast<ModelAPI_Result>(myObject);
  if (aRes)
    aRes->setIsConcealed(false);
}

void Model_Data::addBackReference(FeaturePtr theFeature, std::string theAttrID)
{
  myRefsToMe.insert(theFeature->data()->attribute(theAttrID));
  if (ModelAPI_Session::get()->validators()->isConcealed(theFeature->getKind(), theAttrID)) {
    boost::shared_ptr<ModelAPI_Result> aRes = 
      boost::dynamic_pointer_cast<ModelAPI_Result>(myObject);
    if (aRes) {
      aRes->setIsConcealed(true);
    }
  }
}

void Model_Data::referencesToObjects(
  std::list<std::pair<std::string, std::list<ObjectPtr> > >& theRefs)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = myAttrs.begin();
  std::list<ObjectPtr> aReferenced; // not inside of cycle to avoid excess memory menagement
  for(; anAttr != myAttrs.end(); anAttr++) {
    std::string aType = anAttr->second->attributeType();
    if (aType == ModelAPI_AttributeReference::type()) { // reference to object
      boost::shared_ptr<ModelAPI_AttributeReference> aRef = boost::dynamic_pointer_cast<
          ModelAPI_AttributeReference>(anAttr->second);
      aReferenced.push_back(aRef->value());
      theRefs.push_back(std::pair<std::string, std::list<ObjectPtr> >(anAttr->first, aReferenced));
    } else if (aType == ModelAPI_AttributeRefAttr::type()) { // reference to attribute or object
      boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = boost::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(anAttr->second);
      aReferenced.push_back(aRef->isObject() ? aRef->object() : aRef->attr()->owner());
    } else if (aType == ModelAPI_AttributeRefList::type()) { // list of references
      aReferenced = boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttr->second)->list();
    } else if (aType == ModelAPI_AttributeSelection::type()) { // selection attribute
      boost::shared_ptr<ModelAPI_AttributeSelection> aRef = boost::dynamic_pointer_cast<
          ModelAPI_AttributeSelection>(anAttr->second);
      aReferenced.push_back(aRef->context());
    } else if (aType == ModelAPI_AttributeSelectionList::type()) { // list of selection attributes
      boost::shared_ptr<ModelAPI_AttributeSelectionList> aRef = boost::dynamic_pointer_cast<
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
