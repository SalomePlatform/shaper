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
#include <QMainWindow>

PartSet_WidgetSketchCreator::PartSet_WidgetSketchCreator(QWidget* theParent, 
                                                         PartSet_Module* theModule,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetSelector(theParent, theModule->workshop(), theData),
  myModule(theModule), myIsCustomAttribute(false)
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

AttributePtr PartSet_WidgetSketchCreator::attribute() const
{
  AttributePtr anAttribute;
  if (myIsCustomAttribute)
    anAttribute = myFeature->attribute(myAttributeListID);
  else
    anAttribute = ModuleBase_WidgetSelector::attribute();

  return anAttribute;
}

//********************************************************************
bool PartSet_WidgetSketchCreator::isValidSelection(const ModuleBase_ViewerPrs& theValue)
{
  bool aValid = false;
  if (getValidState(theValue, aValid)) {
    return aValid;
  }
  // check selection to create new sketh (XML current attribute)
  aValid = isValidSelectionForAttribute(theValue, attribute());
  if (!aValid) {
    // check selection to fill list attribute (myAttributeListID)
    myIsCustomAttribute = true;
    aValid = isValidSelectionForAttribute(theValue, attribute());
    myIsCustomAttribute = false;
  }
  storeValidState(theValue, aValid);
  return aValid;
}

void PartSet_WidgetSketchCreator::activateSelectionControl()
{
  setVisibleSelectionControl(true);

  // we need to call activate here as the widget has no focus accepted controls
  // if these controls are added here, activate will happens automatically after focusIn()
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myModule->workshop());
  XGUI_PropertyPanel* aPanel = aWorkshop->propertyPanel();
  aPanel->activateWidget(this, false);
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
    myPreviewPlanes->erasePreviewPlanes(myWorkshop);
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

bool PartSet_WidgetSketchCreator::canCommitCurrentSketch(ModuleBase_IWorkshop* theWorkshop)
{
  bool aCanCommit = true;
  ModuleBase_Operation* anOperation = theWorkshop->currentOperation();
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(theWorkshop);
  XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();
  // check if the operation is nested
  if (anOperation && anOpMgr->operationsCount() > 1) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(anOperation);
    FeaturePtr aCurrentFeature = aFOperation ? aFOperation->feature() : FeaturePtr();

    ModuleBase_Operation* aPOperation =  anOpMgr->previousOperation(anOperation);
    ModuleBase_OperationFeature* aFPOperation = dynamic_cast<ModuleBase_OperationFeature*>(aPOperation);
    FeaturePtr aParentFeature = aFPOperation ? aFPOperation->feature() : FeaturePtr();

    CompositeFeaturePtr aCompositeFeature = 
                             std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCurrentFeature);
    CompositeFeaturePtr aPCompositeFeature = 
                             std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aParentFeature);
    // check if both features are composite: extrusion and sketch
    if (aCompositeFeature.get() && aPCompositeFeature.get()) {
      // selection attribute list is currently filled in execute(), so we need to call it
      // if there is no opened transaction, it should be started and finished
      SessionPtr aMgr = ModelAPI_Session::get();
      bool aIsOp = aMgr->isOperation();
      if (!aIsOp)
        aMgr->startOperation();
      aPCompositeFeature->execute(); // to fill attribute selection list

      std::list<AttributePtr> aSelListAttributes = aParentFeature->data()->attributes(
                                                        ModelAPI_AttributeSelectionList::typeId());
      if (aSelListAttributes.size() == 1) {
        AttributePtr aFirstAttribute = aSelListAttributes.front();

        SessionPtr aMgr = ModelAPI_Session::get();
        ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
        std::string aValidatorID, anError;
        bool isValidPComposite = aFactory->validate(aFirstAttribute, aValidatorID, anError);
        if (!isValidPComposite) {
          int anAnswer = QMessageBox::question(
              aWorkshop->desktop(), tr("Apply current feature"),
                            tr("The current feature can not be used as an argument of the parent feature.\n\
                               After apply it will be deleted. Would you like to continue?"),
                            QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
          if (anAnswer == QMessageBox::Ok)
            aCanCommit = true;
          else
            aCanCommit = false;
        }
      }
      if (!aIsOp) {
        if (aCanCommit)
          aMgr->finishOperation();
        else
          aMgr->abortOperation();
      }
    }
  }
  return aCanCommit;
}

bool PartSet_WidgetSketchCreator::isSelectionMode() const
{
  AttributeSelectionListPtr anAttrList = myFeature->data()->selectionList(myAttributeListID);
  bool aHasValueInList = anAttrList.get() && anAttrList->size() > 0;

  return !aHasValueInList;
}

