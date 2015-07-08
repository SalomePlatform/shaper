// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchCreator.cpp
// Created:     08 June 2015
// Author:      Vitaly SMETANNIKOV

#include "PartSet_WidgetSketchCreator.h"
#include "PartSet_Module.h"

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_OperationMgr.h>

#include <GeomAPI_Face.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <SketchPlugin_SketchEntity.h>
#include <FeaturesPlugin_CompositeBoolean.h>

#include <ModuleBase_Tools.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_IPropertyPanel.h>
#include <Config_WidgetAPI.h>

#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox>

PartSet_WidgetSketchCreator::PartSet_WidgetSketchCreator(QWidget* theParent, 
                                                         PartSet_Module* theModule,
                                                         const Config_WidgetAPI* theData,
                                                         const std::string& theParentId)
: ModuleBase_ModelWidget(theParent, theData, theParentId), myModule(theModule)
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
}

PartSet_WidgetSketchCreator::~PartSet_WidgetSketchCreator()
{
}

QList<QWidget*> PartSet_WidgetSketchCreator::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myTextLine);
  return aControls;
}

bool PartSet_WidgetSketchCreator::restoreValue()
{
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aCompFeature->numberOfSubs() > 0) {
    FeaturePtr aSubFeature = aCompFeature->subFeature(0);
    myTextLine->setText(QString::fromStdString(aSubFeature->data()->name()));
  }
  return true;
}

bool PartSet_WidgetSketchCreator::storeValueCustom() const
{
  return true;
}

void PartSet_WidgetSketchCreator::activateCustom()
{
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aCompFeature->numberOfSubs() == 0)
    connect(myModule, SIGNAL(operationLaunched()), SLOT(onStarted()));
}

void PartSet_WidgetSketchCreator::onStarted()
{
  disconnect(myModule, SIGNAL(operationLaunched()), this, SLOT(onStarted()));

  // Check that model already has bodies
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_Displayer* aDisp = aWorkshop->displayer();
  QObjectPtrList aObjList = aDisp->displayedObjects();
  bool aHasBody = false;
  ResultBodyPtr aBody;
  foreach(ObjectPtr aObj, aObjList) {
    aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aObj);
    if (aBody.get() != NULL) {
      aHasBody = true;
      break;
    }
  }

  if (aHasBody) {
    // Launch Sketch operation
    CompositeFeaturePtr aCompFeature = 
      std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
    FeaturePtr aSketch = aCompFeature->addFeature("Sketch");

    ModuleBase_Operation* anOperation = myModule->createOperation("Sketch");
    anOperation->setFeature(aSketch);
    myModule->sendOperation(anOperation);
    //connect(anOperation, SIGNAL(aborted()), aWorkshop->operationMgr(), SLOT(abortAllOperations()));
  } else {
    // Break current operation
    QMessageBox::warning(this, tr("Extrusion Cut"),
        tr("There are no bodies found. Operation aborted."), QMessageBox::Ok);
    ModuleBase_Operation* aOp = myModule->workshop()->currentOperation();
    aOp->abort();
  }
}

bool PartSet_WidgetSketchCreator::focusTo()
{
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aCompFeature->numberOfSubs() == 0)
    return ModuleBase_ModelWidget::focusTo(); 

  connect(myModule, SIGNAL(operationResumed(ModuleBase_Operation*)), SLOT(onResumed(ModuleBase_Operation*)));
  SessionPtr aMgr = ModelAPI_Session::get();
  bool aIsOp = aMgr->isOperation();
  // Open transaction if it was closed before
  if (!aIsOp)
    aMgr->startOperation();

  restoreValue();
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
    bool aIsOp = aMgr->isOperation();
    // Close transaction
    if (aIsOp)
      aMgr->abortOperation();
    theOp->abort();
  } else {
    // Hide sketcher result
    std::list<ResultPtr> aResults = aSketchFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      (*aIt)->setDisplayed(false);
    }
    aSketchFeature->setDisplayed(false);

    // Add Selected body were created the sketcher to list of selected objects
    DataPtr aData = aSketchFeature->data();
    AttributeSelectionPtr aSelAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
      (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
    if (aSelAttr.get()) {
      ResultPtr aRes = aSelAttr->context();
      GeomShapePtr aShape = aSelAttr->value();
      if (aRes.get()) {
        AttributeSelectionListPtr aSelList = 
          aCompFeature->data()->selectionList(FeaturesPlugin_CompositeBoolean::BOOLEAN_OBJECTS_ID());
        aSelList->append(aRes, GeomShapePtr());
        updateObject(aCompFeature);
      }
    }
  }
}
