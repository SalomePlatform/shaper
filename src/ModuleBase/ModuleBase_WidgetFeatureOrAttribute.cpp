// File:        ModuleBase_WidgetFeatureOrAttribute.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetFeatureOrAttribute.h>

#include <ModuleBase_WidgetValueFeature.h>
#include <ModuleBase_WidgetValue.h>

#include <ModelAPI_RefAttrValidator.h>
#include <ModelAPI_Session.h>

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
      ObjectPtr aObject = aFeatureValue->object();

      std::shared_ptr<ModelAPI_Attribute> anAttribute = findAttribute(aFeatureValue);
      if (anAttribute) {
        isDone = setAttribute(anAttribute, false);
      }
      else if (aObject) {
        isDone = setObject(aObject, false);
      }

      if (isDone)
        emit valuesChanged();
    }
  }
  return isDone;
}

bool ModuleBase_WidgetFeatureOrAttribute::storeValue() const
{
  //FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theFeature);
  //if (!aFeature)
  //  return false;

  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
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
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
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

std::shared_ptr<ModelAPI_Attribute> ModuleBase_WidgetFeatureOrAttribute::findAttribute(
                                                        ModuleBase_WidgetValue* theValue)
{
  std::shared_ptr<ModelAPI_Attribute> anAttribute;
  ModuleBase_WidgetValueFeature* aFeatureValue =
                                  dynamic_cast<ModuleBase_WidgetValueFeature*>(theValue);
  if (!aFeatureValue)
    return anAttribute;

  std::shared_ptr<GeomAPI_Pnt2d> aValuePoint = aFeatureValue->point();
  if (aValuePoint) {
    ObjectPtr aObject = aFeatureValue->object();
    FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
    if (aFeature) {
      // find the given point in the feature attributes
      std::list<std::shared_ptr<ModelAPI_Attribute> > anAttiributes = aFeature->data()
          ->attributes(GeomDataAPI_Point2D::type());
      std::list<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anIt = anAttiributes
          .begin(), aLast = anAttiributes.end();
      for (; anIt != aLast && !anAttribute; anIt++) {
        std::shared_ptr<GeomDataAPI_Point2D> aCurPoint = std::dynamic_pointer_cast<
            GeomDataAPI_Point2D>(*anIt);
        if (aCurPoint && aCurPoint->pnt()->distance(aValuePoint) < Precision::Confusion())
          anAttribute = aCurPoint;
      }
    }
  }
  return anAttribute;
}

bool ModuleBase_WidgetFeatureOrAttribute::setAttribute(
    const std::shared_ptr<ModelAPI_Attribute>& theAttribute, bool theSendEvent)
{
  if (!theAttribute)  // || !featureKinds().contains(theAttribute->attributeType().c_str()))
    return false;

  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::list<ModelAPI_Validator*> aValidators;
  std::list<std::list<std::string> > anArguments;
  aFactory->validators(parentID(), attributeID(), aValidators, anArguments);

  // Check the acceptability of the attribute
  std::list<ModelAPI_Validator*>::iterator aValidator = aValidators.begin();
  int aSize = aValidators.size();
  std::list<std::list<std::string> >::iterator aArgs = anArguments.begin();
  for (; aValidator != aValidators.end(); aValidator++, aArgs++) {
    const ModelAPI_RefAttrValidator* aAttrValidator =
        dynamic_cast<const ModelAPI_RefAttrValidator*>(*aValidator);
    if (aAttrValidator) {
      if (!aAttrValidator->isValid(myFeature, *aArgs, theAttribute)) {
        return false;
      }
    }
  }

  myAttribute = theAttribute;
  editor()->setText(theAttribute ? theAttribute->attributeType().c_str() : "");
  if (theSendEvent)
    emit valuesChanged();
  return true;
}

