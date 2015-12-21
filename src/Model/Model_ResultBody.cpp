// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultBody.cpp
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultBody.h>
#include <Model_BodyBuilder.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_Tools.h>
#include <Config_PropManager.h>
// DEB
//#include <TCollection_AsciiString.hxx>
//#include <TDF_Tool.hxx>
//#define DEB_IMPORT 1

Model_ResultBody::Model_ResultBody()
{
  myBuilder = new Model_BodyBuilder(this);
}

void Model_ResultBody::initAttributes()
{
  // append the color attribute. It is empty, the attribute will be filled by a request
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::typeId());
}

void Model_ResultBody::colorConfigInfo(std::string& theSection, std::string& theName,
  std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_body_color";
  theDefault = DEFAULT_COLOR();
}

bool Model_ResultBody::setDisabled(std::shared_ptr<ModelAPI_Result> theThis, const bool theFlag)
{
  bool aChanged = ModelAPI_ResultBody::setDisabled(theThis, theFlag);
  if (aChanged) { // state is changed, so modifications are needed
    myBuilder->evolutionToSelection(theFlag);
  }
  return aChanged;
}

bool Model_ResultBody::isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  return myBuilder->isLatestEqual(theShape);
}

bool Model_ResultBody::isConcealed()
{
  if (ModelAPI_ResultBody::isConcealed())
    return true;
  ResultPtr aThis = std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner());
  if (aThis.get()) {
    ResultCompSolidPtr aParent = ModelAPI_Tools::compSolidOwner(aThis);
    if (aParent.get()) {
      if (aParent->isConcealed())
        return true;
    }
  }
  return false;
}
