// File:        Model_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Data.h>
#include <Model_AttributeDocRef.h>
#include <Model_AttributeDouble.h>
#include <GeomData_Point.h>
#include <GeomData_Point2D.h>
#include <GeomData_Dir.h>
#include <TDataStd_Name.hxx>

using namespace std;

Model_Data::Model_Data()
{
}

void Model_Data::setLabel(TDF_Label& theLab)
{
  myLab = theLab;
}

string Model_Data::getName()
{
  Handle(TDataStd_Name) aName;
  if (myLab.FindAttribute(TDataStd_Name::GetID(), aName))
    return string(TCollection_AsciiString(aName->Get()).ToCString());
  return ""; // not defined
}

void Model_Data::setName(string theName)
{
  TDataStd_Name::Set(myLab, theName.c_str());
}

void Model_Data::addAttribute(string theID, string theAttrType)
{
  TDF_Label anAttrLab = myLab.FindChild(myAttrs.size() + 1);
  ModelAPI_Attribute* anAttr = 0;
  if (theAttrType == ModelAPI_AttributeDocRef::type())
    anAttr = new Model_AttributeDocRef(anAttrLab);
  else if (theAttrType == ModelAPI_AttributeDouble::type())
    anAttr = new Model_AttributeDouble(anAttrLab);
  else if (theAttrType == GeomData_Point::type())
    anAttr = new GeomData_Point(anAttrLab);
  else if (theAttrType == GeomData_Dir::type())
    anAttr = new GeomData_Dir(anAttrLab);
  else if (theAttrType == GeomData_Point2D::type())
    anAttr = new GeomData_Point2D(anAttrLab);

  if (anAttr) {
    myAttrs[theID] = boost::shared_ptr<ModelAPI_Attribute>(anAttr);
    anAttr->setFeature(myFeature);
  }
  else
    ; // TODO: generate error on unknown attribute request and/or add mechanism for customization
}

boost::shared_ptr<ModelAPI_AttributeDocRef> Model_Data::docRef(const string theID)
{
  map<string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeDocRef>();
  }
  boost::shared_ptr<ModelAPI_AttributeDocRef> aRes = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDocRef>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_AttributeDouble> Model_Data::real(const string theID)
{
  map<string, boost::shared_ptr<ModelAPI_Attribute> >::iterator aFound = myAttrs.find(theID);
  if (aFound == myAttrs.end()) {
    // TODO: generate error on unknown attribute request and/or add mechanism for customization
    return boost::shared_ptr<ModelAPI_AttributeDouble>();
  }
  boost::shared_ptr<ModelAPI_AttributeDouble> aRes = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aFound->second);
  if (!aRes) {
    // TODO: generate error on invalid attribute type request
  }
  return aRes;
}

boost::shared_ptr<ModelAPI_Attribute> Model_Data::attribute(const std::string theID)
{
  boost::shared_ptr<ModelAPI_Attribute> aResult;
  if (myAttrs.find(theID) == myAttrs.end()) // no such attribute
    return aResult;
  return myAttrs[theID];
}
