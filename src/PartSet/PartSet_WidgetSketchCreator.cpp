// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchCreator.cpp
// Created:     08 June 2015
// Author:      Vitaly SMETANNIKOV

#include "PartSet_WidgetSketchCreator.h"
#include "PartSet_Module.h"
#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_PreviewPlanes.h"

#include <Config_Keywords.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_Tools.h>
#include <XGUI_ViewerProxy.h>

#include <GeomAPI_Face.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Events.h>

#include <SketchPlugin_SketchEntity.h>
#include <FeaturesPlugin_CompositeBoolean.h>

#include <ModuleBase_Tools.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_OperationFeature.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>

#include <QLabel>
#include <QLineEdit>
//#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>

PartSet_WidgetSketchCreator::PartSet_WidgetSketchCreator(QWidget* theParent, 
                                                         PartSet_Module* theModule,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetSelector(theParent, theModule->workshop(), theData),
  myModule(theModule)
{
  myAttributeListID = theData->getProperty("attribute_list_id");

  //QFormLayout* aLayout = new QFormLayout(this);
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);

  ModuleBase_Tools::adjustMargins(aLayout);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());

  myLabel = new QLabel(aLabelText, this);
  myLabel->setWordWrap(true);
  aLayout->addWidget(myLabel);
  /*if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(QPixmap(aLabelIcon));


  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  myTextLine = new QLineEdit(this);
  myTextLine->setReadOnly(true);
  myTextLine->setToolTip(aToolTip);
  myTextLine->installEventFilter(this);

  myLabel->setToolTip(aToolTip);

  aLayout->addRow(myLabel, myTextLine);*/

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ', QString::SkipEmptyParts);

  myPreviewPlanes = new PartSet_PreviewPlanes();
}

PartSet_WidgetSketchCreator::~PartSet_WidgetSketchCreator()
{
}

QList<QWidget*> PartSet_WidgetSketchCreator::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myLabel);
  return aControls;
}

bool PartSet_WidgetSketchCreator::restoreValueCustom()
{
  /*CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aCompFeature->numberOfSubs() > 0) {
    FeaturePtr aSubFeature = aCompFeature->subFeature(0);
    myTextLine->setText(QString::fromStdString(aSubFeature->data()->name()));
  }*/
  return true;
}

bool PartSet_WidgetSketchCreator::storeValueCustom() const
{
  return true;
}

void PartSet_WidgetSketchCreator::setVisibleSelectionControl(const bool theSelectionControl)
{
  // hide current widget, activate the next widget
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myModule->workshop());
  XGUI_PropertyPanel* aPanel = aWorkshop->propertyPanel();
  const QList<ModuleBase_ModelWidget*>& aWidgets = aPanel->modelWidgets();
  foreach(ModuleBase_ModelWidget* aWidget, aWidgets) {
    if (theSelectionControl) { // hide other controls
      if (aWidget != this)
        aWidget->setVisible(false);
    }
    else { // hide current control
      if (aWidget == this)
        aWidget->setVisible(false);
      else
        aWidget->setVisible(true);
    }
  }

  if (theSelectionControl) {
    bool aBodyIsVisualized = myPreviewPlanes->hasVisualizedBodies(myWorkshop);
    if (!aBodyIsVisualized) {
      // We have to select a plane before any operation
      myPreviewPlanes->showPreviewPlanes(myWorkshop);
    }
  } else {
    bool aHidePreview = myPreviewPlanes->isPreviewDisplayed();
    myPreviewPlanes->showPreviewPlanes(myWorkshop);
    if (aHidePreview)
      aWorkshop->viewer()->update();
  }
}

QIntList PartSet_WidgetSketchCreator::getShapeTypes() const
{
  QIntList aShapeTypes;
  foreach(QString aType, myShapeTypes) {
    aShapeTypes.append(ModuleBase_Tools::shapeType(aType));
  }
  return aShapeTypes;
}

void PartSet_WidgetSketchCreator::setEditingMode(bool isEditing)
{
  ModuleBase_ModelWidget::setEditingMode(isEditing);
  if (isEditing)
    setVisibleSelectionControl(false);
}

bool PartSet_WidgetSketchCreator::isSelectionMode() const
{
  AttributeSelectionListPtr anAttrList = myFeature->data()->selectionList(myAttributeListID);
  bool aHasValueInList = anAttrList.get() && anAttrList->size() > 0;

  return !aHasValueInList;
}

void PartSet_WidgetSketchCreator::onSelectionChanged()
{
  QList<ModuleBase_ViewerPrs> aSelected = getFilteredSelected();

  if (!startSketchOperation(aSelected)) {
    QList<ModuleBase_ViewerPrs>::const_iterator anIt = aSelected.begin(), aLast = aSelected.end();
    bool aProcessed = false;
    for (; anIt != aLast; anIt++) {
      ModuleBase_ViewerPrs aValue = *anIt;
      if (isValidInFilters(aValue))
        aProcessed = setSelectionCustom(aValue) || aProcessed;
    }
    if (aProcessed) {
      emit valuesChanged();
      updateObject(myFeature);
      setVisibleSelectionControl(false);
      // manually deactivation because the widget was not activated as has no focus acceptin controls
      deactivate();
      emit focusOutWidget(this);
    }
  }
}

