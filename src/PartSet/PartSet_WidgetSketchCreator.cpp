// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchCreator.cpp
// Created:     08 June 2015
// Author:      Vitaly SMETANNIKOV

#include "PartSet_WidgetSketchCreator.h"
#include "PartSet_Module.h"
#include "PartSet_WidgetSketchLabel.h"

#include <Config_Keywords.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_PropertyPanel.h>

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
  myModule(theModule), myUseBody(true)
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

  QString aUseBody = QString::fromStdString(theData->getProperty(USE_BODY));
  if(!aUseBody.isEmpty()) {
    myUseBody = QVariant(aUseBody).toBool();
  }

  aLayout->addRow(myLabel, myTextLine);*/

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ', QString::SkipEmptyParts);
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

void PartSet_WidgetSketchCreator::activateCustom()
{
  if (isSelectionMode()) {
    ModuleBase_WidgetSelector::activateCustom();
    //connect(myModule, SIGNAL(operationLaunched()), SLOT(onStarted()));

    //setVisibleSelectionControl(true);
  }
  //else {
  //  setVisibleSelectionControl(false);
  //  emit focusOutWidget(this);
  //}
}

void PartSet_WidgetSketchCreator::setVisibleSelectionControl(const bool theSelectionControl)
{
  // hide current widget, activate the next widget
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
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
}

QIntList PartSet_WidgetSketchCreator::getShapeTypes() const
{
  QIntList aShapeTypes;
  foreach(QString aType, myShapeTypes) {
    aShapeTypes.append(ModuleBase_Tools::shapeType(aType));
  }
  return aShapeTypes;
}

void PartSet_WidgetSketchCreator::deactivate()
{
  if (isSelectionMode()) {
    ModuleBase_WidgetSelector::activateCustom();
  }
}

bool PartSet_WidgetSketchCreator::isSelectionMode() const
{
  //CompositeFeaturePtr aCompFeature =
  //  std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  //bool aHasSub = aCompFeature->numberOfSubs() > 0;

  AttributeSelectionListPtr anAttrList = myFeature->data()->selectionList(myAttributeListID);
  bool aHasValueInList = anAttrList.get() && anAttrList->size() > 0;

  return !aHasValueInList;//aHasSub || aHasValueInList;
}

void PartSet_WidgetSketchCreator::onSelectionChanged()
{
  QList<ModuleBase_ViewerPrs> aSelected = getFilteredSelected();

  if (aSelected.size() == 1) { // plane or planar face of not sketch object
    startSketchOperation(aSelected.front());
  }
  else if (aSelected.size() > 1) {
    QList<ModuleBase_ViewerPrs>::const_iterator anIt = aSelected.begin(), aLast = aSelected.end();
    bool aProcessed;
    for (; anIt != aLast; anIt++) {
      ModuleBase_ViewerPrs aValue = *anIt;
      if (isValidInFilters(aValue))
        aProcessed = setSelectionCustom(aValue);
    }
    if (aProcessed) {
      emit valuesChanged();
      updateObject(myFeature);
      setVisibleSelectionControl(false);
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
void PartSet_WidgetSketchCreator::onStarted()
{
  disconnect(myModule, SIGNAL(operationLaunched()), this, SLOT(onStarted()));

  setVisibleSelectionControl(true);
}

void PartSet_WidgetSketchCreator::startSketchOperation(const ModuleBase_ViewerPrs& theValue)
{
  // Check that model already has bodies
  /*XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_Displayer* aDisp = aWorkshop->displayer();
  QObjectPtrList aObjList = aDisp->displayedObjects();
  bool aHasBody = !myUseBody;
  ResultBodyPtr aBody;
  if(!aHasBody) {
    foreach(ObjectPtr aObj, aObjList) {
      aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aObj);
      if (aBody.get() != NULL) {
        aHasBody = true;
        break;
      }
    }
  }*/

  //if (aHasBody) {
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

    PartSet_WidgetSketchLabel::fillSketchPlaneBySelection(aSketch, theValue);

    aDoc->setCurrentFeature(aPreviousCurrentFeature, false);

    // start edit operation for the sketch
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                             (myModule->createOperation("Sketch"));
    if (aFOperation)
      aFOperation->setFeature(aSketch);
    myModule->sendOperation(aFOperation);
    //connect(anOperation, SIGNAL(aborted()), aWorkshop->operationMgr(), SLOT(abortAllOperations()));
  //}
  /* else {
    // Break current operation
    std::string anOperationName = feature()->getKind();
    QString aTitle = tr( anOperationName.c_str() );
    QMessageBox::warning(this, aTitle,
        tr("There are no bodies found. Operation aborted."), QMessageBox::Ok);
    ModuleBase_Operation* aOp = myModule->workshop()->currentOperation();
    aOp->abort();
  }*/
}

bool PartSet_WidgetSketchCreator::focusTo()
{
  if (isSelectionMode()) {
    //CompositeFeaturePtr aCompFeature = 
    //   std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
    // if (aCompFeature->numberOfSubs() == 0)
    //   return ModuleBase_ModelWidget::focusTo(); 
    connect(myModule, SIGNAL(operationLaunched()), SLOT(onStarted()));
    // we need to call activate here as the widget has no focus accepted controls
    // if these controls are added here, activate will happens automatically after focusIn()
    activate();
    return true;
  }
  else {
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
    // Abort operation
    SessionPtr aMgr = ModelAPI_Session::get();
    // Close transaction
    /*
    bool aIsOp = aMgr->isOperation();
    if (aIsOp) {
      const static std::string aNestedOpID("Parameters cancelation");
      aMgr->startOperation(aNestedOpID, true);
    }
    */
    theOp->abort();
  } else {
    // Hide sketcher result
    std::list<ResultPtr> aResults = aSketchFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      (*aIt)->setDisplayed(false);
    }
    aSketchFeature->setDisplayed(false);

    if(myUseBody) {
      // Add Selected body were created the sketcher to list of selected objects
      DataPtr aData = aSketchFeature->data();
      AttributeSelectionPtr aSelAttr = 
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
        (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
      if (aSelAttr.get()) {
        ResultPtr aRes = aSelAttr->context();
        GeomShapePtr aShape = aSelAttr->value();
        if (aRes.get()) {
          std::string anObjectsAttribute = FeaturesPlugin_CompositeBoolean::BOOLEAN_OBJECTS_ID();
          SessionPtr aMgr = ModelAPI_Session::get();
          ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
          AttributePtr anAttribute = myFeature->attribute(anObjectsAttribute);
          std::string aValidatorID, anError;
          AttributeSelectionListPtr aSelList = aCompFeature->data()->selectionList(anObjectsAttribute);
          aSelList->append(aRes, GeomShapePtr());
          if (aFactory->validate(anAttribute, aValidatorID, anError))
            updateObject(aCompFeature);
          else
            aSelList->clear();
        }
      }
    }
    else {
      // this is a workarount to display the feature results in the operation selection mode
      // if this is absent, sketch point/line local selection is available on extrusion cut result
      static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
      ModelAPI_EventCreator::get()->sendUpdated(feature(), anUpdateEvent);
      updateObject(feature());
    }
  }
}
