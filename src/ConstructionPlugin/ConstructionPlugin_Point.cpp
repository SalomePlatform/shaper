// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Point.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "ConstructionPlugin_Point.h"
#include "ModelAPI_Session.h"
#include "ModelAPI_Document.h"
#include "ModelAPI_Data.h"
#include "ModelAPI_AttributeDouble.h"
#include <ModelAPI_ResultConstruction.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAPI_Pnt.h>

#include <Config_PropManager.h>

using namespace std;

ConstructionPlugin_Point::ConstructionPlugin_Point()
{
}

const std::string& ConstructionPlugin_Point::getKind()
{
  static std::string MY_KIND = ConstructionPlugin_Point::ID();
  return MY_KIND;
}

void ConstructionPlugin_Point::initAttributes()
{
  data()->addAttribute(ConstructionPlugin_Point::X(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Point::Y(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Point::Z(), ModelAPI_AttributeDouble::typeId());
}

void ConstructionPlugin_Point::execute()
{
  std::shared_ptr<GeomAPI_Pnt> aPnt(
      new GeomAPI_Pnt(data()->real(ConstructionPlugin_Point::X())->value(),
                      data()->real(ConstructionPlugin_Point::Y())->value(),
                      data()->real(ConstructionPlugin_Point::Z())->value()));

  std::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
  aConstr->setShape(GeomAlgoAPI_PointBuilder::point(aPnt));
  setResult(aConstr);
}

bool ConstructionPlugin_Point::customisePresentation(ResultPtr theResult, 
                                                     AISObjectPtr thePrs,
                               std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
{
  bool isCustomized = theDefaultPrs.get() != NULL &&
                      theDefaultPrs->customisePresentation(theResult, thePrs, theDefaultPrs);
  //thePrs->setPointMarker(1, 1.); // Set point as a '+' symbol
  return true;
}
