// File:        ModuleBase_WidgetPoint2dDistance.h
// Created:     23 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetPoint2dDistance.h>
#include <ModuleBase_WidgetValueFeature.h>
#include <ModuleBase_DoubleSpinBox.h>

#include <GeomAPI_Pnt2d.h>
#include <Config_WidgetAPI.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>

ModuleBase_WidgetPoint2dDistance::ModuleBase_WidgetPoint2dDistance(QWidget* theParent,
                                                                   const Config_WidgetAPI* theData,
                                                                   const std::string& theParentId)
    : ModuleBase_WidgetDoubleValue(theParent, theData, theParentId)
{
  myFirstPntName = theData->getProperty("first_point");
}

ModuleBase_WidgetPoint2dDistance::~ModuleBase_WidgetPoint2dDistance()
{
}

bool ModuleBase_WidgetPoint2dDistance::setValue(ModuleBase_WidgetValue* theValue)
{
  bool isDone = false;

  if (theValue) {
    ModuleBase_WidgetValueFeature* aFeatureValue =
        dynamic_cast<ModuleBase_WidgetValueFeature*>(theValue);
    if (aFeatureValue) {
      std::shared_ptr<GeomAPI_Pnt2d> aPnt = aFeatureValue->point();
      ObjectPtr aObject = aFeatureValue->object();
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      if (aFeature && aPnt) {
        setPoint(aFeature, aPnt);
        isDone = true;
      }
    }
  }
  return isDone;
}

void ModuleBase_WidgetPoint2dDistance::setPoint(FeaturePtr theFeature,
                                                const std::shared_ptr<GeomAPI_Pnt2d>& thePnt)
{
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(myFirstPntName));
  if (!aPoint)
    return;

  double aRadius = thePnt->distance(aPoint->pnt());
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (aReal && (aReal->value() != aRadius)) {
    aReal->setValue(aRadius);
    mySpinBox->setValue(aRadius);
  }
}
