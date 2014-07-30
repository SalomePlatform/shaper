// File:        ModuleBase_WidgetFeatureOrAttribute.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetFeatureOrAttribute.h>

#include <ModuleBase_WidgetValueFeature.h>
#include <ModuleBase_WidgetValue.h>
#include <ModuleBase_Tools.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomDataAPI_Point2D.h>

#include <Precision.hxx>

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

ModuleBase_WidgetFeatureOrAttribute::ModuleBase_WidgetFeatureOrAttribute(QWidget* theParent,
                                                   const Config_WidgetAPI* theData, 
                                                   const std::string& theParentId)
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
        isDone = setObject(aObject);
      }
      if (!isDone && aValuePoint) {
        FeaturePtr aFeature = ModuleBase_Tools::feature(aObject);
        if (aFeature) {
          // find the given point in the feature attributes
          std::list<boost::shared_ptr<ModelAPI_Attribute> > anAttiributes =
                                        aFeature->data()->attributes(GeomDataAPI_Point2D::type());
          std::list<boost::shared_ptr<ModelAPI_Attribute> >::const_iterator anIt = anAttiributes.begin(),
                                                                            aLast = anAttiributes.end();
          boost::shared_ptr<GeomDataAPI_Point2D> aFPoint;
          for (;anIt!=aLast && !aFPoint; anIt++) {
            boost::shared_ptr<GeomDataAPI_Point2D> aCurPoint =
                                                boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);
            if (aCurPoint && aCurPoint->pnt()->distance(aValuePoint) < Precision::Confusion())
              aFPoint = aCurPoint;
          }
          if (aFPoint)
            isDone = setAttribute(aFPoint);
        }
      }
    }
  }
  return isDone;
}

bool ModuleBase_WidgetFeatureOrAttribute::storeValue(ObjectPtr theFeature) const
{
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theFeature);
  if (!aFeature)
    return false;
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(attributeID()));

  ModuleBase_WidgetFeatureOrAttribute* that = (ModuleBase_WidgetFeatureOrAttribute*) this;
  if (object())
    aRef->setObject(object());
  else if (myAttribute)
    aRef->setAttr(myAttribute);

  aFeature->execute();
  updateObject(theFeature);

  return true;
}

bool ModuleBase_WidgetFeatureOrAttribute::restoreValue(ObjectPtr theFeature)
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(attributeID()));

  FeaturePtr aFeature = ModuleBase_Tools::feature(aRef->object());
  if (aFeature) {
    setObject(aFeature);
    myAttribute = aRef->attr();

    std::string aText = "";
    if (aFeature)
      aText = aFeature->data()->name().c_str();
    else if (myAttribute)
      aText = myAttribute->attributeType().c_str();

    editor()->setText(aText.c_str());
    return true;
  }
  return false;
}

bool ModuleBase_WidgetFeatureOrAttribute::setAttribute(const boost::shared_ptr<ModelAPI_Attribute>& theAttribute)
{
  if (!theAttribute)// || !featureKinds().contains(theAttribute->attributeType().c_str()))
    return false;

  myAttribute = theAttribute;
  editor()->setText(theAttribute ? theAttribute->attributeType().c_str() : "");
  emit valuesChanged();
  return true;
}

