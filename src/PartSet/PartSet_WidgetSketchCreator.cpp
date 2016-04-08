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
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_SketchEntity.h>
#include <FeaturesPlugin_CompositeBoolean.h>

#include <ModuleBase_Tools.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_ViewerPrs.h>

#include <Config_WidgetAPI.h>

#include <Events_Loop.h>

#include <QLabel>
#include <QLineEdit>
//#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QMainWindow>

#define DEBUG_UNDO_INVALID_SKETCH

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
  aLayout->addStretch(1);

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ', QString::SkipEmptyParts);

  myPreviewPlanes = new PartSet_PreviewPlanes();
}

PartSet_WidgetSketchCreator::~PartSet_WidgetSketchCreator()
{
  // we need to deactivate here in order to hide preview planes if the selection mode is
  // active
  deactivate();
}

QList<QWidget*> PartSet_WidgetSketchCreator::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myLabel);
  return aControls;
}

bool PartSet_WidgetSketchCreator::restoreValueCustom()
{
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
void PartSet_WidgetSketchCreator::openExtrusionTransaction()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  bool aIsOp = aMgr->isOperation();
  if (!aIsOp) {
    const static std::string aNestedOpID("Parameters modification");
    aMgr->startOperation(aNestedOpID, true);
  }
}

//********************************************************************
bool PartSet_WidgetSketchCreator::isValidSelection(const ModuleBase_ViewerPrsPtr& theValue)
{
  bool aValid = false;
  if (myIsCustomAttribute) {
    // check only suiting of the value to custom attribute (myAttributeListID)
    // do not cash of validation to avoid using states, stored for XML attribute
    // there is an alternative is to call clearValidatedCash() in setSelection()
    aValid = isValidSelectionForAttribute(theValue, attribute());
  }
  else { /// if the validated attribute is already custom
    if (getValidState(theValue, aValid)) {
      return aValid;
    }
    aValid = isValidSelectionCustom(theValue);
    if (!aValid)
      // check selection to create new sketh (XML current attribute)
      aValid = isValidSelectionForAttribute(theValue, attribute());
    if (!aValid) {
      // check selection to fill list attribute (myAttributeListID)
      bool isCustomAttribute = myIsCustomAttribute;
      myIsCustomAttribute = true;
      aValid = isValidSelectionForAttribute(theValue, attribute());
      myIsCustomAttribute = isCustomAttribute;
    }
  }
  storeValidState(theValue, aValid);
  return aValid;
}

//********************************************************************
bool PartSet_WidgetSketchCreator::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& theValue)
{
  return PartSet_WidgetSketchLabel::canFillSketch(theValue);
}

void PartSet_WidgetSketchCreator::activateSelectionControl()
{
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
      else {
        aWidget->setVisible(true);
        if (aWidget->attributeID() == myAttributeListID)
          setEnabledModelWidget(aWidget, !hasSubObjects());
      }
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
  if (isEditing) {
    setVisibleSelectionControl(false);

    ModuleBase_ModelWidget* anAttributeListWidget = 0;
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myModule->workshop());
    XGUI_PropertyPanel* aPanel = aWorkshop->propertyPanel();
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPanel->modelWidgets();
    foreach(ModuleBase_ModelWidget* aWidget, aWidgets) {
      if (aWidget->attributeID() == myAttributeListID) {
        anAttributeListWidget = aWidget;
        break;
      }
    }
    if (anAttributeListWidget)
      setEnabledModelWidget(anAttributeListWidget, !hasSubObjects());
  }
}

bool PartSet_WidgetSketchCreator::isSelectionMode() const
{
  AttributeSelectionListPtr anAttrList = myFeature->data()->selectionList(myAttributeListID);
  bool aHasValueInList = anAttrList.get() && anAttrList->size() > 0;

  return !aHasValueInList;
}

bool PartSet_WidgetSketchCreator::hasSubObjects() const
{
  bool aHasSubObjects = false;

  bool aCanSetFocus = true;
  CompositeFeaturePtr aComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aComposite.get())
    aHasSubObjects = aComposite->numberOfSubs() > 0;
  return aHasSubObjects;
}

bool PartSet_WidgetSketchCreator::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                               const bool theToValidate)
{
  bool aDone = false;
  if (!startSketchOperation(theValues)) {
    myIsCustomAttribute = true;
    QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
    bool aProcessed = false;
    for (; anIt != aLast; anIt++) {
      ModuleBase_ViewerPrsPtr aValue = *anIt;
      if (!theToValidate || isValidInFilters(aValue))
        aProcessed = setSelectionCustom(aValue) || aProcessed;
    }
    myIsCustomAttribute = false;
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
  QList<ModuleBase_ViewerPrsPtr> aSelected = getFilteredSelected();
  bool isDone = setSelection(aSelected, true/*false*/);
}

//********************************************************************
void PartSet_WidgetSketchCreator::updateOnSelectionChanged(const bool theDone)
{
}

bool PartSet_WidgetSketchCreator::startSketchOperation(const QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  bool aSketchStarted = false;

  if (theValues.size() != 1)
    return aSketchStarted;

  ModuleBase_ViewerPrsPtr aValue = theValues.front();
  if (!aValue.get() || !PartSet_WidgetSketchLabel::canFillSketch(aValue))
    return aSketchStarted;

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aValue->object());
  /// sketch should not started by object(face) selected as global. If Local face is selected,
  /// sketch is started
  if (aResult.get() && aValue->shape().get() && aResult->shape()->isEqual(aValue->shape())) {
    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aResult);
    if (!aConstruction.get() || !aConstruction->isInfinite())
      return aSketchStarted;
  }
  aSketchStarted = true;

  // manually deactivation because the widget was not activated as has no focus acceptin controls
  deactivate();
  bool aHidePreview = myPreviewPlanes->isPreviewDisplayed();
  myPreviewPlanes->erasePreviewPlanes(myWorkshop);

  // Launch Sketch operation
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);

  // start edit operation for the sketch
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                            (myModule->createOperation("Sketch"));
  QList<ModuleBase_ViewerPrsPtr> aValues;
  aValues.push_back(aValue);
  aFOperation->setPreselection(aValues);

  myModule->sendOperation(aFOperation);

  return aSketchStarted;
}

