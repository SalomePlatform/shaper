// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : ModelHighAPI_RefAttr.cpp
// Purpose:
//
// History:
// 08/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_RefAttr.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include "ModelHighAPI_Interface.h"
//--------------------------------------------------------------------------------------
ModelHighAPI_RefAttr::ModelHighAPI_RefAttr()
: myVariantType(VT_ATTRIBUTE)
{
}

ModelHighAPI_RefAttr::ModelHighAPI_RefAttr(
    const std::shared_ptr<ModelAPI_Attribute> & theValue)
: myVariantType(VT_ATTRIBUTE)
, myAttribute(theValue)
{
}

ModelHighAPI_RefAttr::ModelHighAPI_RefAttr(
    const std::shared_ptr<ModelAPI_Object> & theValue)
: myVariantType(VT_OBJECT)
, myObject(theValue)
{
}

ModelHighAPI_RefAttr::ModelHighAPI_RefAttr(
    const std::shared_ptr<ModelHighAPI_Interface> & theValue)
: myVariantType(VT_OBJECT)
, myObject(std::shared_ptr<ModelAPI_Object>(theValue->defaultResult()))
{
}

ModelHighAPI_RefAttr::~ModelHighAPI_RefAttr()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_RefAttr::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute) const
{
  switch(myVariantType) {
    case VT_ATTRIBUTE: theAttribute->setAttr(myAttribute); return;
    case VT_OBJECT: theAttribute->setObject(myObject); return;
  }
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_RefAttr::appendToList(
    const std::shared_ptr<ModelAPI_AttributeRefAttrList> & theAttribute) const
{
  switch(myVariantType) {
    case VT_ATTRIBUTE: theAttribute->append(myAttribute); return;
    case VT_OBJECT: theAttribute->append(myObject); return;
  }
}

//--------------------------------------------------------------------------------------
bool ModelHighAPI_RefAttr::isEmpty() const
{
  return !(myAttribute && myObject);
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_RefAttr::fillMessage(
    const std::shared_ptr<ModelAPI_ObjectMovedMessage>& theMessage) const
{
  switch (myVariantType) {
  case VT_ATTRIBUTE: theMessage->setMovedAttribute(myAttribute); return;
  case VT_OBJECT: theMessage->setMovedObject(myObject); return;
  }
}
