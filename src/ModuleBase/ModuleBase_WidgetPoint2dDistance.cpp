// File:        ModuleBase_WidgetPoint2dDistance.h
// Created:     23 June 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_WidgetPoint2dDistance.h"

#include <GeomAPI_Pnt2d.h>
#include <Config_WidgetAPI.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>

#include <QDoubleSpinBox>

ModuleBase_WidgetPoint2dDistance::ModuleBase_WidgetPoint2dDistance(QWidget* theParent, const Config_WidgetAPI* theData)
  : ModuleBase_WidgetDoubleValue(theParent, theData)
{
  myFirstPntName = theData->getProperty("first_point");
}

ModuleBase_WidgetPoint2dDistance::~ModuleBase_WidgetPoint2dDistance()
{
}

void ModuleBase_WidgetPoint2dDistance::setPoint(FeaturePtr theFeature, const boost::shared_ptr<GeomAPI_Pnt2d>& thePnt)
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                              (aData->attribute(myFirstPntName));
  double aRadius = thePnt->distance(aPoint->pnt());
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (aReal && (aReal->value() != aRadius)) {
    aReal->setValue(aRadius);
    mySpinBox->setValue(aRadius);
  }
}