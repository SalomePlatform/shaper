// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "Config_AttributeMessage.h"

Config_AttributeMessage::Config_AttributeMessage(const Events_ID theId, const void* theParent)
    : Events_Message(theId, theParent)
{
  myAttributeId = std::string(); // Attribute unique id
  myFeatureId = std::string(); // Feature unique id
  myIsObligatory = true;
  myIsConcealment = false;
}

Config_AttributeMessage::~Config_AttributeMessage()
{

}

const std::string& Config_AttributeMessage::featureId() const
{
  return myFeatureId;
}

void Config_AttributeMessage::setFeatureId(const std::string& theId)
{
  myFeatureId = theId;
}

const std::string& Config_AttributeMessage::attributeId() const
{
  return myAttributeId;
}

void Config_AttributeMessage::setAttributeId(const std::string& theId)
{
  myAttributeId = theId;
}

bool Config_AttributeMessage::isConcealment() const
{
  return myIsConcealment;
}

void Config_AttributeMessage::setConcealment(bool theConcealment)
{
  this->myIsConcealment = theConcealment;
}

bool Config_AttributeMessage::isObligatory() const
{
  return myIsObligatory;
}

void Config_AttributeMessage::setObligatory(bool theObligatory)
{
  this->myIsObligatory = theObligatory;
}

const std::list<std::pair<std::string, std::string> >& Config_AttributeMessage::getCases() const
{
  return myCases;
}

void Config_AttributeMessage::setCases(const std::list<std::pair<std::string,
                                       std::string> >& theCases)
{
  myCases = theCases;
}