//********************************************************************
void PartSet_WidgetSketchCreator::setObject(ObjectPtr theSelectedObject,
                                            GeomShapePtr theShape)
{
  std::string anAttributeId = myAttributeListID;
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(anAttributeId);
  if (anAttribute.get()) {
    std::string aType = anAttribute->attributeType();
    if (aType == ModelAPI_AttributeSelectionList::typeId()) {
      AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(anAttributeId);
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theSelectedObject);
      if (!aSelectionListAttr->isInList(aResult, theShape, myIsInValidate))
        aSelectionListAttr->append(aResult, theShape, myIsInValidate);
    }
  }
}

bool PartSet_WidgetSketchCreator::startSketchOperation(const QList<ModuleBase_ViewerPrs>& theValues)
{
  bool aSketchStarted = false;

  if (theValues.size() != 1)
    return aSketchStarted;

  ModuleBase_ViewerPrs aValue = theValues.front();
  if (!PartSet_WidgetSketchLabel::canFillSketch(aValue))
    return aSketchStarted;

  aSketchStarted = true;

  // manually deactivation because the widget was not activated as has no focus acceptin controls
  deactivate();
  bool aHidePreview = myPreviewPlanes->isPreviewDisplayed();
  myPreviewPlanes->erasePreviewPlanes(myWorkshop);

  // Launch Sketch operation
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);

  /// add sketch feature without current feature change.
  /// it is important to do not change the current feature in order to
  /// after sketch edition, the extrusion cut feature becomes current
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->activeDocument();
  FeaturePtr aPreviousCurrentFeature = aDoc->currentFeature(false);
  FeaturePtr aSketch = aCompFeature->addFeature("Sketch");

  PartSet_WidgetSketchLabel::fillSketchPlaneBySelection(aSketch, aValue);

  aDoc->setCurrentFeature(aPreviousCurrentFeature, false);

  // start edit operation for the sketch
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                            (myModule->createOperation("Sketch"));
  if (aFOperation)
    aFOperation->setFeature(aSketch);
  myModule->sendOperation(aFOperation);

  return aSketchStarted;
}

bool PartSet_WidgetSketchCreator::focusTo()
{
  if (isSelectionMode()) {
    setVisibleSelectionControl(true);

    // we need to call activate here as the widget has no focus accepted controls
    // if these controls are added here, activate will happens automatically after focusIn()
    activateCustom();
    return true;
  }
  else {
    setVisibleSelectionControl(false);

    connect(myModule, SIGNAL(resumed(ModuleBase_Operation*)), SLOT(onResumed(ModuleBase_Operation*)));
    SessionPtr aMgr = ModelAPI_Session::get();
    // Open transaction that is general for the previous nested one: it will be closed on nested commit
    bool aIsOp = aMgr->isOperation();
    if (!aIsOp) {
      const static std::string aNestedOpID("Parameters modification");
      aMgr->startOperation(aNestedOpID, true);
    }
    restoreValue();
  }
  return false;
}

void PartSet_WidgetSketchCreator::onResumed(ModuleBase_Operation* theOp)
{
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  CompositeFeaturePtr aSketchFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCompFeature->subFeature(0));
  if (aSketchFeature->numberOfSubs() == 0) {
    // do nothing, selection control should be shown

    // Abort operation
    //SessionPtr aMgr = ModelAPI_Session::get();
    // Close transaction
    /*
    bool aIsOp = aMgr->isOperation();
    if (aIsOp) {
      const static std::string aNestedOpID("Parameters cancelation");
      aMgr->startOperation(aNestedOpID, true);
    }
    */
    //theOp->abort();
  } else {
    // Update value in attribute selection list
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myModule->workshop());
    XGUI_PropertyPanel* aPanel = aWorkshop->propertyPanel();
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPanel->modelWidgets();
    foreach(ModuleBase_ModelWidget* aWidget, aWidgets) {
      if (aWidget->attributeID() == myAttributeListID)
        aWidget->restoreValue();
    }

    // Hide sketcher result
    std::list<ResultPtr> aResults = aSketchFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      (*aIt)->setDisplayed(false);
    }
    aSketchFeature->setDisplayed(false);

    // restore value in the selection control


    // Add Selected body were created the sketcher to list of selected objects
    std::string anObjectsAttribute = FeaturesPlugin_CompositeBoolean::BOOLEAN_OBJECTS_ID();
    AttributeSelectionListPtr aSelList = aCompFeature->data()->selectionList(anObjectsAttribute);
    if (aSelList.get()) {
      DataPtr aData = aSketchFeature->data();
      AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                    (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
      ResultPtr aRes = aSelAttr.get() ? aSelAttr->context() : ResultPtr();
      if (aRes.get()) {
        SessionPtr aMgr = ModelAPI_Session::get();
        ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
        AttributePtr anAttribute = myFeature->attribute(anObjectsAttribute);
        std::string aValidatorID, anError;
        aSelList->append(aRes, GeomShapePtr());
        if (aFactory->validate(anAttribute, aValidatorID, anError))
          updateObject(aCompFeature);
        else
          aSelList->clear();
      }
    }
  }
}