bool PartSet_WidgetSketchCreator::setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                                               const bool theToValidate)
{
  bool aDone = false;
  if (!startSketchOperation(theValues)) {
    myIsCustomAttribute = true;
    QList<ModuleBase_ViewerPrs>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
    bool aProcessed = false;
    for (; anIt != aLast; anIt++) {
      ModuleBase_ViewerPrs aValue = *anIt;
      if (!theToValidate || isValidInFilters(aValue))
        aProcessed = setSelectionCustom(aValue) || aProcessed;
    }
    myIsCustomAttribute = true;
    aDone = aProcessed;
    if (aProcessed) {
      emit valuesChanged();
      updateObject(myFeature);
      setVisibleSelectionControl(false);
      // manually deactivation because the widget was not activated as has no focus acceptin controls
      deactivate();
      emit focusOutWidget(this);
    }
  }
  return aDone;
}

//********************************************************************
void PartSet_WidgetSketchCreator::onSelectionChanged()
{
  QList<ModuleBase_ViewerPrs> aSelected = getFilteredSelected();
  bool isDone = setSelection(aSelected, true/*false*/);
}

//********************************************************************
void PartSet_WidgetSketchCreator::updateOnSelectionChanged(const bool theDone)
{
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
    activateSelectionControl();

    SessionPtr aMgr = ModelAPI_Session::get();
    bool aIsOp = aMgr->isOperation();
    if (!aIsOp)
      aMgr->startOperation(myFeature->getKind());
    return true;
  }
  else {
    //setVisibleSelectionControl(false);

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

void PartSet_WidgetSketchCreator::deactivate()
{
  ModuleBase_WidgetSelector::deactivate();

  bool aHidePreview = myPreviewPlanes->isPreviewDisplayed();
  myPreviewPlanes->erasePreviewPlanes(myWorkshop);
  if (aHidePreview)
    XGUI_Tools::workshop(myWorkshop)->viewer()->update();

}

void PartSet_WidgetSketchCreator::onResumed(ModuleBase_Operation* theOp)
{
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myModule->workshop());

  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  //CompositeFeaturePtr aSketchFeature = 
  //  std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCompFeature->subFeature(0));
  if (aCompFeature->numberOfSubs() == 0) {
    // do nothing, selection control should be hidden
    setVisibleSelectionControl(false);
  } else {
    // check if the created sketch is invalid. Validate attribute selection list
    // Shetch should be deleted if the attribute is invalid.
    AttributeSelectionListPtr anAttrList = myFeature->data()->selectionList(myAttributeListID);
    
    SessionPtr aMgr = ModelAPI_Session::get();
    ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
    std::string aValidatorID, anError;
    bool isValidPComposite = aFactory->validate(anAttrList, aValidatorID, anError);
    /// if the sketch is not appropriate fro extrusion, it should be deleted and
    /// the selection control should be activated again
    if (!isValidPComposite) {
      CompositeFeaturePtr aSketchFeature = 
               std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCompFeature->subFeature(0));

      QObjectPtrList anObjects;
      anObjects.append(aSketchFeature);
      std::set<FeaturePtr> anIgnoredFeatures;
      aWorkshop->deleteFeatures(anObjects, anIgnoredFeatures);

      // do nothing, selection control should be shown
      activateSelectionControl();
    }
    else {
      setVisibleSelectionControl(false);
      // Update value in attribute selection list
      XGUI_PropertyPanel* aPanel = aWorkshop->propertyPanel();
      const QList<ModuleBase_ModelWidget*>& aWidgets = aPanel->modelWidgets();
      foreach(ModuleBase_ModelWidget* aWidget, aWidgets) {
        if (aWidget->attributeID() == myAttributeListID)
          aWidget->restoreValue();
      }

      // Hide sketcher result
      CompositeFeaturePtr aSketchFeature = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCompFeature->subFeature(0));
      std::list<ResultPtr> aResults = aSketchFeature->results();
      std::list<ResultPtr>::const_iterator aIt;
      for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
        (*aIt)->setDisplayed(false);
      }
      aSketchFeature->setDisplayed(false);
      static Events_Loop* aLoop = Events_Loop::loop();
      aLoop->flush(aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY));

      // Add Selected body were created the sketcher to list of selected objects
      std::string anObjectsAttribute = FeaturesPlugin_CompositeBoolean::OBJECTS_ID();
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
}
