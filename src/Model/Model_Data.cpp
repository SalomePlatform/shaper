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
#include <Model_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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

boost::shared_ptr<ModelAPI_AttributeRefAttr> Model_Data::refattr(const std::string& theID)
{
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
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
  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = myAttrs.begin();
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

bool Model_Data::referencesTo(const boost::shared_ptr<ModelAPI_Feature>& theFeature)
{
  // collect results of this feature first to check references quickly in the cycle
  std::set<ObjectPtr> aFeatureObjs;
  aFeatureObjs.insert(theFeature);
  std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter =
    theFeature->results().cbegin();
  for(; aRIter != theFeature->results().cend(); aRIter++) {
    if (*aRIter)
      aFeatureObjs.insert(*aRIter);
  }

  std::map<std::string, boost::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
    myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    if (anAttrsIter->second->attributeType() == ModelAPI_AttributeRefAttr::type()) {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttr = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttrsIter->second);
      if (aRefAttr && aRefAttr->isObject()) { // check referenced object
        if (aFeatureObjs.find(aRefAttr->object()) != aFeatureObjs.end())
          return true;
      } else { // check object of referenced attribute
        boost::shared_ptr<ModelAPI_Attribute> anAttr = aRefAttr->attr();
        if (anAttr && aFeatureObjs.find(anAttr->owner()) != aFeatureObjs.end())
          return true;
      }
    } else if (anAttrsIter->second->attributeType() == ModelAPI_AttributeReference::type()) {
      boost::shared_ptr<ModelAPI_AttributeReference> aRef = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(anAttrsIter->second);
      if (aFeatureObjs.find(aRef->value()) != aFeatureObjs.end()) {
        return true;
      }
    }
  }
  return false;
}
