// File:        Model_Object.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Object.h>
#include <Model_AttributeDocRef.h>
#include <TDataStd_Name.hxx>

using namespace std;

Model_Object::Model_Object()
{
}

void Model_Object::setLabel(TDF_Label& theLab)
{
  myLab = theLab;
}

string Model_Object::getName()
{
  Handle(TDataStd_Name) aName;
  if (myLab.FindAttribute(TDataStd_Name::GetID(), aName))
    return string(TCollection_AsciiString(aName->Get()).ToCString());
  return ""; // not defined
}

void Model_Object::setName(string theName)
{
  TDataStd_Name::Set(myLab, theName.c_str());
}

void Model_Object::addAttribute(string theID, string theAttrType)
{
  TDF_Label anAttrLab = myLab.FindChild(myAttrs.size() + 1);
  ModelAPI_Attribute* anAttr = 0;
  if (theAttrType == ModelAPI_AttributeDocRef::type())
    anAttr = new Model_AttributeDocRef(anAttrLab);
  if (anAttr)
    myAttrs[theID] = std::shared_ptr<ModelAPI_Attribute>(anAttr);
  else
    ; // TODO: generate error on unknown attribute request and/or add mechanism for customization
}

shared_ptr<ModelAPI_AttributeDocRef> Model_Object::docRef(const string theID)
{
  map<string, shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return std::shared_ptr<ModelAPI_AttributeDocRef>();
  }
  shared_ptr<ModelAPI_AttributeDocRef> aRes = 
    dynamic_pointer_cast<ModelAPI_AttributeDocRef>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}
