// File:        ModuleBase_WidgetFeatureOrAttribute.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetFeatureOrAttribute.h>

#include <ModuleBase_WidgetValueFeature.h>
#include <ModuleBase_WidgetValue.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Pnt2d.h>

#include <GeomDataAPI_Point2D.h>

#include <Precision.hxx>

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

ModuleBase_WidgetFeatureOrAttribute::ModuleBase_WidgetFeatureOrAttribute(
    QWidget* theParent, const Config_WidgetAPI* theData, const std::string& theParentId)
    : ModuleBase_WidgetFeature(theParent, theData, theParentId)
{
}

ModuleBase_WidgetFeatureOrAttribute::~ModuleBase_WidgetFeatureOrAttribute()
{
}

bool ModuleBase_WidgetFeatureOrAttribute::setValue(ModuleBase_WidgetValue* theValue)
{
  bool isDone = false;

  if (theValue) {
    ModuleBase_WidgetValueFeature* aFeatureValue =
        dynamic_cast<ModuleBase_WidgetValueFeature*>(theValue);
    if (aFeatureValue) {
      boost::shared_ptr<GeomAPI_Pnt2d> aValuePoint = aFeatureValue->point();
      ObjectPtr aObject = aFeatureValue->object();
      if (aObject) {
        isDone = setObject(aObject, false);
      }
      if (aValuePoint) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
        if (aFeature) {
          // find the given point in the feature attributes
          std::list<boost::shared_ptr<ModelAPI_Attribute> > anAttiributes = aFeature->data()
              ->attributes(GeomDataAPI_Point2D::type());
          std::list<boost::shared_ptr<ModelAPI_Attribute> >::const_iterator anIt = anAttiributes
              .begin(), aLast = anAttiributes.end();
          boost::shared_ptr<GeomDataAPI_Point2D> aFPoint;
          for (; anIt != aLast && !aFPoint; anIt++) {
            boost::shared_ptr<GeomDataAPI_Point2D> aCurPoint = boost::dynamic_pointer_cast<
                GeomDataAPI_Point2D>(*anIt);
            if (aCurPoint && aCurPoint->pnt()->distance(aValuePoint) < Precision::Confusion())
              aFPoint = aCurPoint;
          }
          if (aFPoint)
            isDone = setAttribute(aFPoint, false);
        }
      }
      if (isDone)
        emit valuesChanged();
    }
  }
  return isDone;
}

bool ModuleBase_WidgetFeatureOrAttribute::storeValue() const
{
  //FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theFeature);
  //if (!aFeature)
  //  return false;

  boost::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(attributeID()));

  if (myObject)
    aRef->setObject(myObject);
  if (myAttribute)
    aRef->setAttr(myAttribute);

  myFeature->execute();
  updateObject(myFeature);

  return true;
}

bool ModuleBase_WidgetFeatureOrAttribute::restoreValue()
{
  boost::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(attributeID()));

  ObjectPtr aObj = aRef->object();
  FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
  if (aFeature) {
    myObject = aFeature;
    myAttribute = aRef->attr();

    std::string aText = "";
    if (aFeature)
      aText = aFeature->data()->name().c_str();
    if (myAttribute)
      aText = myAttribute->attributeType().c_str();

    editor()->setText(aText.c_str());
    return true;
  }
  return false;
}

bool ModuleBase_WidgetFeatureOrAttribute::setAttribute(
    const boost::shared_ptr<ModelAPI_Attribute>& theAttribute, bool theSendEvent)
{
  if (!theAttribute)  // || !featureKinds().contains(theAttribute->attributeType().c_str()))
    return false;

  myAttribute = theAttribute;
  editor()->setText(theAttribute ? theAttribute->attributeType().c_str() : "");
  if (theSendEvent)
    emit valuesChanged();
  return true;
}

