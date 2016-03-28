// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetShapeSelector.cpp
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_Filter.h>
#include <ModuleBase_IModule.h>

#include <Config_WidgetAPI.h>
#include <Events_Loop.h>
#include <Events_Message.h>
#include <GeomAPI_Interface.h>
#include <GeomAPI_Shape.h>

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeValidator.h>

#include <Config_WidgetAPI.h>
#include <Events_Error.h>

#include <GeomAPI_Shape.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QString>
#include <QEvent>
#include <QDockWidget>
#include <QApplication>
#include <QFormLayout>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <memory>

#include <list>
#include <string>

ModuleBase_WidgetShapeSelector::ModuleBase_WidgetShapeSelector(QWidget* theParent,
                                                     ModuleBase_IWorkshop* theWorkshop,
                                                     const Config_WidgetAPI* theData)
: ModuleBase_WidgetSelector(theParent, theWorkshop, theData)
{
  QFormLayout* aLayout = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aLayout);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, this);
  if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(QPixmap(aLabelIcon));


  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  myTextLine = new QLineEdit(this);
  QString anObjName = QString::fromStdString(attributeID());
  myTextLine->setObjectName(anObjName);
  myTextLine->setReadOnly(true);
  myTextLine->setToolTip(aToolTip);
  myTextLine->installEventFilter(this);

  aLayout->addRow(myLabel, myTextLine);
  myLabel->setToolTip(aToolTip);

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ', QString::SkipEmptyParts);
}

//********************************************************************
ModuleBase_WidgetShapeSelector::~ModuleBase_WidgetShapeSelector()
{
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::storeValueCustom() const
{
  // the value is stored on the selection changed signal processing 
  return true;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                                                  const bool theToValidate)
{
  if (theValues.empty()) {
    // In order to make reselection possible, set empty object and shape should be done
    setSelectionCustom(ModuleBase_ViewerPrs());
    return false;
  }
  // it removes the processed value from the parameters list
  ModuleBase_ViewerPrs aValue = theValues.takeFirst();
  bool isDone = false;

  if (!theToValidate || isValidInFilters(aValue)) {
    isDone = setSelectionCustom(aValue);
    // updateObject - to update/redisplay feature
    // it is commented in order to perfom it outside the method
    //updateObject(myFeature);
    // to storeValue()
    //emit valuesChanged();
  }
  return isDone;
}

//********************************************************************
QList<ModuleBase_ViewerPrs> ModuleBase_WidgetShapeSelector::getAttributeSelection() const
{
  QList<ModuleBase_ViewerPrs> aSelected;
  if(myFeature) {
    DataPtr aData = myFeature->data();
    AttributePtr anAttribute = myFeature->attribute(attributeID());

    ObjectPtr anObject = ModuleBase_Tools::getObject(anAttribute);
    std::shared_ptr<GeomAPI_Shape> aShapePtr = getShape();
    ModuleBase_ViewerPrs aPrs(anObject, aShapePtr, NULL);
    aSelected.append(aPrs);
  }
  return aSelected;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::restoreValueCustom()
{
  bool isBlocked = this->blockSignals(true);
  updateSelectionName();
  this->blockSignals(isBlocked);

  return true;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetShapeSelector::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myTextLine);
  return aControls;
}

void ModuleBase_WidgetShapeSelector::updateFocus()
{
  emit focusOutWidget(this);
}

//********************************************************************
QIntList ModuleBase_WidgetShapeSelector::getShapeTypes() const
{
  QIntList aShapeTypes;
  foreach(QString aType, myShapeTypes) {
    aShapeTypes.append(ModuleBase_Tools::shapeType(aType));
  }
  return aShapeTypes;
}

//********************************************************************
GeomShapePtr ModuleBase_WidgetShapeSelector::getShape() const
{
  GeomShapePtr aShape;
  DataPtr aData = myFeature->data();
  if (!aData->isValid())
    return aShape;

  std::string aType = aData->attribute(attributeID())->attributeType();
  if (aType == ModelAPI_AttributeReference::typeId()) {
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
    if (aRefAttr.get() && !aRefAttr->isObject()) {
      AttributePtr anAttribute = aRefAttr->attr();
      aShape = myWorkshop->module()->findShape(anAttribute);
    }
  } else if (aType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectAttr = aData->selection(attributeID());
    aShape = aSelectAttr->value();
  }

  return aShape;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::updateSelectionName()
{
  DataPtr aData = myFeature->data();
  if (!aData->isValid())
    return;

  bool isNameUpdated = false;
  AttributeSelectionPtr aSelect = aData->selection(attributeID());
  if (aSelect) {
    myTextLine->setText(QString::fromStdString(aSelect->namingName(getDefaultValue())));
    isNameUpdated = true;
  }
  if (!isNameUpdated) {
    ObjectPtr anObject = ModuleBase_Tools::getObject(myFeature->attribute(attributeID()));
    if (anObject.get() != NULL) {
      std::string aName = anObject->data()->name();
      myTextLine->setText(QString::fromStdString(aName));
    } else {
      AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
      if (aRefAttr && aRefAttr->attr().get() != NULL) {
        //myIsObject = aRefAttr->isObject();
        std::string anAttrName = generateName(aRefAttr->attr(), myWorkshop);
        myTextLine->setText(QString::fromStdString(anAttrName));
      }
      else {
        myTextLine->setText(getDefaultValue().c_str());
      }
    }
  }
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::storeAttributeValue()
{
  ModuleBase_WidgetValidated::storeAttributeValue();

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());

  myObject = ModuleBase_Tools::getObject(anAttribute);
  myShape = getShape();
  myRefAttribute = AttributePtr();
  myIsObject = false;
  AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
  if (aRefAttr) {
    myIsObject = aRefAttr->isObject();
    myRefAttribute = aRefAttr->attr();
  }
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::restoreAttributeValue(bool theValid)
{
  ModuleBase_WidgetValidated::restoreAttributeValue(theValid);

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());

  setObject(myObject, myShape);
  AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
  if (aRefAttr) {
    if (!myIsObject)
      aRefAttr->setAttr(myRefAttribute);
  }
}
