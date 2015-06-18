// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetShapeSelector.h
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

#include <GeomValidators_ShapeType.h>

#include <Config_WidgetAPI.h>
#include <Events_Loop.h>
#include <Events_Message.h>
#include <GeomAPI_Interface.h>
#include <GeomAPI_Shape.h>
#include <GeomValidators_Tools.h>

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
#include <ModelAPI_ShapeValidator.h>

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
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
    : ModuleBase_WidgetValidated(theParent, theData, theParentId),
      myWorkshop(theWorkshop)
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
  myTextLine->setReadOnly(true);
  myTextLine->setToolTip(aToolTip);
  myTextLine->installEventFilter(this);

  aLayout->addRow(myLabel, myTextLine);

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ', QString::SkipEmptyParts);

  myShapeValidator = new GeomValidators_ShapeType();
}

//********************************************************************
ModuleBase_WidgetShapeSelector::~ModuleBase_WidgetShapeSelector()
{
  delete myShapeValidator;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::storeValueCustom() const
{
  // the value is stored on the selection changed signal processing 
  return true;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::setObject(ObjectPtr theSelectedObject,
                                               GeomShapePtr theShape)
{
  DataPtr aData = myFeature->data();
  AttributeReferencePtr aRef = aData->reference(attributeID());
  if (aRef) {
    ObjectPtr aObject = aRef->value();
    if (!(aObject && aObject->isSame(theSelectedObject))) {
      aRef->setValue(theSelectedObject);
    }
  } else {
    AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
    if (aRefAttr) {
      ObjectPtr aObject = aRefAttr->object();
      if (!(aObject && aObject->isSame(theSelectedObject))) {
        aRefAttr->setObject(theSelectedObject);
      }
    } else {
      AttributeSelectionPtr aSelectAttr = aData->selection(attributeID());
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theSelectedObject);
      if (aSelectAttr.get() != NULL) {
        aSelectAttr->setValue(aResult, theShape);
      }
    }
  }
}

//********************************************************************
QList<ModuleBase_ViewerPrs> ModuleBase_WidgetShapeSelector::getAttributeSelection() const
{
  QList<ModuleBase_ViewerPrs> aSelected;
  if(myFeature) {
    DataPtr aData = myFeature->data();
    AttributePtr anAttribute = myFeature->attribute(attributeID());

    ObjectPtr anObject = GeomValidators_Tools::getObject(anAttribute);
    TopoDS_Shape aShape;
    std::shared_ptr<GeomAPI_Shape> aShapePtr = getShape();
    if (aShapePtr.get()) {
      aShape = aShapePtr->impl<TopoDS_Shape>();
    }
    ModuleBase_ViewerPrs aPrs(anObject, aShape, NULL);
    aSelected.append(aPrs);
  }
  return aSelected;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::getGeomSelection(const ModuleBase_ViewerPrs& thePrs,
                                                      ObjectPtr& theObject,
                                                      GeomShapePtr& theShape)
{
  theObject = myWorkshop->selection()->getResult(thePrs);
  theShape = myWorkshop->selection()->getShape(thePrs);
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::clearAttribute()
{
  DataPtr aData = myFeature->data();
  AttributeSelectionPtr aSelect = aData->selection(attributeID());
  if (aSelect) {
    aSelect->setValue(ResultPtr(), std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape()));
    return;
  }
  AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
  if (aRefAttr) {
    aRefAttr->setObject(ObjectPtr());
    return;
  }
  AttributeReferencePtr aRef = aData->reference(attributeID());
  if (aRef) {
    aRef->setObject(ObjectPtr());
  }
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::restoreValue()
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

//********************************************************************
void ModuleBase_WidgetShapeSelector::onSelectionChanged()
{
  // In order to make reselection possible, set empty object and shape should be done
  setObject(ObjectPtr(), std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape()));

  QList<ModuleBase_ViewerPrs> aSelected = myWorkshop->selection()->getSelected(ModuleBase_ISelection::AllControls);
  bool aHasObject = setSelection(aSelected);

  // the updateObject method should be called to flush the updated sigal. The workshop listens it,
  // calls validators for the feature and, as a result, updates the Apply button state.
  updateObject(myFeature);
  // the widget loses the focus only if the selected object is set
  if (aHasObject)
    emit focusOutWidget(this);
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::acceptSubShape(const TopoDS_Shape& theShape) const
{
  foreach (QString aType, myShapeTypes) {
    if (theShape.ShapeType() == ModuleBase_Tools::shapeType(aType))
      return true;
  }
  return false;
}

//********************************************************************
GeomShapePtr ModuleBase_WidgetShapeSelector::getShape() const
{
  GeomShapePtr aShape;
  DataPtr aData = myFeature->data();
  if (!aData->isValid())
    return aShape;

  AttributeSelectionPtr aSelect = aData->selection(attributeID());
  if (aSelect)
    aShape = aSelect->value();

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
    ObjectPtr anObject = GeomValidators_Tools::getObject(myFeature->attribute(attributeID()));
    if (anObject.get() != NULL) {
      std::string aName = anObject->data()->name();
      myTextLine->setText(QString::fromStdString(aName));
    } else {
      AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
      if (aRefAttr && aRefAttr->attr().get() != NULL) {
        //myIsObject = aRefAttr->isObject();
        AttributePtr anAttr = aRefAttr->attr();
        if (anAttr.get() != NULL) {
          std::stringstream aName;
          aName <<anAttr->owner()->data()->name()<<"/"<<anAttr->id();
          myTextLine->setText(QString::fromStdString(aName.str()));
        }
      }
      else {
        myTextLine->setText(getDefaultValue().c_str());
      }
    }
  }
}


//********************************************************************
void ModuleBase_WidgetShapeSelector::activateSelection(bool toActivate)
{
  updateSelectionName();

  if (toActivate) {
    QIntList aList;
    foreach (QString aType, myShapeTypes) {
      aList.append(ModuleBase_Tools::shapeType(aType));
    }
    myWorkshop->activateSubShapesSelection(aList);
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::raisePanel() const
{
  QWidget* aParent = this->parentWidget();
  QWidget* aLastPanel = 0;
  while (!aParent->inherits("QDockWidget")) {
    aLastPanel = aParent;
    aParent = aParent->parentWidget();
    if (!aParent)
      return;
  }
  if (aParent->inherits("QDockWidget")) {
    QDockWidget* aTabWgt = (QDockWidget*) aParent;
    aTabWgt->raise();
  }
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::activateCustom()
{
  connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  activateSelection(true);

  // Restore selection in the viewer by the attribute selection list
  myWorkshop->setSelected(getAttributeSelection());

  activateFilters(myWorkshop, true);
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::storeAttributeValue()
{
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());

  myObject = GeomValidators_Tools::getObject(anAttribute);
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
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());

  setObject(myObject, myShape);
  AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
  if (aRefAttr) {
    if (!myIsObject)
      aRefAttr->setAttr(myRefAttribute);
  }
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  GeomShapePtr aShape = myWorkshop->selection()->getShape(thePrs);
  bool aValid;
  // if there is no selected shape, the method returns true
  if (!aShape.get())
    aValid = true;
  else {
    // Check that the selection corresponds to selection type
    TopoDS_Shape aTopoShape = aShape->impl<TopoDS_Shape>();
    aValid = acceptSubShape(aTopoShape);
  }
  if (aValid) {
    // In order to avoid selection of the same object
    ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
    FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aResult);
    aValid = aSelectedFeature != myFeature;
  }
  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::setSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  ObjectPtr anObject;
  GeomShapePtr aShape;
  getGeomSelection(thePrs, anObject, aShape);

  setObject(anObject, aShape);
  return true;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::deactivate()
{
  activateSelection(false);
  activateFilters(myWorkshop, false);
  disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}
