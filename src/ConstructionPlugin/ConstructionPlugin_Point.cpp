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

using namespace std;

ConstructionPlugin_Point::ConstructionPlugin_Point()
{
}

void ConstructionPlugin_Point::initAttributes()
{
  data()->addAttribute(POINT_ATTR_X, ModelAPI_AttributeDouble::type());
  data()->addAttribute(POINT_ATTR_Y, ModelAPI_AttributeDouble::type());
  data()->addAttribute(POINT_ATTR_Z, ModelAPI_AttributeDouble::type());
}

void ConstructionPlugin_Point::execute()
{
  boost::shared_ptr<GeomAPI_Pnt> aPnt(
      new GeomAPI_Pnt(data()->real(POINT_ATTR_X)->value(), data()->real(POINT_ATTR_Y)->value(),
                      data()->real(POINT_ATTR_Z)->value()));

  boost::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
  aConstr->setShape(GeomAlgoAPI_PointBuilder::point(aPnt));
  setResult(aConstr);
}