bool PartSet_WidgetSketchCreator::focusTo()
{
  // this method is called only in creation mode. In Edition mode this widget is hidden
  if (isSelectionMode() && !hasSubObjects()) {
    setVisibleSelectionControl(true);
    activateSelectionControl();
    openExtrusionTransaction();
    return true;
  }
  else
    connect(myModule, SIGNAL(resumed(ModuleBase_Operation*)), SLOT(onResumed(ModuleBase_Operation*)));

  return true;
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
  SessionPtr aMgr = ModelAPI_Session::get();
  bool aIsOp = aMgr->isOperation();
  if (aIsOp) {
    // in current implementation, all transactions are closed when resume happens
    // so, this is a wrong case, which is not checked.
    // To provide it, make correction in later rows about abort/undo transactions
    return;
  }
  // Set visible only selection control
  setVisibleSelectionControl(true);

  // Validate the created sketch. If it is valid, it is set into the composite feature selection
  // list, otherwise it is removed
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  AttributeSelectionListPtr anAttrList = myFeature->data()->selectionList(myAttributeListID);
  if (aCompFeature->numberOfSubs() > 0) {
    // set the sub feature to attribute selection list and check whether sketch is valid
    SessionPtr aMgr = ModelAPI_Session::get();
    const static std::string aNestedOpID("Set Sketch result into Selection list");
    aMgr->startOperation(aNestedOpID, false); // false to not attach to Extrusion operation
    setSketchObjectToList(aCompFeature, anAttrList);
    aMgr->finishOperation();

    if (!validateSelectionList()) {
#ifdef DEBUG_UNDO_INVALID_SKETCH
      aMgr->undo(); // Extrusion modification parameters: setSketchObjectToList()
      aMgr->undo(); /// Sketch creation
#else
      aMgr->startOperation("Delete invalid Sketch feature", false);

      // delete invalid sketch
      CompositeFeaturePtr aSketchFeature = 
              std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCompFeature->subFeature(0));
      QObjectPtrList anObjects;
      anObjects.append(aSketchFeature);

      XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myModule->workshop());
      aWorkshop->deleteFeatures(anObjects, std::set<FeaturePtr>());

      aMgr->finishOperation();
#endif
    }
  }
  openExtrusionTransaction();

  if (aCompFeature->numberOfSubs() == 0) {
    // call activateWidget() of the parent to connect to the viewer seleciton
    activateSelectionControl();
  }
  else {
    // check if the created sketch is valid. If it is invalid, it will be deleted with warning else
    /// the attribute selection list will be filled by result of this sketch.
    setVisibleSelectionControl(false);

    // Update value in attribute selection list
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myModule->workshop());
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
  restoreValue();
}

bool PartSet_WidgetSketchCreator::validateSelectionList() const
{
  AttributeSelectionListPtr anAttrList = myFeature->data()->selectionList(myAttributeListID);

  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::string aValidatorID, anError;
  bool isValidPComposite = aFactory->validate(anAttrList, aValidatorID, anError);
  if (!isValidPComposite) {
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myModule->workshop());
    QMessageBox::question(aWorkshop->desktop(), tr("Apply current feature"),
                  tr("Sketch is invalid and will be deleted.\nError: %1").arg(anError.c_str()),
                  QMessageBox::Ok);
  }
  return isValidPComposite;
}

void PartSet_WidgetSketchCreator::setSketchObjectToList(const CompositeFeaturePtr& theCompositeFeature,
                                                        const AttributePtr& theAttribute)
{
  if (!theCompositeFeature.get() || theCompositeFeature->numberOfSubs() != 1)
    return;

  AttributeSelectionListPtr aBaseObjectsSelectionList =
                     std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!aBaseObjectsSelectionList.get() || aBaseObjectsSelectionList->isInitialized()) {
    return;
  }

  FeaturePtr aSketchFeature = theCompositeFeature->subFeature(0);
  if(!aSketchFeature.get() || aSketchFeature->results().empty()) {
    return;
  }

  ResultPtr aSketchRes = aSketchFeature->results().front();
  ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSketchRes);
  if(!aConstruction.get()) {
    return;
  }

  if(aBaseObjectsSelectionList->size() == 0) {
    aBaseObjectsSelectionList->append(aSketchRes, GeomShapePtr());
  }
}

void PartSet_WidgetSketchCreator::setEnabledModelWidget(ModuleBase_ModelWidget* theModelWidget,
                                                        const bool theEnabled)
{
  QList<QWidget*> aMyControls = theModelWidget->getControls();
  foreach(QWidget*  eachControl, aMyControls) {
    eachControl->setEnabled(theEnabled);
  }
}
